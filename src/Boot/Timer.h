/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Timer.h -
	PSX Timer
*/

#pragma once

#include "OpenPara.h"

namespace OpenPara
{
	namespace Timer
	{
		// Timer types
		typedef int32_t Ticks;

		// Timer functions
		void Init();

		Ticks Tell();
		void Seek(Ticks time);

		void Pause();
		void Resume();
	}
}
