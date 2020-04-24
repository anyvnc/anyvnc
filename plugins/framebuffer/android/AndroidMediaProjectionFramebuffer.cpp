/*
 * AndroidMediaProjectionFramebuffer.cpp
 *
 * Copyright (c) 2020 Tobias Junghans <tobydox@veyon.io>
 *
 * This file is part of AnyVNC - https://anyvnc.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <QColor>
#include <QElapsedTimer>
#include <QRect>
#include <QThread>

#include "libanyvnc/qt/core/AnyVncQt.h"
#include "AndroidMediaProjectionFramebuffer.h"

namespace AnyVnc
{

template<AndroidMediaProjectionFramebuffer::AndroidPixelFormat>
QRgb pixelToRgb( const uchar* scanLine, int x );

template<>
QRgb pixelToRgb<AndroidMediaProjectionFramebuffer::AndroidPixelFormat::RGBA_8888>( const uchar* scanLine, int x )
{
	return qRgb( scanLine[x*4+0], scanLine[x*4+1], scanLine[x*4+2] );
}


template<>
QRgb pixelToRgb<AndroidMediaProjectionFramebuffer::AndroidPixelFormat::RGB_888>( const uchar* scanLine, int x )
{
	return qRgb( scanLine[x*3+0], scanLine[x*3+1], scanLine[x*3+2] );
}


template<>
QRgb pixelToRgb<AndroidMediaProjectionFramebuffer::AndroidPixelFormat::RGB_565>( const uchar* scanLine, int x )
{
	const auto pixel = reinterpret_cast<const uint16_t *>( scanLine )[x];
	return qRgb( pixel >> 11, ( pixel >> 5 ) & 63, pixel & 31 );
}


template<AndroidMediaProjectionFramebuffer::AndroidPixelFormat PIXEL_FORMAT>
bool convertAndScan( const uchar* sourceImageData, int sourceRowStride, int width, int height, QRgb* destination,
					AndroidMediaProjectionFramebuffer::RectVector* rects )
{
	Types::Rectangle currentRect;

	for( int y = 0; y < height; ++y )
	{
		int minX = -1, maxX = -1;
		const auto sourceScanLine = sourceImageData + y * sourceRowStride;
		auto destinationScanLine = destination + y * width;

		for( int x = 0; x < width; ++x )
		{
			const auto sourcePixel = pixelToRgb<PIXEL_FORMAT>( sourceScanLine, x );

			if( sourcePixel != destinationScanLine[x] )
			{
				destinationScanLine[x] = sourcePixel;
				if( minX < 0 )
				{
					minX = x;
				}
				maxX = x;
			}
		}

		if( minX >= 0 )
		{
			if( currentRect.isValid() )
			{
				if( minX < currentRect.left() )
				{
					currentRect.setLeft( minX );
				}
				if( maxX > currentRect.right() )
				{
					currentRect.setRight( maxX );
				}
				currentRect.setBottom( y );
			}
			else
			{
				currentRect = { minX, y, maxX, y };
			}
		}
		else if( currentRect.isValid() )
		{
			rects->append( currentRect );
			currentRect = {};
		}
	}

	if( currentRect.isValid() )
	{
		rects->append( currentRect );
		return true;
	}

	return false;
}



static bool updateBuffer( const uchar* sourceImageData,
				  AndroidMediaProjectionFramebuffer::AndroidPixelFormat sourceFormat,
				  int sourceRowStride, QRgb* data, Types::Size size,
				  AndroidMediaProjectionFramebuffer::RectVector* rects )
{
	switch( sourceFormat )
	{
	case AndroidMediaProjectionFramebuffer::AndroidPixelFormat::RGBA_8888:
	case AndroidMediaProjectionFramebuffer::AndroidPixelFormat::RGBX_8888:
		return convertAndScan<AndroidMediaProjectionFramebuffer::AndroidPixelFormat::RGBA_8888>(
			sourceImageData, sourceRowStride, size.width(), size.height(), data, rects );

	case AndroidMediaProjectionFramebuffer::AndroidPixelFormat::RGB_888:
		return convertAndScan<AndroidMediaProjectionFramebuffer::AndroidPixelFormat::RGB_888>(
			sourceImageData, sourceRowStride, size.width(), size.height(), data, rects );

	case AndroidMediaProjectionFramebuffer::AndroidPixelFormat::RGB_565:
		return convertAndScan<AndroidMediaProjectionFramebuffer::AndroidPixelFormat::RGB_565>(
			sourceImageData, sourceRowStride, size.width(), size.height(), data, rects );

	default:
		avqCritical() << "Invalid image format:" << static_cast<int>( sourceFormat );
		break;
	}

	return false;
}



bool AndroidMediaProjectionFramebuffer::initialize( Core::Server* )
{
	auto activity = QtAndroid::androidActivity();

	AJO::callStaticMethod<void>( "com/stardust/util/ScreenMetrics", "initIfNeeded",
								 "(Landroid/app/Activity;)V",
								 activity.object() );

	auto serviceName = AJO::getStaticObjectField<jstring>( "android.content.Context",
														   "MEDIA_PROJECTION_SERVICE");
	auto mediaProjectionManager = activity.callObjectMethod( "getSystemService",
															 "(Ljava/lang/String;)Ljava/lang/Object;",
															 serviceName.object<jstring>() );

	auto intent = mediaProjectionManager.callObjectMethod("createScreenCaptureIntent",
														   "()Landroid/content/Intent;");

	QtAndroid::startActivity( intent, RequestCodeCapturePermission, this );

	while( update( []( Types::Rectangle ) {} ) & UpdateFlag::Initializing )
	{
		QThread::msleep( 100 );
	}

	return true;
}



void* AndroidMediaProjectionFramebuffer::data() const
{
	return m_data;
}



Types::Size AndroidMediaProjectionFramebuffer::size() const
{
	return m_size;
}



AndroidMediaProjectionFramebuffer::UpdateFlags AndroidMediaProjectionFramebuffer::update(
		const Interfaces::Framebuffer::RectangleVisitor& visitor )
{
	RectVector rects;
	UpdateFlags updateFlags{};

	const auto state = readMediaProjectionBuffer( &rects );

	switch( state )
	{
	case BufferState::WaitingForCapturer: return UpdateFlags{ UpdateFlag::Initializing };
	case BufferState::InvalidImageSize: return UpdateFlags{ UpdateFlag::RequiresRestart };
	case BufferState::SizeChanged: updateFlags |= UpdateFlag::SizeChanged; break;
	case BufferState::Rotated: updateFlags |= UpdateFlag::SizeChanged; break;
	case BufferState::Ready: return UpdateFlags{ UpdateFlag::None };
	case BufferState::Updated:
		for( const auto& rect : qAsConst(rects) )
		{
			visitor( { rect.left(), rect.top(), rect.right(), rect.bottom() } );
		}
		break;
	}

	return updateFlags;
}



Types::Screens AndroidMediaProjectionFramebuffer::availableScreens() const
{
	return { { size(), 32 } };
}



void AndroidMediaProjectionFramebuffer::handleActivityResult( int receiverRequestCode, int resultCode, const QAndroidJniObject& data )
{
	if( receiverRequestCode == RequestCodeCapturePermission )
	{
		const auto resultOk = AJO::getStaticField<jint>( "android/app/Activity", "RESULT_OK");

		if( resultCode == resultOk )
		{
			const auto screenDensity = AJO::callStaticMethod<jint>( "com/stardust/util/ScreenMetrics",
																	"getDeviceScreenDensity",
																	"()I" );

			auto context = QtAndroid::androidContext();

			QMutexLocker locker( &m_screenCapturerMutex );

			m_screenCapturer = AJO( "com/stardust/autojs/core/image/capture/ScreenCapturer",
									"(Landroid/content/Context;Landroid/content/Intent;IILandroid/os/Handler;)V",
									context.object(),
									data.object(),
									0,
									screenDensity, nullptr );

			avqInfo() << "created ScreenCapturer";
		}
	}
}



AndroidMediaProjectionFramebuffer::BufferState AndroidMediaProjectionFramebuffer::readMediaProjectionBuffer( RectVector* rects )
{
	QElapsedTimer benchTimer;
	benchTimer.start();

	QAndroidJniEnvironment qjniEnv;
	QAndroidJniObject image;

	QMutexLocker locker( &m_screenCapturerMutex );

	avqDebug() << "Capturing image";

	if( rects &&
		m_screenCapturer.isValid() &&
		( image = m_screenCapturer.callObjectMethod( "capture",
													 "()Landroid/media/Image;" ) ).isValid() )
	{
		const auto imageWidth = image.callMethod<jint>("getWidth");
		const auto imageHeight = image.callMethod<jint>("getHeight");
		const size_t pixelCount = size_t( imageWidth * imageHeight );
		const auto imageFormat = static_cast<AndroidPixelFormat>( image.callMethod<jint>("getFormat") );

		if( imageWidth <= 0 || imageHeight <= 0  )
		{
			avqCritical() << "Invalid image size" << imageWidth << imageHeight;
			return BufferState::InvalidImageSize;
		}

		const Types::Size imageSize( imageWidth, imageHeight );

		if( m_data == nullptr )
		{
			avqInfo() << "initializing buffer";

			m_size = { imageSize.width(), imageSize.height() };
			m_data = new QRgb[pixelCount];
			memset( m_data, 0, pixelCount );
		}

		bool rotated{ false };

		// screen rotated?
		if( m_size.width() == imageHeight &&
			m_size.height() == imageWidth )
		{
			avqInfo() << "rotated" << imageWidth << imageHeight;
			m_size = imageSize;
			rotated = true;
		}
		else if( imageSize != m_size )
		{
			delete[] m_data;

			avqInfo() << "FB size changed";

			m_size = { imageSize.width(), imageSize.height() };
			m_data = new QRgb[pixelCount];
			memset( m_data, 0, pixelCount );

			return BufferState::SizeChanged;
		}

		auto planesArray = image.callObjectMethod( "getPlanes", "()[Landroid/media/Image$Plane;" );
		auto planes = planesArray.object<jobjectArray>();
		const int planeCount = qjniEnv->GetArrayLength( planes );

		bool updated{ false };

		for( int planeIndex = 0; planeIndex < planeCount; ++planeIndex )
		{
			auto element = qjniEnv->GetObjectArrayElement( planes, planeIndex );
			auto plane = QAndroidJniObject::fromLocalRef( element );
			const auto rowStride = plane.callMethod<jint>( "getRowStride" );
			const auto byteBuffer = plane.callObjectMethod( "getBuffer", "()Ljava/nio/ByteBuffer;" );
			const auto byteBufferObject = byteBuffer.object<jobject>();
			const auto sourceImageData = reinterpret_cast<const uchar *>( qjniEnv->GetDirectBufferAddress(byteBufferObject) );

			updated |= updateBuffer( sourceImageData, imageFormat, rowStride, m_data, m_size, rects );
		}

		avqDebug() << "Finished in" << benchTimer.elapsed();
		return rotated ? BufferState::Rotated :
					   updated ? BufferState::Updated : BufferState::Ready;
	}

	return BufferState::WaitingForCapturer;
}

}

ANYVNC_EXPORT_PLUGIN(AnyVnc::AndroidMediaProjectionFramebuffer)
