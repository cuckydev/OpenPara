/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Rap.h -
	Rap code
*/

#pragma once

#include "Boot/OpenPara.h"

#include "Util/Fixed.h"

namespace OpenPara
{
	namespace Rap
	{
		// Rap constants
		using Substep = Fixed<uint32_t, 5>;

		// static constexpr uint32_t SUBSTEPS_TIMED_E = SUBSTEPS * 0.4;
		// static constexpr uint32_t SUBSTEPS_TIMED_S = SUBSTEPS * 0.6;

		// static constexpr uint32_t SUBSTEPS_FUNK_F = SUBSTEPS * 0.1;
		// static constexpr uint32_t SUBSTEPS_FUNK_B = SUBSTEPS * 0.3;

		static constexpr uint64_t GetTimer(double bpm)
		{
			return (uint64_t)((double)(1ULL << 32) * ((bpm * 4.0 * Substep::base) / 60.0 / 75.0));
		}
		static Substep SectorSubstep(uint32_t sector, Substep start, uint64_t timer)
		{
			uint32_t substep = (timer * sector) >> 32;
			if (substep < start.to_raw())
				return Substep(0.0);
			return Substep::from_raw(substep) - start;
		}

		static constexpr uint32_t ToStep(uint32_t bar, uint32_t beat, uint32_t step)
		{
			return (step + (beat << 2) + (bar << 4));
		}
		static constexpr Substep ToSubstep(uint32_t bar, uint32_t beat, double step)
		{
			uint32_t istep = (uint32_t)(step * Substep::base);
			return Substep::from_raw(istep + ((beat << Substep::frac) << 2) + ((bar << Substep::frac) << 4));
		}

		// Rap types
		enum Rank
		{
			Cool,
			Good,
			Bad,
			Awful
		};

		enum Button
		{
			// Keys
			Triangle = (0 << 0),
			Circle = (1 << 0),
			Cross = (2 << 0),
			Square = (3 << 0),
			R = (4 << 0),
			L = (5 << 0),
			KeyMask = (Triangle | Circle | Cross | Square | R | L),

			// Modifiers
			Repeat = (1 << 3),
			Reset = (1 << 4)
		};

		struct Tap
		{
			// Tap info
			Substep time;
			uint32_t button;

			// Functions
			bool IsBackbeat()
			{
				return Substep::type(time.round()) & 1;
			}
			bool IsTimed()
			{
				Substep sub = time.mod1();
				return (sub <= Substep(0.35) || sub >= Substep(0.65));
			}
			bool IsFunky()
			{
				Substep sub = time.mod1();
				if (IsBackbeat())
					return (sub > Substep(0.3));
				else
					return (sub > Substep(0.1));
			}
		};

		struct Suggest
		{
			// Suggest info
			Substep time;
			uint32_t button;
		};

		enum LineFlag
		{
			Teacher = (1ULL << 0),
		};
		struct Line
		{
			// Line info
			Substep::type start, length;
			Substep::type rap_ahead;
			uint32_t flag;

			// Suggested buttons
			uint32_t suggests;
			const Suggest *suggest;
		};

		struct Chart
		{
			// Chart info
			Substep start;
			uint64_t timer;

			// Lines
			uint32_t lines;
			const Line *line;
		};

		// Rap helpers
		class Pair
		{
			private:
				// Pair state
				uint32_t pair_step = 0;
				uint32_t pair_v = 0;
				uint32_t pair[4] = {};

			public:
				// Pair functions
				void Submit(uint32_t step)
				{
					// Get pair X and check for push
					uint32_t px = step >> 1;
					uint32_t pv = 1 << (step & 1);

					while (px != pair_step)
					{
						pair[pair_v]++;
						pair_v = 0;
						pair_step++;
					}

					// Submit to pair
					pair_v |= pv;
				}

				int32_t Judge(int32_t worth_set, int32_t worth_00, int32_t worth_01, int32_t worth_10, int32_t worth_11)
				{
					// Get sets collected
					uint32_t sets = ~(uint32_t(0));
					for (auto &i : pair)
						if (i < sets)
							sets = i;
					
					// Judge pairs
					int32_t judge_result = worth_set * sets;
					judge_result += worth_00 * pair[0];
					judge_result += worth_01 * pair[1];
					judge_result += worth_10 * pair[2];
					judge_result += worth_11 * pair[3];

					return judge_result;
				}
		};

		// Rap class
		class Rap
		{
			private:
				// Chart state
				const Chart *chart = nullptr;
				
				Substep substep;
				Substep::type step;
				
				const Line *line_p, *line_e;

				// Rap state
				int32_t score = 0;
				Rank rank = Rank::Good;
				
				const Line *cur_line, *rap_line;
				
				uint32_t tap_inputs;
				Tap tap_input[512];

			public:
				// Chart functions
				Rap(const Chart *_chart) { SetChart(_chart); }

				void SetChart(const Chart *_chart);

				void SetTime(uint32_t sector);

				bool IsRapping();

				void StartRap(const Line *line);
				void SubmitRap(uint32_t button);
				int32_t JudgeRap();

				void Draw();

			private:
				// Internal chart functions
				void DrawTapSprite(Substep substep, uint8_t r, uint8_t g, uint8_t b);
				void DrawLine(const Line *line);

				const Line *NextLine(const Line *p)
				{
					p++;
					if (p == line_e)
						return nullptr;
					return p;
				}
		};
	}
}
