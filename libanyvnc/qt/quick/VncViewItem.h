/*
 * VncViewItem.h - QtQuick VNC view item
 *
 * Copyright (c) 2019-2020 Tobias Junghans <tobydox@veyon.io>
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

#include "VncViewItemBase.h"

namespace AnyVncQt
{

namespace Quick
{

class ANYVNC_QT_QUICK_EXPORT VncViewItem : public VncViewItemBase
{
	Q_OBJECT
	Q_PROPERTY(QString host READ host WRITE setHost)
	Q_PROPERTY(int port READ port WRITE setPort)
	Q_PROPERTY(QString password READ password WRITE setPassword)
public:
	VncViewItem( QQuickItem* parent = nullptr );
	~VncViewItem() override;

	const QString& host() const
	{
		return connection()->host();
	}

	void setHost( const QString& host )
	{
		connection()->setHost( host );
	}

	int port() const
	{
		return connection()->port();
	}

	void setPort( int port )
	{
		connection()->setPort( port );
	}

	const QString& password() const
	{
		return connection()->password();
	}

	void setPassword( const QString& password )
	{
		connection()->setPassword( password );
	}

protected:
	void componentComplete() override;
};

}

}
