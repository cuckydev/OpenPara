/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Platform.h -
	Platform dependent code
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
	namespace Platform
	{
		// Platform functions
		void Init();

		static void FlushCache() { ::FlushCache(); }
	}
}
