/*
	[ OpenParaPS1 ]
	Copyright Regan "CKDEV" Green 2022
	
	- Platform.cpp -
	PSX platform
*/

#include "Platform.h"

#include "CD.h"
#include "SPU.h"
#include "Pad.h"
#include "GPU.h"
#include "Timer.h"

namespace OpenPara
{
	namespace Platform
	{
		// Platform functions
		void Init()
		{
			// Initialize system
			::ResetCallback();

			// Initialize systems
			CD::Init();
			SPU::Init();
			Pad::Init();
			GPU::Init();
			Timer::Init();
		}
	}
}
