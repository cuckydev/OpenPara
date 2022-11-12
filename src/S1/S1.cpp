/*
	[ OpenPara ]
	Copyright Regan "CKDEV" Green 2022
	
	- S1.cpp -
	Scene 1
*/

#include "S1/S1.h"

#include "Boot/OpenPara.h"
#include "Boot/Rap.h"
#include "Boot/Int.h"
#include "Boot/Mesh.h"

#include "Boot/GPU.h"
#include "Boot/Pad.h"
#include "Boot/CD.h"

namespace OpenPara
{
	namespace S1
	{
		// Stage types
		struct PaRap
		{
			uint32_t anim, prog, tone;
		};

		class S1
		{
			private:
				// Stage data
				Int::Int int_compo;

				// Rap state
				Rap::Rap rap;
				
				// Onion state
				Mesh::MIMe on_mime[2];

				void *on_tmd;
				Int::IntFile *on_vdf, *on_dat;

				// PaRappa state
				Mesh::MIMe pa_mime[2];
				Rap::Substep::type pa_rapdb = 0;

				void *pa_tmd;
				Int::IntFile *pa_vdf, *pa_dat;
				
				// Beat event
				static void Event_Stepped(void *stage_void)
				{
					// Get arguments
					S1 *stage = (S1*)stage_void;
					Rap::Substep::type step = stage->rap.GetStep();
					DebugOut("%d.%d.%d\n", step / 16, (step / 4) & 3, step & 3);

					// PaRappa idle
					uint32_t pa_idle, pa_idle_time, pa_idle_speed;
					if (step >= Rap::ToStep(55, 0, 0))
					{
						pa_idle = 18; // PA_WT3
						pa_idle_time = 16;
						pa_idle_speed = 0xF8;
					}
					else if (step >= Rap::ToStep(47, 0, 0))
					{
						pa_idle = 17; // PA_WT2
						pa_idle_time = 16;
						pa_idle_speed = 0xF8;
					}
					else if (step >= Rap::ToStep(33, 0, 0))
					{
						pa_idle = 16; // PA_WT1
						pa_idle_time = 8;
						pa_idle_speed = 0xF0;
					}
					else
					{
						pa_idle = 15; // PA_WT0
						pa_idle_time = 8;
						pa_idle_speed = 0x100;
					}

					if ((step & 3) == 0 && step >= stage->pa_rapdb)
					{
						stage->pa_rapdb = stage->rap.GetStep() + pa_idle_time;
						stage->pa_mime[0].Play(stage->pa_vdf[pa_idle].ptr, stage->pa_dat[pa_idle].ptr, pa_idle_speed);
					}
				}

				// PaRappa rap event
				static constexpr PaRap parap_kick  = { 6, 0, 0 }; // PA_KICK
				static constexpr PaRap parap_punch = { 9, 0, 0 }; // PA_PNCH
				static constexpr PaRap parap_chop  = { 1, 0, 0 }; // PA_CHOP
				static constexpr PaRap parap_block = { 0, 0, 0 }; // PA_BLCK

				static void Event_PaRap(void *stage_void, void *user)
				{
					// Get arguments
					S1 *stage = (S1*)stage_void;
					PaRap *parap = (PaRap*)user;

					// Play animation
					stage->pa_rapdb = stage->rap.GetStep() + 8;
					stage->pa_mime[0].Play(stage->pa_vdf[parap->anim].ptr, stage->pa_dat[parap->anim].ptr);
				}

				// Stage chart
				#include "S1/Chart.h"

			public:
				// Stage functions
				S1()
				{
					// Read stage data
					int_compo.Read(&g_scenes[g_scene].compo.file);

					// Get Onion data
					on_tmd = int_compo[40].ptr;
					on_vdf = &(int_compo[49]);
					on_dat = &(int_compo[101]);

					// Get PaRappa data
					pa_tmd = int_compo[41].ptr;
					pa_vdf = &(int_compo[81]);
					pa_dat = &(int_compo[133]);
				}

				bool Main()
				{
					// Start rap
					rap.Start((void*)this, &chart);
					
					// Start music
					CD::XA::Volume(0x40);
					CD::XA::Filter(1, 1);
					CD::XA::Play(&g_scenes[g_scene].xa.file);

					// Stage loop
					while (1)
					{
						// Poll pad
						Pad::Poll();

						// Run rap
						rap.SetTime(CD::XA::Tell());

						// Submit rap inputs
						if (rap.IsRapping())
						{
							uint32_t modifier = 0;
							if (Pad::g_pad[0].held & Pad::Button::Left)
								rap.SubmitRap(Rap::Button::Reset);
							if (Pad::g_pad[0].held & Pad::Button::Right)
								modifier |= Rap::Button::Repeat;

							if (Pad::g_pad[0].press & Pad::Button::Triangle)
								rap.SubmitRap(Rap::Button::Triangle | modifier);
							if (Pad::g_pad[0].press & Pad::Button::Circle)
								rap.SubmitRap(Rap::Button::Circle | modifier);
							if (Pad::g_pad[0].press & Pad::Button::Cross)
								rap.SubmitRap(Rap::Button::Cross | modifier);
							if (Pad::g_pad[0].press & Pad::Button::Square)
								rap.SubmitRap(Rap::Button::Square | modifier);
							if (Pad::g_pad[0].press & Pad::Button::R1)
								rap.SubmitRap(Rap::Button::R | modifier);
							if (Pad::g_pad[0].press & Pad::Button::L1)
								rap.SubmitRap(Rap::Button::L | modifier);
						}
						
						// Scene draw
						rap.Draw();

						// Set camera TEMP
						gte_SetGeomOffset(GPU::g_width / 2, GPU::g_height / 2);
						gte_SetGeomScreen(440);

						MATRIX mat = {{{},{},{}},{}};
						mat.m[0][0] = 0x1000;
						mat.m[1][1] = 0x1000;
						mat.m[2][2] = 0x1000;
						mat.t[0] = 0x0200;
						mat.t[1] = 0x0300;
						mat.t[2] = 0x1000;

						gte_SetTransMatrix(&mat);
						gte_SetRotMatrix(&mat);

						// Draw PaRappa
						Mesh::DrawMIMe(pa_tmd, pa_mime);

						// Flip GPU
						GPU::FillRect(GPU::OT::Length - 1, ::RECT{0, 0, short(GPU::g_width), short(GPU::g_height)}, 0, 0, 0);
						GPU::Flip();
					}

					return false;
				}
		};

