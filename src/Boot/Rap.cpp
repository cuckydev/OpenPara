/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- Rap.cpp -
	Rap code
*/

#include "Boot/Rap.h"

#include "Boot/GPU.h"

namespace OpenPara
{
	namespace Rap
	{
		// Chart functions
		void Rap::Start(void *_stage, const Chart *_chart)
		{
			// Set parent stage
			stage = _stage;

			// Set and start chart
			chart = _chart;

			substep = Substep(0.0);
			step = 0;

			line_p = chart->line;
			line_e = chart->line + chart->lines;

			time_event_p = chart->time_event;
			time_event_e = chart->time_event + chart->time_events;

			// Initialize rap
			score = 0;
			rank = Rank::Good;

			StartRap(nullptr);
		}

		void Rap::SetTime(uint32_t sector)
		{
			// Set chart time
			Substep next_substep = SectorSubstep(sector, chart->start, chart->timer);
			if (next_substep > substep)
			{
				// Update substep, step if the song has just started
				if (substep == Substep(0.0))
					chart->stepped(stage);
				substep = next_substep;

				// Update step
				uint32_t next_step = Substep::type(next_substep);
				for (; step < next_step;)
				{
					step++;
					chart->stepped(stage);
				}
			}

			// Scroll line
			while (line_p != nullptr && step >= (line_p->start + line_p->length + 16))
				line_p = NextLine(line_p);
			
			// Get current line
			cur_line = nullptr;
			for (const Line *line = line_p; line != nullptr; line = NextLine(line))
			{
				if (step >= (line->start + line->length))
					continue;
				if (line->start > step)
					break;
				cur_line = line;
				break;
			}
			
			// Get rapping line
			const Line *next_rap_line = nullptr;
			for (const Line *line = cur_line; line != nullptr; line = NextLine(line))
			{
				if (step >= (line->start + line->length))
					continue;
				if (line->start > step + line->rap_ahead)
					break;
				if (!(line->flag & LineFlag::Teacher))
				{
					next_rap_line = line;
					break;
				}
			}

			// Judge if line changed
			if (next_rap_line != rap_line)
			{
				if (rap_line != nullptr)
				{
					int32_t judge = JudgeRap();
					score += judge;
					if (score < 0)
						score = 0;
				}
				StartRap(next_rap_line);
			}
		}

		bool Rap::IsRapping()
		{
			return rap_line != nullptr;
		}

		void Rap::StartRap(const Line *line)
		{
			// Set rap line pointer
			rap_line = line;

			// Initialize rap
			tap_inputs = 0;
			for (auto &i : tap_eventi)
				i = 0;
		}

		void Rap::SubmitRap(uint32_t button)
		{
			(void)button;

			// Check if tap buffer is full
			if (tap_inputs >= COUNTOF(tap_input))
				return;
			
			// Insert tap
			Tap tap;
			tap.time = substep;
			tap.button = button;
			tap_input[tap_inputs++] = tap;

			// Handle tap events
			if (button == Button::Reset)
			{
				// Reset tap events
				for (auto &i : tap_eventi)
					i = 0;
			}
			else
			{
				uint32_t key = button & Button::KeyMask;
				uint32_t events = rap_line->tap_event[key].events;
				
				if (rap_line->tap_event[key].events != 0)
				{
					// Run tap event
					const TapEvent &event = rap_line->tap_event[key].event[tap_eventi[key]];
					event.func(stage, event.user);

					// Increment tap event
					if (!(button & Button::Repeat))
						if (++tap_eventi[key] == events)
							tap_eventi[key] = 0;
				}
			}
		}

