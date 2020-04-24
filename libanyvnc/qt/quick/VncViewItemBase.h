/*
 * VncViewItemBase.h
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

#include <QQuickPaintedItem>

#include "AnyVncQtQuick.h"
#include "../core/VncView.h"

namespace AnyVncQt
{

namespace Quick
{

class ANYVNC_QT_QUICK_EXPORT VncViewItemBase : public QQuickItem, public Core::VncView
{
	Q_OBJECT
public:
	VncViewItemBase( Core::VncConnection* connection, QQuickItem* parent = nullptr );
	~VncViewItemBase() override = default;

	QSGNode* updatePaintNode( QSGNode *oldNode, UpdatePaintNodeData* updatePaintNodeData ) override;

protected:
	virtual void updateView( int x, int y, int w, int h ) override;
	virtual QSize viewSize() const override;
	virtual void setViewCursor( const QCursor& cursor ) override;

	bool event( QEvent* event ) override;

private:
	QSize m_framebufferSize;

};

}

}
