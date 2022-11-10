/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- CxxGlue.cpp -
	C++ support glue
*/

#include "Boot/OpenPara.h"

#include "Boot/Mem.h"

extern "C"
{
	// Start static ctors
	typedef void (*fptr)();

	extern fptr __preinit_array_start[];
	extern fptr __preinit_array_end[];
	extern fptr __init_array_start[];
	extern fptr __init_array_end[];

	void __cxa_atexit(void (*func)(void*), void *arg, void *dso_handle) { (void)func; (void)arg; (void)dso_handle; }
	void *__dso_handle = nullptr;

	void cxxmain()
	{
		size_t count;

		count = __preinit_array_end - __preinit_array_start;
		for (size_t i = 0; i < count; i++)
		{
			fptr f = __preinit_array_start[i];
			if (f != nullptr)
				f();
		}

		count = __init_array_end - __init_array_start;
		for (size_t i = 0; i < count; i++)
		{
			fptr f = __init_array_start[i];
			if (f != nullptr)
				f();
		}

		OpenPara::Main();
	}

	// GCC built-in new and delete
	void *__builtin_new(size_t size) { return OpenPara::Mem::Alloc(size); }
	void  __builtin_delete(void *ptr) { OpenPara::Mem::Free(ptr); }
	
	// Pure virtual call
	int __cxa_guard_acquire(u_long *guard_object)
	{
		return !((*guard_object) & 1);
	}

	void __cxa_guard_release(u_long *guard_object)
	{
		*guard_object = 0;
		*guard_object |= 1;
	}

	void __cxa_guard_abort(u_long *guard_object)
	{
		*guard_object = 0;
	}

	void __cxa_pure_virtual(void) { OpenPara::Abort(); }
}

// C++ new and delete
void *operator new(size_t size) noexcept { return OpenPara::Mem::Alloc(size); }
void *operator new[](size_t size) noexcept { return OpenPara::Mem::Alloc(size); }

namespace std { enum class align_val_t : size_t {}; }
void *operator new(size_t size, std::align_val_t align) noexcept { (void)align; return OpenPara::Mem::Alloc(size); }
void *operator new[](size_t size, std::align_val_t align) noexcept { (void)align; return OpenPara::Mem::Alloc(size); }

void operator delete(void *ptr) noexcept { OpenPara::Mem::Free(ptr); }
void operator delete[](void *ptr) noexcept { OpenPara::Mem::Free(ptr); }
void operator delete(void *ptr, size_t size) noexcept { (void)size; OpenPara::Mem::Free(ptr); }
void operator delete[](void *ptr, size_t size) noexcept { (void)size; OpenPara::Mem::Free(ptr); }

// C++ placement new
void *operator new(size_t size, void *ptr) noexcept { (void)size; return ptr; }
void *operator new[](size_t size, void *ptr) noexcept { (void)size; return ptr; }
