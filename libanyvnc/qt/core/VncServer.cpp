/*
 * Server.cpp
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

#include "VncServer.h"

namespace AnyVncQt::Core
{

VncServer::VncServer( QObject* parent ) :
    QObject( parent )
{
}



int VncServer::port() const
{
	return m_server->port();
}



void VncServer::setPort( int port )
{
	if( running() == false &&
		m_server->port() != port )
	{
		m_server->setPort( port );
		Q_EMIT portChanged();
	}
}



QString VncServer::password() const
{
	return QString::fromStdString( m_server->password() );
}



void VncServer::setPassword( const QString& password )
{
	const auto stdPassword = password.toStdString();

	if( running() == false &&
		m_server->password() != stdPassword )
	{
		m_server->setPassword( stdPassword );
		Q_EMIT passwordChanged();
	}
}



bool VncServer::running() const
{
	return m_serverRunnable.isRunning();
}



void VncServer::setRunning( bool running )
{
	if( running != m_serverRunnable.isRunning() )
	{
		if( running )
		{
			m_serverRunnable = QtConcurrent::run( [this]() {
				m_quit = false;
				while( m_quit == false )
				{
					m_server->run();
				}
			} );
		}
		else
		{
			m_quit = true;
			m_server->quit();
		}

		Q_EMIT runningChanged();
	}
}

}
