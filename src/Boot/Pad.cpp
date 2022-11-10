/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Pad.cpp -
	PSX pad
*/

#include "Boot/Pad.h"

namespace OpenPara
{
	namespace Pad
	{
		// Pad state
		struct PADTYPE
		{
			u_char stat;
			u_char len : 4;
			u_char type : 4;
			u_short btn;
			u_char rs_x, rs_y;
			u_char ls_x, ls_y;
		};

		static u_short pad_buff[2][34 / 2];

		// Pad globals
		Pad g_pad[2];

		// Pad functions
		void Init()
		{
			// Initialize pad API
			::InitPAD((char*)pad_buff[0], 34, (char*)pad_buff[1], 34);
			pad_buff[0][0] = 0xFFFF;
			pad_buff[1][0] = 0xFFFF;
			::StartPAD();

			// Change pad interrupt
			::ChangeClearPAD(0);
		}

		void Poll()
		{
			// Read pad states
			for (int i = 0; i < 2; i++)
			{
				// Read pad state
				PADTYPE &pad_state = *((PADTYPE*)pad_buff[i]);
				Pad &pad = g_pad[i];

				if (pad_state.stat == 0)
				{
					if ((pad_state.type == 0x4) || (pad_state.type == 0x5) || (pad_state.type == 0x7))
					{
						//Set pad state
						pad.press = Button((~pad_state.btn) & (~pad.held));
						pad.held = Button(~pad_state.btn);
						pad.left_x = (char)pad_state.ls_x + -0x80;
						pad.left_y = (char)pad_state.ls_y + -0x80;
						pad.right_x = (char)pad_state.rs_x + -0x80;
						pad.right_y = (char)pad_state.rs_y + -0x80;
					}
				}
			}
		}
	}
}
