/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- GPU.cpp -
	PSX GPU
*/

#include "Boot/GPU.h"

#include "Boot/OpenPara.h"

namespace OpenPara
{
	namespace GPU
	{
		// GPU globals
		bool g_pal = false;

		// GPU buffers
		Buffer g_buffer[2];
		Buffer *g_bufferp;

		// GPU functions
		void Init()
		{
			// Get PAL flag from GPU
			if ((GPU_GP1 >> 20) & 1)
				g_pal = true;
			DebugOut("%s\n", g_pal ? "PAL GPU" : "NTSC GPU");

			// Reset GPU
			::SetDispMask(0);
			::ResetGraph(0);
			
			// Initialize GTE
			#ifndef __INTELLISENSE__
				__asm__ volatile(
					"mfc0    $v0, $12;"
					"lui     $v1, 0x4000;"
					"or      $v0, $v1;"
					"mtc0    $v0, $12;"
					"nop;"
					"li      $t0, 0x155;"
					"ctc2    $t0, $29;"
					"nop;"
					"li      $t0, 0x100;"
					"ctc2    $t0, $30;"
					"nop;"
					"li      $t0, 0x3E8;"
					"ctc2    $t0, $26;"
					"nop;"
					"li      $t0, 0xFFFFEF9E;"
					"ctc2    $t0, $27;"
					"nop;"
					"lui     $t0, 0x140;"
					"ctc2    $t0, $28;"
					"nop;"
					"ctc2    $zero, $24;"
					"ctc2    $zero, $25;"
					"nop;"
				);
			#endif

			// Initialize screen
			::SetDefDispEnv(&g_buffer[0].disp, 0,   0, g_width, g_height);
			::SetDefDispEnv(&g_buffer[1].disp, 0, 240, g_width, g_height);

			::SetDefDrawEnv(&g_buffer[0].draw, 0, 240, g_width, g_height);
			::SetDefDrawEnv(&g_buffer[1].draw, 0,   0, g_width, g_height);

			g_buffer[0].draw.dtd = 0;
			g_buffer[1].draw.dtd = 0;

			// Set video mode
			if (g_pal)
			{
				SetVideoMode(MODE_PAL);
				g_buffer[0].disp.screen.y = g_buffer[1].disp.screen.y = 24;
			}
			else
			{
				SetVideoMode(MODE_NTSC);
			}

			// Clear VRAM
			::RECT clear_rect = {0, 0, g_width, 512};
			::ClearImage(&clear_rect, 0, 0, 0);

			// Set and initialize buffer
			g_bufferp = &g_buffer[0];

			g_bufferp->prip = g_bufferp->pri;
			::ClearOTagR((u_long*)g_bufferp->ot, 1 + OT::Length);

			// Enable display
			::SetDispMask(1);
		}

		void Flip()
		{
			// Sync
			::DrawSync(0);
			::VSync(0);
			
			// Use display and draw environment
			::PutDispEnv(&g_bufferp->disp);
			::PutDrawEnv(&g_bufferp->draw);

			// Draw ordering tables
			::DrawOTag((u_long*)&g_bufferp->ot[OT::Length]);
			
			// Flip and initialize buffer
			g_bufferp = (g_bufferp == &g_buffer[0]) ? &g_buffer[1] : &g_buffer[0];

			g_bufferp->prip = g_bufferp->pri;
			::ClearOTagR((u_long*)g_bufferp->ot, 1 + OT::Length);
		}
	}
}
