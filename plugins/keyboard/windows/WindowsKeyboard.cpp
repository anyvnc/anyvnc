/*
 * WindowsKeyboard.cpp - based on vncKeymap.cpp
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

#include <map>
#include <vector>

#include <windows.h>
#include <ime.h>

#include "WindowsKeyboard.h"

#include "libanyvnc/core/Server.h"
#include "libanyvnc/interfaces/Framebuffer.h"

#define XK_LATIN1
#define XK_CURRENCY
#define XK_GREEK
#define XK_TECHNICAL
#define XK_KATAKANA

#include "../common/keysym.h"

namespace AnyVnc
{

static void keybd_uni_event( BYTE bVk, BYTE bScan, DWORD dwFlags, ULONG_PTR dwExtraInfo )
{
	keybd_event( bVk, bScan, dwFlags, dwExtraInfo );
}


static uint32_t keysymDead = 0;

struct keymap_t {
	uint32_t keysym;
	uint8_t vk;
	bool extended;
};

static std::vector<unsigned char> deadChars;

static keymap_t keymap[] = {

	// TTY functions
	{ XK_BackSpace,        VK_BACK, 0 },
	{ XK_Tab,              VK_TAB, 0 },
	{ XK_Clear,            VK_CLEAR, 0 },
	{ XK_Return,           VK_RETURN, 0 },
	{ XK_Pause,            VK_PAUSE, 0 },
	{ XK_Escape,           VK_ESCAPE, 0 },
	{ XK_Delete,           VK_DELETE, 1 },

	// Japanese stuff - almost certainly wrong...
	//	[v1.0.2-jp1 fix] IOport's patch (Correct definition of Japanese key)
	//{ XK_Kanji,            VK_KANJI, 0 },
	//{ XK_Kana_Shift,       VK_KANA, 0 },
	// Japanese key
	{ XK_Kanji,            VK_KANJI, 0 },				/* 0x19: Kanji, Kanji convert */
	{ XK_Muhenkan,         VK_NONCONVERT, 0 },		/* 0x1d: Cancel Conversion */
	{ XK_Romaji,           VK_DBE_ROMAN, 0 },			/* 0xf5: to Romaji */
	{ XK_Hiragana,         VK_DBE_HIRAGANA, 0 },		/* 0xf2: to Hiragana */
	{ XK_Katakana,         VK_DBE_KATAKANA, 0 },		/* 0xf1: to Katakana */
	{ XK_Zenkaku,          VK_DBE_SBCSCHAR, 0 },		/* 0xf3: to Zenkaku */
	{ XK_Hankaku,          VK_DBE_DBCSCHAR, 0 },		/* 0xf4: to Hankaku */
	{ XK_Eisu_toggle,      VK_DBE_ALPHANUMERIC, 0 },	/* 0xf0: Alphanumeric toggle */
	{ XK_Mae_Koho,         VK_CONVERT, 0 },			/* 0x1c: Previous Candidate */

	// Cursor control & motion
	{ XK_Home,             VK_HOME, 1 },
	{ XK_Left,             VK_LEFT, 1 },
	{ XK_Up,               VK_UP, 1 },
	{ XK_Right,            VK_RIGHT, 1 },
	{ XK_Down,             VK_DOWN, 1 },
	{ XK_Page_Up,          VK_PRIOR, 1 },
	{ XK_Page_Down,        VK_NEXT, 1 },
	{ XK_End,              VK_END, 1 },

	// Misc functions
	{ XK_Select,           VK_SELECT, 0 },
	{ XK_Print,            VK_SNAPSHOT, 0 },
	{ XK_Execute,          VK_EXECUTE, 0 },
	{ XK_Insert,           VK_INSERT, 1 },
	{ XK_Help,             VK_HELP, 0 },
	{ XK_Break,            VK_CANCEL, 1 },

	// Keypad Functions, keypad numbers
	{ XK_KP_Space,         VK_SPACE, 0 },
	{ XK_KP_Tab,           VK_TAB, 0 },
	{ XK_KP_Enter,         VK_RETURN, 1 },
	{ XK_KP_F1,            VK_F1, 0 },
	{ XK_KP_F2,            VK_F2, 0 },
	{ XK_KP_F3,            VK_F3, 0 },
	{ XK_KP_F4,            VK_F4, 0 },
	{ XK_KP_Home,          VK_HOME, 0 },
	{ XK_KP_Left,          VK_LEFT, 0 },
	{ XK_KP_Up,            VK_UP, 0 },
	{ XK_KP_Right,         VK_RIGHT, 0 },
	{ XK_KP_Down,          VK_DOWN, 0 },
	{ XK_KP_End,           VK_END, 0 },
	{ XK_KP_Page_Up,       VK_PRIOR, 0 },
	{ XK_KP_Page_Down,     VK_NEXT, 0 },
	{ XK_KP_Begin,         VK_CLEAR, 0 },
	{ XK_KP_Insert,        VK_INSERT, 0 },
	{ XK_KP_Delete,        VK_DELETE, 0 },
	// XXX XK_KP_Equal should map in the same way as ascii '='
	{ XK_KP_Multiply,      VK_MULTIPLY, 0 },
	{ XK_KP_Add,           VK_ADD, 0 },
	{ XK_KP_Separator,     VK_SEPARATOR, 0 },
	{ XK_KP_Subtract,      VK_SUBTRACT, 0 },
	{ XK_KP_Decimal,       VK_DECIMAL, 0 },
	{ XK_KP_Divide,        VK_DIVIDE, 1 },

	{ XK_KP_0,             VK_NUMPAD0, 0 },
	{ XK_KP_1,             VK_NUMPAD1, 0 },
	{ XK_KP_2,             VK_NUMPAD2, 0 },
	{ XK_KP_3,             VK_NUMPAD3, 0 },
	{ XK_KP_4,             VK_NUMPAD4, 0 },
	{ XK_KP_5,             VK_NUMPAD5, 0 },
	{ XK_KP_6,             VK_NUMPAD6, 0 },
	{ XK_KP_7,             VK_NUMPAD7, 0 },
	{ XK_KP_8,             VK_NUMPAD8, 0 },
	{ XK_KP_9,             VK_NUMPAD9, 0 },

	// Auxilliary Functions
	{ XK_F1,               VK_F1, 0 },
	{ XK_F2,               VK_F2, 0 },
	{ XK_F3,               VK_F3, 0 },
	{ XK_F4,               VK_F4, 0 },
	{ XK_F5,               VK_F5, 0 },
	{ XK_F6,               VK_F6, 0 },
	{ XK_F7,               VK_F7, 0 },
	{ XK_F8,               VK_F8, 0 },
	{ XK_F9,               VK_F9, 0 },
	{ XK_F10,              VK_F10, 0 },
	{ XK_F11,              VK_F11, 0 },
	{ XK_F12,              VK_F12, 0 },
	{ XK_F13,              VK_F13, 0 },
	{ XK_F14,              VK_F14, 0 },
	{ XK_F15,              VK_F15, 0 },
	{ XK_F16,              VK_F16, 0 },
	{ XK_F17,              VK_F17, 0 },
	{ XK_F18,              VK_F18, 0 },
	{ XK_F19,              VK_F19, 0 },
	{ XK_F20,              VK_F20, 0 },
	{ XK_F21,              VK_F21, 0 },
	{ XK_F22,              VK_F22, 0 },
	{ XK_F23,              VK_F23, 0 },
	{ XK_F24,              VK_F24, 0 },

	// Modifiers
	{ XK_Shift_L,          VK_SHIFT, 0 },
	{ XK_Shift_R,          VK_RSHIFT, 0 },
	{ XK_Control_L,        VK_CONTROL, 0 },
	{ XK_Control_R,        VK_CONTROL, 1 },
	{ XK_Alt_L,            VK_MENU, 0 },
	{ XK_Alt_R,            VK_RMENU, 1 },

	// Left & Right Windows keys & Windows Menu Key
	{ XK_Super_L,			VK_LWIN, 0 },
	{ XK_Super_R,			VK_RWIN, 0 },
	{ XK_Menu,			VK_APPS, 0 },

};