		// Entry point
		void Main()
		{
			// Start stage
			alignas(S1) static char stage[sizeof(S1)];
			new(stage) S1();

			while (((S1*)stage)->Main());
			
			/*
			// PaRappa model data
			void *on_tmd = int_compo[40];

			void *pa_tmd = int_compo[41];
			Anim::MIMe pa_anim_kick = { int_compo[81 + 6], int_compo[133 + 6] };
			Anim::MIMe pa_anim_punch = { int_compo[81 + 9], int_compo[133 + 9] };
			Anim::MIMe pa_anim_chop = { int_compo[81 + 1], int_compo[133 + 1] };
			Anim::MIMe pa_anim_block = { int_compo[81 + 0], int_compo[133 + 0] };

			Anim::Anim pa_anim;
			pa_anim.SetAnim(&pa_anim_block);

			// Start rap
			Rap::Rap rap(&chart);

			CD::XA::Volume(0x40);
			CD::XA::Filter(1, 1);
			CD::XA::Play(&g_scenes[g_scene].xa.file);

			while (1)
			{
				// Poll pad
				Pad::Poll();

				// Run rap
				rap.SetTime(CD::XA::Tell());

				// Submit rap inputs
				if (rap.IsRapping())
				{
					uint32_t modifier = 0;
					if (Pad::g_pad[0].held & Pad::Button::Left)
						rap.SubmitRap(Rap::Button::Reset);
					if (Pad::g_pad[0].held & Pad::Button::Right)
						modifier |= Rap::Button::Repeat;

					if (Pad::g_pad[0].press & Pad::Button::Triangle)
						rap.SubmitRap(Rap::Button::Triangle | modifier);
					if (Pad::g_pad[0].press & Pad::Button::Circle)
						rap.SubmitRap(Rap::Button::Circle | modifier);
					if (Pad::g_pad[0].press & Pad::Button::Cross)
						rap.SubmitRap(Rap::Button::Cross | modifier);
					if (Pad::g_pad[0].press & Pad::Button::Square)
						rap.SubmitRap(Rap::Button::Square | modifier);
					if (Pad::g_pad[0].press & Pad::Button::R1)
						rap.SubmitRap(Rap::Button::R | modifier);
					if (Pad::g_pad[0].press & Pad::Button::L1)
						rap.SubmitRap(Rap::Button::L | modifier);
				}
				
				// Scene draw
				rap.Draw();
				
				if (Pad::g_pad[0].press & Pad::Button::Triangle)
					pa_anim.SetAnim(&pa_anim_kick);
				if (Pad::g_pad[0].press & Pad::Button::Circle)
					pa_anim.SetAnim(&pa_anim_punch);
				if (Pad::g_pad[0].press & Pad::Button::Cross)
					pa_anim.SetAnim(&pa_anim_chop);
				if (Pad::g_pad[0].press & Pad::Button::Square)
					pa_anim.SetAnim(&pa_anim_block);

				// Draw PaRappa
				gte_SetGeomOffset(GPU::g_width / 2, GPU::g_height / 2);
				gte_SetGeomScreen(440);

				MATRIX mat = {{{},{},{}},{}};
				mat.m[0][0] = 0x1000;
				mat.m[1][1] = 0x1000;
				mat.m[2][2] = 0x1000;
				mat.t[0] = 0x0200;
				mat.t[1] = 0x0300;
				mat.t[2] = 0x1000;

				gte_SetTransMatrix(&mat);
				gte_SetRotMatrix(&mat);

				uint32_t pa_anim_time = pa_anim.GetTime();
				Anim::MIMe *pa_anim_mime = (Anim::MIMe*)pa_anim.GetAnim();
				Mesh::DrawMIMe(pa_tmd, pa_anim_time, pa_anim_mime->vdf, pa_anim_mime->dat);

				mat.t[0] = -0x0200;
				mat.t[1] = 0x0300;
				mat.t[2] = 0x1000;

				gte_SetTransMatrix(&mat);
				gte_SetRotMatrix(&mat);

				Mesh::DrawMIMe(on_tmd, 0, nullptr, nullptr);
				
				// Flip GPU
				GPU::FillRect(GPU::OT::Length - 1, ::RECT{0, 0, short(GPU::g_width), short(GPU::g_height)}, 0, 0, 0);
				GPU::Flip();
			}
			*/
		}
	}
}
