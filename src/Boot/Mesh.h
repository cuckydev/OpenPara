/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Mesh.h -
	Mesh drawing
*/

#pragma once

#include "Boot/OpenPara.h"

#include "Boot/GPU.h"

namespace OpenPara
{
	namespace Mesh
	{
		// Timer class
		class Timer
		{
			private:
				int start = GPU::g_frame;

			public:
				void Start()
				{
					start = GPU::g_frame;
				}

				uint32_t Get()
				{
					uint32_t delta = GPU::g_frame - start;
					if (GPU::g_pal)
						return (uint64_t)delta * 0x9600 / 360;
					else
						return delta << 7;
				}
		};

		// Anim types
		struct MIMe
		{
			Timer timer;
			void *vdf = nullptr, *dat = nullptr;

			void Play(void *_vdf, void *_dat)
			{
				vdf = _vdf;
				dat = _dat;
				timer.Start();
			}
		};
		
		// Mesh functions
		void DrawMIMe(const void *tmd, MIMe *mime);
	}
}
