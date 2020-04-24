/*
 * QtQuickUserInterface.cpp
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

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#include "QtQuickUserInterface.h"
#include "libanyvnc/qt/core/VncServer.h"
#include "libanyvnc/qt/quick/VncViewItem.h"

namespace AnyVnc
{

bool QtQuickUserInterface::initialize()
{
	return true;
}



bool QtQuickUserInterface::run( int argc, char** argv )
{
	if( QCoreApplication::instance() == nullptr )
	{
		new QGuiApplication( argc, argv );
	}

	QQuickStyle::setStyle( QStringLiteral("Material") );

	qmlRegisterType<AnyVncQt::Core::VncServer>( "AnyVnc.Core", 1, 0, "VncServer" );
	qmlRegisterType<AnyVncQt::Quick::VncViewItem>( "AnyVnc.QtQuick", 1, 0, "VncViewItem" );

	QQmlApplicationEngine engine( QUrl( QStringLiteral("qrc:/qtquick-ui/AnyVncUI.qml") ) );

	return QGuiApplication::exec() == 0;
}

}

ANYVNC_EXPORT_PLUGIN(AnyVnc::QtQuickUserInterface)
