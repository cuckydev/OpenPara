/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Pad.h -
	PSX pad
*/

#pragma once

#include "OpenPara.h"

namespace OpenPara
{
	namespace Pad
	{
		// Pad globals
		enum Button : uint16_t
		{
			Select = 0x0001,
			L3 = 0x0002,
			R3 = 0x0004,
			Start = 0x0008,
			Up = 0x0010,
			Right = 0x0020,
			Down = 0x0040,
			Left = 0x0080,
			L2 = 0x0100,
			R2 = 0x0200,
			L1 = 0x0400,
			R1 = 0x0800,
			Triangle = 0x1000,
			Circle = 0x2000,
			Cross = 0x4000,
			Square = 0x8000
		};

		struct Pad
		{
			Button held = Button(0), press = Button(0);
			int8_t left_x = 0, left_y = 0;
			int8_t right_x = 0, right_y = 0;
		};

		extern Pad g_pad[2];

		// Pad functions
		void Init();

		void Poll();
	}
}
