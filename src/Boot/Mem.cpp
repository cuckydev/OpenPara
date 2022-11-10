/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Mem.cpp -
	Memory allocator
*/

#include "Boot/Mem.h"

#include "Boot/OpenPara.h"

namespace OpenPara
{
	namespace Mem
	{
		// Mem alignment
		static constexpr uintptr_t ALIGNMENT = 0x10;
		
		template<typename T>
		static T Align(T x) { return T(((uintptr_t)x + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1)); }
		
		template<typename T>
		static T AlignEnd(T x) { return T((uintptr_t)x & ~(ALIGNMENT - 1)); }
		
		// Mem heap
		struct Header
		{
			Header *prev, *next;
			size_t size;
		};
		static Header *mem;

		// Mem functions
		void Init(void *ptr, size_t size)
		{
			// Initialize header
			mem = (Header*)Align(ptr);
			mem->prev = nullptr;
			mem->next = nullptr;
			mem->size = ((char*)ptr + size) - (char*)mem;

			DebugOut("Mem = (0x%p, 0x%X)\n", ptr, size);
		}
		
		void *Alloc(size_t size)
		{
			// Align size
			size = Align(size) + Align(sizeof(Header));

			// Get header pointer
			Header *head, *prev, *next;
			char *hpos = (char*)mem + Align(sizeof(Header));
			
			prev = mem;
			next = prev->next;
			
			while (1)
			{
				if (next != NULL)
				{
					// Check against the next block
					size_t cleft = (char*)next - hpos;
					if (cleft >= size)
					{
						// Set pointer
						head = (Header*)hpos;
						break;
					}
					
					// Check next header
					hpos = (char*)next + next->size;
					prev = next;
					next = prev->next;
				}
				else
				{
					// Check against end of heap
					size_t cleft = ((char*)mem + mem->size) - hpos;
					if (cleft < size)
						return nullptr;
					
					// Set pointer
					head = (Header*)hpos;
					break;
				}
			}

			// Link header
			head->size = size;
			head->prev = prev;
			if ((head->next = prev->next) != nullptr)
				head->next->prev = head;
			prev->next = head;

			// Return pointer
			return (void*)(hpos + Align(sizeof(Header)));
		}

		void Free(void *ptr)
		{
			// Get header
			if (ptr == nullptr)
				return;
			Header *head = (Header*)((char*)ptr - Align(sizeof(Header)));

			// Unlink header
			if ((head->prev->next = head->next) != nullptr)
				head->next->prev = head->prev;
		}
	}
}
