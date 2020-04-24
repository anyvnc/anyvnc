/*
 * VncViewWidget.h - VNC viewer widget
 *
 * Copyright (c) 2006-2020 Tobias Junghans <tobydox@veyon.io>
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

#include <QCoreApplication>
#include <QTimer>
#include <QWidget>

#include "../core/VncView.h"
#include "AnyVncQtWidgets.h"

namespace AnyVncQt
{

namespace Widgets
{

class ANYVNC_QT_WIDGETS_EXPORT VncViewWidget : public QWidget, public Core::VncView
{
	Q_OBJECT
public:
	VncViewWidget( const QString& host, int port, QWidget* parent );
	~VncViewWidget() override;

	QSize sizeHint() const override;

	void setViewOnly( bool enabled ) override;

Q_SIGNALS:
	void mouseAtBorder();
	void startConnection();
	void connectionEstablished();
	void sizeHintChanged();

protected:
	void updateView( int x, int y, int w, int h ) override;
	QSize viewSize() const override;
	void setViewCursor( const QCursor& cursor ) override;

	void updateFramebufferSize( int w, int h ) override;
	void updateImage( int x, int y, int w, int h ) override;

	bool event( QEvent* handleEvent ) override;
	bool eventFilter( QObject* obj, QEvent* handleEvent ) override;
	void focusInEvent( QFocusEvent* handleEvent ) override;
	void focusOutEvent( QFocusEvent* handleEvent ) override;
	void mouseEventHandler( QMouseEvent* handleEvent ) override;
	void paintEvent( QPaintEvent* handleEvent ) override;
	void resizeEvent( QResizeEvent* handleEvent ) override;

private:
	bool m_viewOnlyFocus{true};
	bool m_initDone{false};

	static constexpr int MouseBorderSignalDelay = 500;
	QTimer m_mouseBorderSignalTimer{this};

} ;

}

}
