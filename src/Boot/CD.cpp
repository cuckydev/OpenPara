/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- CD.cpp -
	PSX CD
*/

#include "Boot/CD.h"

#include "Boot/OpenPara.h"

namespace OpenPara
{
	namespace CD
	{
		// CD globals
		Region g_region;

		// CD functions
		void Init()
		{
			// Initialize CD API
			::CdInit();

			// Request the required region from the CD drive
			u_char param[1] = {0x22};
			u_char result[8] = {};
			::CdControlB(0x19, param, result);

			// Check region result
			if (memcmp(result, (u_char*)"for Japan", 8) == 0)
				g_region = Region::Japan;
			else if (memcmp(result, (u_char*)"for U/C", 8) == 0)
				g_region = Region::America;
			else if (memcmp(result, (u_char*)"for Europe", 8) == 0)
				g_region = Region::Europe;
			else if (memcmp(result, (u_char*)"\x11\x10", 2) == 0)
				g_region = Region::Japan;
			else
				g_region = Region::America; // Unknown, includes Net Yaroze and debug units
			
			switch (g_region)
			{
				case Region::Japan:
					DebugOut("Region::Japan\n");
					break;
				case Region::America:
					DebugOut("Region::America\n");
					break;
				case Region::Europe:
					DebugOut("Region::Europe\n");
					break;
			}
		}

		namespace XA
		{
			static uint32_t start_loc, end_loc, cur_loc;

			static void ReadyCallback(u_char status, u_char *result)
			{
				(void)status;
				(void)result;

				// Check error
				if (status != CdlDataReady)
					return;

				// Read subheader
				u_long subheader[0xC / 4]; 
				::CdGetSector(subheader, 0xC / 4);

				// Check if this is the end of the stream
				::CdlLOC *loc = (::CdlLOC*)subheader;
				uint32_t read_loc = ::CdPosToInt(loc);
				if (read_loc >= end_loc)
					Stop();
			}

			void Volume(int volume)
			{
				// Set CD mix
				::CdlATV cd_vol;
				cd_vol.val0 = cd_vol.val1 = cd_vol.val2 = cd_vol.val3 = volume;
				::CdMix(&cd_vol);
			}

			void Filter(int file, int channel)
			{
				// Change CD filter
				::CdlFILTER filter;
				filter.file = file;
				filter.chan = channel;
				::CdControlF(CdlSetfilter, (u_char*)&filter);
			}

			void Play(const CdlFILE *file)
			{
				// Get CD locations
				start_loc = ::CdPosToInt((CdlLOC*)&file->pos);
				end_loc = start_loc + ((file->size + 0x7FF) / 0x800);
				
				cur_loc = start_loc;
				
				// Start playing file
				u_char param[4] = {};
				param[0] = CdlModeRT | CdlModeSF | CdlModeSize1;
				::CdControlB(CdlSetmode, param, nullptr);

				::CdReadyCallback(ReadyCallback);
				::CdControl(CdlReadS, (u_char*)&file->pos, nullptr);
			}

			void Stop()
			{
				// Stop playing file
				::CdControlF(CdlPause, nullptr);
			}

			bool Playing()
			{
				::CdControl(CdlNop, nullptr, nullptr);
				return (::CdStatus() & CdlStatRead) != 0;
			}

			uint32_t Tell()
			{
				if (Playing())
				{
					// Get location from CD drive
					u_char result[8];
					::CdControlB(CdlGetlocP, nullptr, result);

					// Update location
					uint32_t next_loc = (DecBCD(result[2]) * 75 * 60) + (DecBCD(result[3]) * 75) + DecBCD(result[4]);
					if (next_loc >= end_loc)
					{
						Stop();
						cur_loc = end_loc;
					}
					else if (next_loc > cur_loc)
					{
						cur_loc = next_loc;
					}
				}

				return cur_loc - start_loc;
			}
		}

		void Seek(const CdlLOC *loc)
		{
			// Seek to location
			u_char param[4] = {};
			param[0] = CdlModeSpeed;
			::CdControl(CdlSetmode, param, nullptr);

			::CdControl(CdlSeekL, (u_char*)loc, nullptr);
		}
	}
}
