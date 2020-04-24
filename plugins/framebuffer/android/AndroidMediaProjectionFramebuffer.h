/*
 * AndroidMediaProjectionFramebuffer.h
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

#pragma once

#include <QAndroidActivityResultReceiver>
#include <QAndroidJniObject>
#include <QColor>
#include <QMutex>
#include <QRect>
#include <QVector>

#include "libanyvnc/interfaces/Framebuffer.h"

namespace AnyVnc
{

// clazy:excludeall=copyable-polymorphic

class AndroidMediaProjectionFramebuffer : public Interfaces::Framebuffer, QAndroidActivityResultReceiver
{
public:
	enum class AndroidPixelFormat {
		Other = 0,
		RGBA_8888 = 1,
		RGBX_8888 = 2,
		RGB_888 = 3,
		RGB_565 = 4,
	};

	using RectVector = QVector<Types::Rectangle>;

	explicit AndroidMediaProjectionFramebuffer() = default;

	std::string uid() const override
	{
		return "6e58e32f-e57f-4093-8be0-ac04a8c8a8b6";
	}

	Types::VersionNumber version() const override
	{
		return { 1, 0 };
	}

	std::string name() const override
	{
		return "AndroidMediaProjectionFramebuffer";
	}

	std::string description() const override
	{
		return "AndroidMediaProjection framebuffer";
	}

	std::string vendor() const override
	{
		return "AnyVNC Community";
	}

	std::string copyright() const override
	{
		return "Tobias Junghans";
	}

	Flags flags() const override
	{
		return Flags{ Flag::ProvidesDefaultImplementation };
	}

	bool initialize( Core::Server* server ) override;

	void* data() const override;
	Types::Size size() const override;

	UpdateFlags update( const RectangleVisitor& visitor ) override;

	Types::Screens availableScreens() const override;

	void handleActivityResult( int receiverRequestCode, int resultCode, const QAndroidJniObject& data ) override;

private:
	static constexpr int RequestCodeCapturePermission = 14722;

	using AJO = QAndroidJniObject;

	enum class BufferState
	{
		WaitingForCapturer,
		InvalidImageSize,
		Ready,
		Updated,
		Rotated,
		SizeChanged,
	};

	BufferState readMediaProjectionBuffer( RectVector* rects );

	QMutex m_screenCapturerMutex;
	AJO m_screenCapturer;

	QRgb* m_data{nullptr};
	Types::Size m_size;

};

}
