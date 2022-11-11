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
#include "Boot/Anim.h"

#include "Boot/GPU.h"
#include "Boot/Pad.h"
#include "Boot/CD.h"

namespace OpenPara
{
	namespace S1
	{
		// Song chart
		static const Rap::Chart chart = {
			// Chart info
			Rap::ToSubstep(0, 3, 0.0),
			Rap::GetTimer(110.0),

			// Lines
			16,
			(const Rap::Line[])
			{
				// KICK
				Rap::Line{
					Rap::ToStep(6, 0, 0), Rap::ToStep(0, 2, 0), 4,
					Rap::LineFlag::Teacher,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(6, 0, 0), Rap::Button::Triangle }
					}
				},
				Rap::Line{
					Rap::ToStep(6, 2, 0), Rap::ToStep(0, 2, 0), 4,
					0,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(6, 2, 0), Rap::Button::Triangle }
					}
				},
				// PUNCH
				Rap::Line{
					Rap::ToStep(7, 0, 0), Rap::ToStep(0, 2, 0), 4,
					Rap::LineFlag::Teacher,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(7, 0, 0), Rap::Button::Circle }
					}
				},
				Rap::Line{
					Rap::ToStep(7, 2, 0), Rap::ToStep(0, 2, 0), 4,
					0,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(7, 2, 0), Rap::Button::Circle }
					}
				},
				// CHOP
				Rap::Line{
					Rap::ToStep(8, 0, 0), Rap::ToStep(0, 2, 0), 4,
					Rap::LineFlag::Teacher,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(8, 0, 0), Rap::Button::Cross }
					}
				},
				Rap::Line{
					Rap::ToStep(8, 2, 0), Rap::ToStep(0, 2, 0), 4,
					0,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(8, 2, 0), Rap::Button::Cross }
					}
				},
				// BLOCK
				Rap::Line{
					Rap::ToStep(9, 0, 0), Rap::ToStep(0, 2, 0), 4,
					Rap::LineFlag::Teacher,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(9, 0, 0), Rap::Button::Square }
					}
				},
				Rap::Line{
					Rap::ToStep(9, 2, 0), Rap::ToStep(0, 2, 0), 4,
					0,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(9, 2, 0), Rap::Button::Square }
					}
				},

				// KICK
				Rap::Line{
					Rap::ToStep(10, 0, 0), Rap::ToStep(0, 2, 0), 4,
					Rap::LineFlag::Teacher,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(10, 0, 0), Rap::Button::Triangle }
					}
				},
				Rap::Line{
					Rap::ToStep(10, 2, 0), Rap::ToStep(0, 2, 0), 4,
					0,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(10, 2, 0), Rap::Button::Triangle }
					}
				},
				// PUNCH
				Rap::Line{
					Rap::ToStep(11, 0, 0), Rap::ToStep(0, 2, 0), 4,
					Rap::LineFlag::Teacher,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(11, 0, 0), Rap::Button::Circle }
					}
				},
				Rap::Line{
					Rap::ToStep(11, 2, 0), Rap::ToStep(0, 2, 0), 4,
					0,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(11, 2, 0), Rap::Button::Circle }
					}
				},
				// CHOP
				Rap::Line{
					Rap::ToStep(12, 0, 0), Rap::ToStep(0, 2, 0), 4,
					Rap::LineFlag::Teacher,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(12, 0, 0), Rap::Button::Cross }
					}
				},
				Rap::Line{
					Rap::ToStep(12, 2, 0), Rap::ToStep(0, 2, 0), 4,
					0,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(12, 2, 0), Rap::Button::Cross }
					}
				},
				// BLOCK
				Rap::Line{
					Rap::ToStep(13, 0, 0), Rap::ToStep(0, 2, 0), 4,
					Rap::LineFlag::Teacher,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(13, 0, 0), Rap::Button::Square }
					}
				},
				Rap::Line{
					Rap::ToStep(13, 2, 0), Rap::ToStep(0, 2, 0), 4,
					0,

					1,
					(const Rap::Suggest[])
					{
						{ Rap::ToSubstep(13, 2, 0), Rap::Button::Square }
					}
				},
			}
		};

		// Entry point
		void Main()
		{
			// Read INT
			Int::Int int_compo;
			int_compo.Read(&g_scenes[g_scene].compo.file);

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
		}
	}
}
