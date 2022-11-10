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
		// Rap constants

		// Chart functions
		void Rap::SetChart(const Chart *_chart)
		{
			// Set and start chart
			chart = _chart;

			substep = step = 0;
			line_p = chart->line;
			line_e = chart->line + chart->lines;

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
				substep = next_substep;
				step = Substep::type(next_substep);
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
					DebugOut("SCORE: %d\n", score);
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

			// Get suggests to compare against
			const Suggest *suggest_p = rap_line->suggest;
			const Suggest *suggest_e = rap_line->suggest + rap_line->suggests;

			for (uint32_t i = 0; i < tap_inputs; i++)
			{
				// Get tap to judge
				Tap &tap = tap_input[i];

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

				// Check if note is in the line and properly timed
				if (rank == Rank::Cool)
				{
					if (tap.IsTimed())
						judge_timed += 3; // Reward timed inputs
					else
						judge_timed -= 3; // Penalize mistimed inputs
					
					// Submit for freestyle
					pair_freestyle.Submit(Substep::type(tap.time.round()));

					// Submit for funk
					if (tap.IsFunky())
						pair_funk.Submit(Substep::type(tap.time.round()));
				}
				else
				{
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
			}

			// Apply scores
			if (rank == Rank::Cool)
			{
				return judge_timed + pair_freestyle.Judge(18, 0, 15, 6, 9) + pair_funk.Judge(27, 0, 22, 9, 13);
			}
			else
			{
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
				{0xFF, 0xFF, 0x00},
				{0x00, 0x00, 0xFF},
			};
			(void)cols;

			// Draw head
			DrawTapSprite(substep, 0xFF, 0x20, 0x00);

			// Draw tapped line
			{
				for (uint32_t i = 0; i < tap_inputs; i++)
				{
					const Tap &tap = tap_input[i];
					DrawTapSprite(tap.time, cols[tap.button].r, cols[tap.button].g, cols[tap.button].b);
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

			/*
			// Tap colours
			static const struct
			{
				int r, g, b;
			} cols[6] = {
				{0x00, 0xFF, 0x00},
				{0xFF, 0x00, 0x00},
				{0x30, 0x00, 0xFF},
				{0xFF, 0x00, 0xFF},
				{0xFF, 0xFF, 0x00},
				{0x00, 0x00, 0xFF},
			};
			

			// Get screen coordinates
			int32_t sx = ((int32_t)GPU::g_width - (16 * 16)) / 2;
			int32_t sy = 32;

			// Get beat alignment
			uint32_t ss = line->start;

			// Draw head
			{
				uint32_t s = step - ss;
				uint32_t subs = substep & SUBSTEPS_AND;

				int32_t x = sx + ((s & 15) << 4) + (subs * 16 / SUBSTEPS);
				int32_t y = sy + ((s >> 4) << 4);
				
				GPU::FillRect(OpenPara::GPU::OT::UI, ::RECT{short(x + 2), short(y + 2), 12, 12}, 0xFF, 0x20, 0x00);
			}

			// Draw suggested line
			{
				for (uint32_t i = 0; i < line->suggests; i++)
				{
					const Suggest &suggest = line->suggest[i];
					
					uint32_t s = (suggest.time / SUBSTEPS) - ss;
					uint32_t subs = suggest.time & SUBSTEPS_AND;

					int32_t x = sx + ((s & 15) << 4) + (subs * 16 / SUBSTEPS);
					int32_t y = sy + ((s >> 4) << 4);
					
					int key = suggest.button & Button::KeyMask;
					GPU::FillRect(OpenPara::GPU::OT::UI, ::RECT{short(x + 2), short(y + 2), 12, 12}, cols[key].r, cols[key].g, cols[key].b);
				}
			}

			// Draw beats
			{

				for (uint32_t s = line->start; s < (line->start + line->length); s++)
				{
					int32_t x = sx + (((s - ss) & 15) << 4);
					int32_t y = sy + (((s - ss) >> 4) << 4);

					if (s & 3)
						GPU::FillRect(OpenPara::GPU::OT::UI, ::RECT{short(x + 6), short(y + 6), 4, 4}, 0xFF, 0xFF, 0xFF);
					else
						GPU::FillRect(OpenPara::GPU::OT::UI, ::RECT{short(x + 4), short(y + 4), 8, 8}, 0xFF, 0x80, 0x00);
				}
			}
			*/
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
