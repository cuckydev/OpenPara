/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- OpenPara.cpp -
	OpenPara app
*/

#include "Boot/OpenPara.h"

#include "Boot/CD.h"
#include "Boot/SPU.h"
#include "Boot/Pad.h"
#include "Boot/GPU.h"
#include "Boot/Timer.h"

#include "Boot/Mem.h"

#include "S1/S1.h"

// Linker addresses
extern "C" {
	extern char __heap_start, __ram_top;
}

namespace OpenPara
{
	// Scenes
	SceneDef g_scenes[] = {
		{ { "\\S0\\COMOD0.BIN;1" }, { "\\S0\\COMPO00.INT;1" }, {}, nullptr },
		{ { "\\S1\\COMOD1.BIN;1" }, { "\\S1\\COMPO01.INT;1" }, { "\\S1\\STAGE1.XA1;1" }, S1::Main },
		{ { "\\S2\\COMOD2.BIN;1" }, { "\\S2\\COMPO02.INT;1" }, { "\\S2\\STAGE2.XA1;1" }, nullptr },
		{ { "\\S3\\COMOD3.BIN;1" }, { "\\S3\\COMPO03.INT;1" }, { "\\S3\\STAGE3.XA1;1" }, nullptr },
		{ { "\\S5\\COMOD5.BIN;1" }, { "\\S5\\COMPO05.INT;1" }, { "\\S5\\STAGE5.XA1;1" }, nullptr },
		{ { "\\S6\\COMOD6.BIN;1" }, { "\\S6\\COMPO06.INT;1" }, { "\\S6\\STAGE6.XA1;1" }, nullptr },
		{ { "\\S7\\COMOD7.BIN;1" }, { "\\S7\\COMPO07.INT;1" }, { "\\S7\\STAGE7.XA1;1" }, nullptr },
		{ { "\\S8\\COMOD8.BIN;1" }, { "\\S8\\COMPO08.INT;1" }, {}, nullptr },
		{ { "\\S9\\COMOD9.BIN;1" }, { "\\S9\\COMPO09.INT;1" }, { "\\S9\\STAGE9.XA1;1" }, nullptr },
	};
	Scene g_scene;

	// Entry point
	void Main()
	{
		// Initialize system
		::ResetCallback();

		// Initialize systems
		CD::Init();
		SPU::Init();
		Pad::Init();
		GPU::Init();
		Timer::Init();

		// Find scene files
		for (auto &i : g_scenes)
			i.Search();

		// Start game
		g_scene = Scene::Scene1;

		// Scene loop
		while (1)
		{
			// Read module file
			::CdlFILE &file = g_scenes[g_scene].comod.file;

			size_t size = file.size;
			size_t sectors = (size + 0x7FF) >> 11;
			
			::CdControlB(CdlSetloc, (u_char*)&file.pos, nullptr);
			::CdRead(sectors, (u_long*)&__heap_start, CdlModeSpeed);
			::CdReadSync(0, nullptr);
			
			// Flush CPU cache to avoid executing stale memory
			::FlushCache();
			
			// Set heap to end of overlay data
			Mem::Init(&__heap_start + size, ((uintptr_t)(&__ram_top) - 0x200) - (uintptr_t)(&__heap_start) - size);

			// Execute module
			g_scenes[g_scene].main();
		}
	}
}
