/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- OpenPara.h -
	OpenPara app
*/

#pragma once

#ifdef __INTELLISENSE__
	#define u_char unsigned char
	#define u_short unsigned short
	#define u_long unsigned long
#endif

// PSX headers
#include <sys/types.h>
#include <sys/file.h>
#include <stdio.h>

#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libspu.h>
#include <libcd.h>
#include <libsnd.h>
#include <libapi.h>
#include <libpress.h>
#include <libmcrd.h>

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <hwregs_c.h>
#include <inline_n.h>
#include <gtemac.h>

void *operator new(size_t size, void *ptr) noexcept;
void *operator new[](size_t size, void *ptr) noexcept;

namespace OpenPara
{
	// Common macros
	#ifndef COUNTOF
		#define COUNTOF(x) (sizeof(x) / sizeof(x[0]))
	#endif

	// Main functions
	#ifdef __INTELLISENSE__
		#define DebugOut(...) {}
		#define DebugAbort(fmt, ...) {}
	#else
		#define DebugOut(fmt, ...) { ::printf("[%s] " fmt, __PRETTY_FUNCTION__ __VA_OPT__(,) __VA_ARGS__); }
		#define DebugAbort(fmt, ...) { ::printf("[%s] " fmt, __PRETTY_FUNCTION__ __VA_OPT__(,) __VA_ARGS__); OpenPara::Abort(); }
	#endif

	#if defined(__INTELLISENSE__)
		static inline void Abort()
		{
			::exit(1);
		}
	#else
		static inline void Abort()
		{
			::exit();
		}
	#endif

	// Scenes
	struct SceneFile
	{
		const char *name = nullptr;
		CdlFILE file = {};

		void Search()
		{
			if (name != nullptr && ::CdSearchFile(&file, (char*)name) == nullptr)
				DebugAbort("Search %s failed\n", name);
		}
	};

	struct SceneDef
	{
		SceneFile comod, compo, xa;
		void (*main)();

		void Search()
		{
			comod.Search();
			compo.Search();
			xa.Search();
		}
	};

	enum Scene
	{
		Scene0,
		Scene1,
		Scene2,
		Scene3,
		Scene5,
		Scene6,
		Scene7,
		Scene8,
		Scene9
	};

	extern SceneDef g_scenes[];
	extern Scene g_scene;
	
	// Entry point
	void Main();
}
