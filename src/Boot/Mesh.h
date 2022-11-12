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
				uint32_t speed;

			public:
				void Start(uint32_t _speed = 0x100)
				{
					start = GPU::g_frame;
					speed = _speed;
				}

				uint32_t Get()
				{
					uint32_t delta = GPU::g_frame - start;
					if (GPU::g_pal)
						return (uint64_t)delta * (speed * 150) / 360;
					else
						return (delta * speed) >> 1;
				}
		};

		// Anim types
		struct MIMe
		{
			Timer timer;
			void *vdf = nullptr, *dat = nullptr;

			void Play(void *_vdf, void *_dat, uint32_t speed = 0x100)
			{
				vdf = _vdf;
				dat = _dat;
				timer.Start(speed);
			}
		};
		
		// Mesh functions
		void DrawMIMe(const void *tmd, MIMe *mime);
	}
}
