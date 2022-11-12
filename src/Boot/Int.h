/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Int.h -
	Int archive loader
*/

#pragma once

#include "Boot/OpenPara.h"

#include "Boot/CD.h"

namespace OpenPara
{
	namespace Int
	{
		// Int types
		struct IntFile
		{
			void *ptr;
			char name[16];
		};

		// Int class
		class Int
		{
			private:
				// Int memory pointers
				void *mem = nullptr;
				IntFile mem_file[(8192 + 0x13) / 0x14];

			public:
				// Int functions
				~Int();

				void Read(const ::CdlFILE *file);

				static void ReadyCallback(u_char status, u_char *result);

				IntFile &operator[](int i) { return mem_file[i]; }
		};
	}
}