static uint8_t latin1DeadChars[] = {
	XK_grave, XK_acute, XK_asciicircum, XK_diaeresis, XK_degree, XK_cedilla, XK_asciitilde
};


// doKeyboardEvent wraps the system keybd_event function and attempts to find
// the appropriate scancode corresponding to the supplied virtual keycode.

static inline void doKeyboardEvent( BYTE vkCode, DWORD flags )
{
	keybd_uni_event(vkCode, BYTE(MapVirtualKey(vkCode, 0)), flags, 0);
}

// KeyStateModifier is a class which helps simplify generating a "fake" press
// or release of shift, ctrl, alt, etc.  An instance of the class is created
// for every key which may need to be pressed or released.  Then either press()
// or release() may be called to make sure that the corresponding key is in the
// right state.  The destructor of the class automatically reverts to the
// previous state.

class KeyStateModifier
{
public:
	KeyStateModifier(int vkCode_, DWORD flags_=0)
		: vkCode(vkCode_), flags(flags_), pressed(false), released(false)
	{
	}

	void press()
	{
		if (!(GetAsyncKeyState(vkCode) & 0x8000))
		{
			doKeyboardEvent(BYTE(vkCode), flags);
			pressed = true;
		}
	}
	void release()
	{
		if (GetAsyncKeyState(vkCode) & 0x8000)
		{
			doKeyboardEvent(BYTE(vkCode), flags | KEYEVENTF_KEYUP);
			released = true;
		}
	}
	~KeyStateModifier()
	{
		if (pressed)
		{
			doKeyboardEvent(BYTE(vkCode), flags | KEYEVENTF_KEYUP);
		}
		else if (released)
		{
			doKeyboardEvent(BYTE(vkCode), flags);
		}
	}

