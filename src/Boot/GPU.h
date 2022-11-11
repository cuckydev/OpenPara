/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- GPU.h -
	PSX GPU
*/

#pragma once

#include "OpenPara.h"

namespace OpenPara
{
	namespace GPU
	{
		// GPU globals
		static constexpr unsigned int g_width = 320;
		static constexpr unsigned int g_height = 240;

		extern bool g_pal;

		extern int g_frame;

		// GPU buffers
		enum OT
		{
			UI = 0,
			Length = 2048
		};

		struct Buffer
		{
			// Environment
			::DISPENV disp;
			::DRAWENV draw;
			
			// Buffers
			uint32_t ot[1 + OT::Length]; // Ordering table
			uint32_t *prip;
			uint32_t pri[0x8000]; // Primitive buffer
		};

		extern Buffer g_buffer[2];
		extern Buffer *g_bufferp;
		
		// GPU functions
		void Init();

		void Flip();

		template <typename T>
		static T *AllocPrim(uint32_t ot)
		{
			// Allocate and link primitive
			void *pri = g_bufferp->prip;
			g_bufferp->prip += sizeof(T) >> 2;
			addPrim(&g_bufferp->ot[1 + ot], pri);
			return (T*)pri;
		}

		template <typename T>
		static T *GetPrim() { return (T*)g_bufferp->prip; }

		template <typename T>
		static void InsertPrim() { g_bufferp->prip += sizeof(T) >> 2; }

		static void FillRect(uint32_t ot, const ::RECT &rect, uint8_t r, uint8_t g, uint8_t b)
		{
			TILE *tile = AllocPrim<TILE>(ot);
			setTile(tile);
			tile->x0 = rect.x;
			tile->y0 = rect.y;
			tile->w = rect.w;
			tile->h = rect.h;
			tile->r0 = r;
			tile->g0 = g;
			tile->b0 = b;
		}
	}
}
