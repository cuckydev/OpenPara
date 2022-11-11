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
					start = 0;//VSync(-1);
				}

				void *GetAnim()
				{
					return anim;
				}

				uint32_t GetTime()
				{
					return (start++) * 0x8000;
					//uint32_t delta = VSync(-1) - start;
					//if (GPU::g_pal)
					//	return (uint64_t)delta * 0x960000 / 360;
					//else
					//	return delta * 0x8000;
				}
		};
	}
}