	int vkCode;
	DWORD flags;
	bool pressed;
	bool released;
};


class Keymapper
{
public:
	Keymapper()
	{
		for (size_t i = 0; i < sizeof(keymap) / sizeof(keymap_t); i++)
		{
			vkMap[keymap[i].keysym] = keymap[i].vk;
			extendedMap[keymap[i].keysym] = keymap[i].extended;
		}

		BYTE keystate[256];
		memset(keystate, 0, 256);
		for (size_t j = 0; j < sizeof(latin1DeadChars); j++)
		{
			SHORT s = VkKeyScan(CHAR(latin1DeadChars[j]));
			if (s != -1) {
				BYTE vkCode = LOBYTE(s);
				BYTE modifierState = HIBYTE(s);
				keystate[VK_SHIFT] = (modifierState & 1) ? 0x80 : 0;
				keystate[VK_CONTROL] = (modifierState & 2) ? 0x80 : 0;
				keystate[VK_MENU] = (modifierState & 4) ? 0x80 : 0;
				WORD chars;
				int nchars = ToAscii(vkCode, 0, keystate, &chars, 0);
				if (nchars < 0) {
					deadChars.push_back(latin1DeadChars[j]);
					ToAscii(vkCode, 0, keystate, &chars, 0);
				}
			}
		}
	}

