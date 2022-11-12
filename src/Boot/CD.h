/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- CD.h -
	PSX CD
*/

#pragma once

#include "OpenPara.h"

namespace OpenPara
{
	namespace CD
	{
		// CD enums
		enum Region
		{
			Japan = 0,
			America = 1,
			Europe = 2
		};
		
		// CD globals
		extern Region g_region;

		// CD functions
		void Init();

		namespace XA
		{
			void Volume(int volume);
			void Filter(int file, int channel);

			void Play(const CdlFILE *file);
			void Stop();

			bool Playing();
			uint32_t Tell();
		}

		void Seek(const CdlLOC *loc);

		static uint8_t DecBCD(uint8_t x) { return x - 6 * (x >> 4); }
		static uint8_t EncBCD(uint8_t x) { return ((x / 10) << 4) | (x % 10); }
	}
}
