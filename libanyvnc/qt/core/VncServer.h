/*
 * Server.h
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

#include <QtConcurrent>

#include "libanyvnc/core/Server.h"

#include "AnyVncQt.h"

namespace AnyVncQt
{

namespace Core
{

class ANYVNC_QT_CORE_EXPORT VncServer : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
	Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
	Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
public:
	VncServer( QObject* parent = nullptr );

	int port() const;
	void setPort( int port );

	QString password() const;
	void setPassword( const QString& password );

	bool running() const;
	void setRunning( bool running );

Q_SIGNALS:
	void portChanged();
	void passwordChanged();
	void runningChanged();

private:
	AnyVnc::Core::Server* m_server{new AnyVnc::Core::Server};

	QFuture<void> m_serverRunnable;
	QAtomicInt m_quit;

};

}

}
