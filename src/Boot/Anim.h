/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Anim.h -
	Anim code
*/

#pragma once

#include "Boot/OpenPara.h"

#include "Boot/GPU.h"

namespace OpenPara
{
	namespace Anim
	{
		// Anim containers
		struct MIMe
		{
			void *vdf, *dat;
		};

		// Anim class
		class Anim
		{
			private:
				void *anim = nullptr;
				int start;

			public:
				void SetAnim(void *_anim)
				{
					anim = _anim;
					start = GPU::g_frame;
				}

				void *GetAnim()
				{
					return anim;
				}

				uint32_t GetTime()
				{
					uint32_t delta = GPU::g_frame - start;
					if (GPU::g_pal)
						return (uint64_t)delta * 0x9600 / 360;
					else
						return delta << 7;
				}
		};
	}
}