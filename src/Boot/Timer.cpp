/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Timer.cpp -
	PSX Timer
*/

#include "Boot/Timer.h"

// C externs
extern "C" void InterruptCallback(int index, void (*cb)());
extern "C" void ChangeClearRCnt(int t, int m);

namespace OpenPara
{
	namespace Timer
	{
		// Timer constants
		static constexpr Ticks TIMER_RATE = 1000;

		// Timer globals
		static Ticks ticks = 0;

		// Timer tick IRQ
		static void TickIRQ() { ticks++; }

		// Timer functions
		void Init()
		{
			::EnterCriticalSection();
			::ChangeClearRCnt(2, 0);
			::InterruptCallback(6, TickIRQ);

			// CLK/8 input, IRQ on reload, disable one-shot IRQ
			TIMER_CTRL(2)   = 0x0258;
			TIMER_RELOAD(2) = (F_CPU / 8) / TIMER_RATE;

			::ExitCriticalSection();
		}

		Ticks Tell()
		{
			return ticks;
		}

		void Seek(Ticks time)
		{
			ticks = time;
		}

		void Pause()
		{
			// Clear IRQ
			::EnterCriticalSection();
			::InterruptCallback(6, nullptr);
			::ExitCriticalSection();
		}

		void Resume()
		{
			// Resume IRQ
			::EnterCriticalSection();
			::ChangeClearRCnt(2, 0);
			::InterruptCallback(6, TickIRQ);
			::ExitCriticalSection();
		}
	}
}
