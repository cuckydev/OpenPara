/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Mem.h -
	Memory allocator
*/

#pragma once

#include "OpenPara.h"

namespace OpenPara
{
	namespace Mem
	{
		// Mem functions
		void Init(void *ptr, size_t addr);
		
		void *Alloc(size_t size);
		void Free(void *ptr);
	}
}