		int32_t Rap::JudgeRap()
		{
			// Reject too many taps
			if (tap_inputs >= rap_line->length * 2)
				return -100;
			
			// Judge scores
			int32_t judge_timed = 0;
			int32_t judge_penalty = 0;
			
			if (rank != Rank::Cool)
				judge_timed = -2 * rap_line->suggests;

			// Judge pairs
			Pair pair_freestyle;
			Pair pair_funk;

			// Collect required keys
			uint32_t req_keys = 0, tap_keys = 0;
			if (rank != Rank::Cool)
			{
				for (uint32_t i = 0; i < rap_line->suggests; i++)
					req_keys |= 1 << rap_line->suggest[i].button;
			}

			if (rank == Rank::Cool)
			{
				// Judge taps
				for (uint32_t i = 0; i < tap_inputs; i++)
				{
					// Get tap to judge
					Tap &tap = tap_input[i];
					if (tap.button == Button::Reset)
						continue;

					// Check if tap was timed
					if (tap.IsTimed())
					{
						// Reward timed inputs
						judge_timed += 3;

						// Submit for freestyle
						pair_freestyle.Submit(Substep::type(tap.time.round()));

						// Submit for funk
						if (tap.IsFunky())
							pair_funk.Submit(Substep::type(tap.time.round()));
					}
					else
					{
						// Penalize mistimed inputs
						judge_timed -= 3;
					}
				}

				// Final judge
				return judge_timed + pair_freestyle.Judge(18, 0, 15, 6, 9) + pair_funk.Judge(27, 0, 22, 9, 13);
			}
			else
			{
				// Get suggests to compare against
				const Suggest *suggest_p = rap_line->suggest;
				const Suggest *suggest_e = rap_line->suggest + rap_line->suggests;

				// Judge taps
				for (uint32_t i = 0; i < tap_inputs; i++)
				{
					// Get tap to judge
					Tap &tap = tap_input[i];
					if (tap.button == Button::Reset)
						continue;

					// Mark key as pressed
					uint32_t key_bit = 1 << (tap.button & Button::KeyMask);
					tap_keys |= key_bit;

					// Get nearby suggest
					while (suggest_p != nullptr && Substep::type(suggest_p->time.round()) < Substep::type(tap.time.round()))
					{
						if (++suggest_p == suggest_e)
							suggest_p = nullptr;
					}
					const Suggest *suggest_c = (suggest_p != nullptr && Substep::type(suggest_p->time.round()) == Substep::type(tap.time.round())) ? suggest_p : nullptr;

					// Check if tap was timed
					if (tap.IsTimed() && (req_keys & key_bit) != 0)
					{
						// Reward timed inputs
						if (i < rap_line->suggests)
							judge_timed += 4;
						else
							judge_timed += 1;
						
						// Reward playing the suggested line
						if (suggest_c != nullptr && suggest_c->button == tap.button)
							judge_timed += 1;

						// Submit for freestyle
						pair_freestyle.Submit(Substep::type(tap.time.round()));

						// Submit for funk
						if (tap.IsFunky())
							pair_funk.Submit(Substep::type(tap.time.round()));
					}
					else
					{
						// Penalize mistimed inputs
						if (i < rap_line->suggests)
							judge_penalty += 2;
						else
							judge_penalty += 6;
					}
				}

				// Final judge
				if ((req_keys & tap_keys) != req_keys)
					return -judge_penalty;
				if (judge_timed <= 0)
					judge_penalty = 0;
				return judge_timed - judge_penalty + pair_freestyle.Judge(18, 0, 15, 6, 9) + pair_funk.Judge(27, 0, 22, 9, 13);
			}
		}

		void Rap::DrawTapSprite(Substep time, unsigned char r, unsigned char g, unsigned char b)
		{
			// Get screen coordinates
			int32_t sx = ((int32_t)GPU::g_width - (16 * 16)) / 2;
			int32_t sy = 32;
			
			sx = sx + (Substep::type(time * Substep(16.0)) & 0xFF);
			sy = sy + ((Substep::type(time) / 16) * 16) - (Substep::type(substep));
			if (sy <= 32 - 16 || sy > 32 + 16)
				return;

			GPU::FillRect(OpenPara::GPU::OT::UI, ::RECT{short(sx), short(sy), 16, 16}, r, g, b);
		}
		
		void Rap::DrawLine(const Line *line)
		{
			(void)line;

			// Tap colours
			static const struct
			{
				uint8_t r, g, b;
			} cols[6] = {
				{0x00, 0xFF, 0x00},
				{0xFF, 0x00, 0x00},
				{0x30, 0x00, 0xFF},
				{0xFF, 0x00, 0xFF},
				{0x00, 0x00, 0xFF},
				{0xFF, 0xFF, 0x00},
			};
			(void)cols;

			// Draw tapped line
			{
				for (uint32_t i = 0; i < tap_inputs; i++)
				{
					const Tap &tap = tap_input[i];
					if (tap.button == Button::Reset)
						continue;
					uint32_t button = tap.button & Button::KeyMask;
					DrawTapSprite(tap.time, cols[button].r, cols[button].g, cols[button].b);
				}
			}

			// Draw suggested line
			{
				for (uint32_t i = 0; i < line->suggests; i++)
				{
					const Suggest &suggest = line->suggest[i];
					DrawTapSprite(suggest.time, cols[suggest.button].r, cols[suggest.button].g, cols[suggest.button].b);
				}
			}
		}

		void Rap::Draw()
		{
			if (cur_line != nullptr)
			{
				// Draw head
				DrawTapSprite(substep, 0xFF, 0x20, 0x00);

				// Draw lines
				for (const Line *line = line_p; line != nullptr; line = NextLine(line))
				{
					if (line->start > step + 32)
						break;
					DrawLine(line);
				}
			}
		}
	}
}