	void keyEvent(uint32_t keysym, bool down, bool jap, bool unicode)
	{
		if (keysym>=XK_dead_grave && keysym <=XK_dead_belowdot)
		{
			keysymDead=keysym;
			//we have a dead key
			//Record dead key
			return;
		}

		if ((keysym >= 32 && keysym <= 126) ||
			(keysym >= 160 && keysym <= 255))
		{
			if (keysymDead!=0 && down)
			{
				switch (keysymDead)
				{
				case XK_dead_grave:
					switch(keysym)
					{
					case XK_A: keysym=XK_Agrave;break;
					case XK_E: keysym=XK_Egrave;break;
					case XK_I: keysym=XK_Igrave;break;
					case XK_O: keysym=XK_Ograve;break;
					case XK_U: keysym=XK_Ugrave;break;
					case XK_a: keysym=XK_agrave;break;
					case XK_e: keysym=XK_egrave;break;
					case XK_i: keysym=XK_igrave;break;
					case XK_o: keysym=XK_ograve;break;
					case XK_u: keysym=XK_ugrave;break;
					}
					break;
				case XK_dead_acute:
					switch(keysym)
					{
					case XK_A: keysym=XK_Aacute;break;
					case XK_E: keysym=XK_Eacute;break;
					case XK_I: keysym=XK_Iacute;break;
					case XK_O: keysym=XK_Oacute;break;
					case XK_U: keysym=XK_Uacute;break;
					case XK_a: keysym=XK_aacute;break;
					case XK_e: keysym=XK_eacute;break;
					case XK_i: keysym=XK_iacute;break;
					case XK_o: keysym=XK_oacute;break;
					case XK_u: keysym=XK_uacute;break;
					case XK_y: keysym=XK_yacute;break;
					case XK_Y: keysym=XK_Yacute;break;
					}
					break;
				case XK_dead_circumflex:
					switch(keysym)
					{
					case XK_A: keysym=XK_Acircumflex;break;
					case XK_E: keysym=XK_Ecircumflex;break;
					case XK_I: keysym=XK_Icircumflex;break;
					case XK_O: keysym=XK_Ocircumflex;break;
					case XK_U: keysym=XK_Ucircumflex;break;
					case XK_a: keysym=XK_acircumflex;break;
					case XK_e: keysym=XK_ecircumflex;break;
					case XK_i: keysym=XK_icircumflex;break;
					case XK_o: keysym=XK_ocircumflex;break;
					case XK_u: keysym=XK_ucircumflex;break;
					}
					break;
				case XK_dead_tilde:
					switch(keysym)
					{
					case XK_A : keysym=XK_Ntilde;break;
					case XK_O : keysym=XK_Otilde;break;
					case XK_a : keysym=XK_atilde;break;
					case XK_n : keysym=XK_ntilde;break;
					case XK_o : keysym=XK_otilde;break;
					}
					break;
				case XK_dead_diaeresis:
					switch(keysym)
					{
					case XK_A: keysym=XK_Adiaeresis;break;
					case XK_E: keysym=XK_Ediaeresis;break;
					case XK_I: keysym=XK_Idiaeresis;break;
					case XK_O: keysym=XK_Odiaeresis;break;
					case XK_U: keysym=XK_Udiaeresis;break;
					case XK_a: keysym=XK_adiaeresis;break;
					case XK_e: keysym=XK_ediaeresis;break;
					case XK_i: keysym=XK_idiaeresis;break;
					case XK_o: keysym=XK_odiaeresis;break;
					case XK_u: keysym=XK_udiaeresis;break;
					case XK_y: keysym=XK_ydiaeresis;break;
					}
					break;
				case XK_dead_cedilla:
					switch(keysym)
					{
					case XK_C: keysym=XK_Ccedilla;break;
					case XK_c: keysym=XK_ccedilla;break;
					}
				}
				keysymDead=0;

			}
			// ordinary Latin-1 character
			auto s = VkKeyScan(CHAR(keysym));

			if (jap)
			{
				if (keysym==XK_kana_WO) {
					s = 0x0130;
				} else if (keysym==XK_backslash) {
					s = 0x00e2;
				} else if (keysym==XK_yen) {
					s = 0x00dc;
				}
			}

			if (s == -1)
			{

				if (down)
				{
					// not a single keypress - try synthesizing dead chars.
					{
						//Lookup ascii representation
						int ascii=0;
						ascii = keysym;
						if (ascii <= 255)
						{
							uint8_t a0=ascii/100;
							ascii=ascii%100;
							uint8_t a1=ascii/10;
							ascii=ascii%10;
							uint8_t a2=ascii;

							KeyStateModifier shift(VK_SHIFT);
							KeyStateModifier lshift(VK_LSHIFT);
							KeyStateModifier rshift(VK_RSHIFT);

							lshift.release();
							rshift.release();

							keybd_uni_event(VK_MENU,BYTE(MapVirtualKey( VK_MENU, 0 )), 0 ,0);
							/**
				Pressing the Alt+NNN combinations without leading zero (for example, Alt+20, Alt+130, Alt+221)
				will insert characters from the Extended ASCII (or MS DOS ASCII, or OEM) table. The character
				glyphs contained by this table depend on the language of Windows. See the table below for the
				list of characters that can be inserted through the Alt+NNN combinations (without leading zero)
				in English Windows.

                Pressing the Alt+0NNN combinations will insert the ANSI characters corresponding to the activate
                keyboard layout. Please see Windows Character Map utility (charmap.exe) for the possible Alt+0NNN
                combinations.

                Finally, the Alt+00NNN combinations (two leading zeros) will insert Unicode characters. The Unicode
                codes of characters are displayed in Charmap.

							**/
							// jdp 11 December 2008 - Need the leading 0!
							keybd_uni_event(VK_NUMPAD0,    BYTE(MapVirtualKey(VK_NUMPAD0,    0)), 0, 0);
							keybd_uni_event(VK_NUMPAD0,    BYTE(MapVirtualKey(VK_NUMPAD0,    0)),KEYEVENTF_KEYUP,0);
							keybd_uni_event(VK_NUMPAD0+a0, BYTE(MapVirtualKey(VK_NUMPAD0+a0, 0)), 0, 0);
							keybd_uni_event(VK_NUMPAD0+a0, BYTE(MapVirtualKey(VK_NUMPAD0+a0, 0)),KEYEVENTF_KEYUP,0);
							keybd_uni_event(VK_NUMPAD0+a1, BYTE(MapVirtualKey(VK_NUMPAD0+a1, 0)),0,0);
							keybd_uni_event(VK_NUMPAD0+a1, BYTE(MapVirtualKey(VK_NUMPAD0+a1, 0)),KEYEVENTF_KEYUP, 0);
							keybd_uni_event(VK_NUMPAD0+a2, BYTE(MapVirtualKey(VK_NUMPAD0+a2, 0)) ,0, 0);
							keybd_uni_event(VK_NUMPAD0+a2, BYTE(MapVirtualKey(VK_NUMPAD0+a2, 0)),KEYEVENTF_KEYUP, 0);
							keybd_uni_event(VK_MENU, BYTE(MapVirtualKey( VK_MENU, 0 )),KEYEVENTF_KEYUP, 0);
							return;
						}
					}
				}
				return;
			}

			BYTE vkCode = LOBYTE(s);

			// 18 March 2008 jdp
			// Correct the keymask shift state to cope with the capslock state
			BOOL capslockOn = (GetKeyState(VK_CAPITAL) & 1) != 0;

			BYTE modifierState = HIBYTE(s);
			modifierState = capslockOn ? modifierState ^ 1 : modifierState;
			KeyStateModifier ctrl(VK_CONTROL);
			KeyStateModifier alt(VK_MENU);
			KeyStateModifier shift(VK_SHIFT);
			KeyStateModifier lshift(VK_LSHIFT);
			KeyStateModifier rshift(VK_RSHIFT);

			if (down) {
				if (modifierState & 2) ctrl.press();
				if (modifierState & 4) alt.press();
				if (modifierState & 1) {
					shift.press();
				} else {
					// [v1.0.2-jp1 fix] Even if "SHIFT + SPACE" are pressed, "SHIFT" is valid
					if (vkCode == 0x20){
					}
					else{
						lshift.release();
						rshift.release();
					}
				}
			}

			doKeyboardEvent(vkCode, down ? 0 : KEYEVENTF_KEYUP);
		}
		else
		{
			// see if it's a recognised keyboard key, otherwise ignore it
			if (vkMap.find(keysym) == vkMap.end()) {
				if (unicode) {
					if (keysym == 65509)
						return;
					INPUT inputs[1];
					char *key, text[32];
					sprintf_s(text,"%d",keysym);
					key = text;
					if (down) {
						inputs[0].type= INPUT_KEYBOARD;
						inputs[0].ki.wVk = 0;
						inputs[0].ki.wScan = atoi(key);
						inputs[0].ki.time = 0;
						inputs[0].ki.dwExtraInfo = 0;
						inputs[0].ki.dwFlags = KEYEVENTF_UNICODE;
						SendInput(1, inputs, sizeof(INPUT));
					} else {
						inputs[0].type= INPUT_KEYBOARD;
						inputs[0].ki.wVk = 0;
						inputs[0].ki.wScan = atoi(key);
						inputs[0].ki.time = 0;
						inputs[0].ki.dwExtraInfo = 0;
						inputs[0].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
						SendInput(1, inputs, sizeof(INPUT));
					}
				}
				return;
			}
			BYTE vkCode = vkMap[keysym];
			DWORD flags = 0;
			if (extendedMap[keysym]) flags |= KEYEVENTF_EXTENDEDKEY;
			if (!down) flags |= KEYEVENTF_KEYUP;

			if (down && (vkCode == VK_DELETE) &&
				((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0) &&
				((GetAsyncKeyState(VK_MENU) & 0x8000) != 0))
			{
				auto sasEvent = OpenEventW( EVENT_MODIFY_STATE, false, L"Global\\AnyVncServiceSasEvent" );
				SetEvent( sasEvent );
				CloseHandle( sasEvent );
				return;
			}

			doKeyboardEvent(vkCode, flags);
		}
	}

private:
	std::map<uint32_t,uint8_t> vkMap;
	std::map<uint32_t,bool> extendedMap;
};

static Keymapper key_mapper;


static void SetShiftState(BYTE key, BOOL down)
{
	BOOL keystate = (GetAsyncKeyState(key) & 0x8000) != 0;

	// This routine sets the specified key to the desired value (up or down)
	if ((keystate && down) || ((!keystate) && (!down)))
		return;

	// Now send a key event to set the key to the new value
	doKeyboardEvent(key, down ? 0 : KEYEVENTF_KEYUP);
	keystate = (GetAsyncKeyState(key) & 0x8000) != 0;
}



static void ClearShiftKeys()
{
	// LEFT
	SetShiftState(VK_LSHIFT, FALSE);
	SetShiftState(VK_LCONTROL, FALSE);
	SetShiftState(VK_LMENU, FALSE);

	// RIGHT
	SetShiftState(VK_RSHIFT, FALSE);
	SetShiftState(VK_RCONTROL, FALSE);
	SetShiftState(VK_RMENU, FALSE);
}



bool WindowsKeyboard::initialize( Core::Server* server )
{
	m_server = server;

	ClearShiftKeys();

	return true;
}



void WindowsKeyboard::synthesizeKeyEvent( KeySym keySym, bool down )
{
	key_mapper.keyEvent( keySym, down, false, false );
}

}

ANYVNC_EXPORT_PLUGIN(AnyVnc::WindowsKeyboard)
