/*
 * AnyVncQt.h
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

#include <QDebug>

#include "AnyVncQtCore.h"

namespace AnyVncQt
{

ANYVNC_QT_CORE_EXPORT bool isDebugging();
ANYVNC_QT_CORE_EXPORT void setDebugging( bool enabled );
ANYVNC_QT_CORE_EXPORT QByteArray shortenFuncinfo( const QByteArray& info );
ANYVNC_QT_CORE_EXPORT QByteArray cleanupFuncinfo( QByteArray info );

}

#define AV_FUNC_INFO AnyVncQt::shortenFuncinfo(Q_FUNC_INFO).constData()

#define avqDebug() if( AnyVncQt::isDebugging()==false ); else qDebug() << AV_FUNC_INFO
#define avqInfo() qInfo() << AV_FUNC_INFO
#define avqWarning() qWarning() << AV_FUNC_INFO
#define avqCritical() qCritical() << AV_FUNC_INFO
