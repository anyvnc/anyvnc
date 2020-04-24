/*
 * KeyboardShortcutTrapper.h - class for trapping system-wide keyboard shortcuts
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

#include <QObject>

#include "AnyVncQtCore.h"

namespace AnyVncQt
{

namespace Core
{

class ANYVNC_QT_CORE_EXPORT KeyboardShortcutTrapper : public QObject
{
	Q_OBJECT
public:
	enum Shortcut
	{
		None,
		AltTab,
		AltEsc,
		AltSpace,
		AltF4,
		CtrlEsc,
		SuperKeyDown,
		SuperKeyUp
	} ;
	Q_ENUM(Shortcut)

	explicit KeyboardShortcutTrapper( QObject* parent = nullptr ) :
		QObject( parent )
	{
	}

	virtual void setEnabled( bool on ) = 0;

Q_SIGNALS:
	void shortcutTrapped( KeyboardShortcutTrapper::Shortcut );

};

}

}
