/*
 * VncViewItemBase.cpp
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

#include <QSGSimpleTextureNode>

#include "QSGImageTexture.h"
#include "VncViewItemBase.h"


namespace AnyVncQt::Quick
{

VncViewItemBase::VncViewItemBase( Core::VncConnection* connection, QQuickItem* parent ) :
	QQuickItem( parent ),
	VncView( connection )
{
	connectUpdateFunctions( this );

	setAcceptHoverEvents( true );
	setAcceptedMouseButtons( Qt::AllButtons );
	setKeepMouseGrab( true );

	setFlag( ItemHasContents );
	setFlag( ItemIsFocusScope );
}



QSGNode* VncViewItemBase::updatePaintNode( QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData )
{
	Q_UNUSED(updatePaintNodeData)

	auto* node = dynamic_cast<QSGSimpleTextureNode *>(oldNode);
	if( !node )
	{
		node = new QSGSimpleTextureNode();
		auto texture = new QSGImageTexture();
		node->setTexture( texture );
	}

	qobject_cast<QSGImageTexture *>( node->texture() )->setImage( connection()->image() );
	node->setRect( boundingRect() );

	return node;
}



void VncViewItemBase::setViewCursor( const QCursor& cursor )
{
	setCursor( cursor );
}



QSize VncViewItemBase::viewSize() const
{
	return { int(width()), int(height()) };
}



void VncViewItemBase::updateView( int x, int y, int w, int h )
{
	Q_UNUSED(x)
	Q_UNUSED(y)
	Q_UNUSED(w)
	Q_UNUSED(h)

	update();
}



bool VncViewItemBase::event( QEvent* event )
{
	return handleEvent( event ) || QQuickItem::event( event );
}

}
