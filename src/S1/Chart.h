static constexpr Rap::Chart chart = {
	// Chart info
	Rap::ToSubstep(0, 3, 0.0),
	Rap::GetTimer(110.0),

	// Chart events
	Event_Stepped,

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
			},
			{
				{
					1,
					(const Rap::TapEvent[])
					{
						{ Event_PaRap, (void*)&parap_kick }
					}
				},
				{},
				{},
				{},
				{},
				{}
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
			},
			{
				{},
				{
					1,
					(const Rap::TapEvent[])
					{
						{ Event_PaRap, (void*)&parap_punch }
					}
				},
				{},
				{},
				{},
				{}
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
			},
			{
				{},
				{},
				{
					1,
					(const Rap::TapEvent[])
					{
						{ Event_PaRap, (void*)&parap_chop }
					}
				},
				{},
				{},
				{}
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
			},
			{
				{},
				{},
				{},
				{
					1,
					(const Rap::TapEvent[])
					{
						{ Event_PaRap, (void*)&parap_block }
					}
				},
				{},
				{}
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
			},
			{
				{
					1,
					(const Rap::TapEvent[])
					{
						{ Event_PaRap, (void*)&parap_kick }
					}
				},
				{},
				{},
				{},
				{},
				{}
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
			},
			{
				{},
				{
					1,
					(const Rap::TapEvent[])
					{
						{ Event_PaRap, (void*)&parap_punch }
					}
				},
				{},
				{},
				{},
				{}
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
			},
			{
				{},
				{},
				{
					1,
					(const Rap::TapEvent[])
					{
						{ Event_PaRap, (void*)&parap_chop }
					}
				},
				{},
				{},
				{}
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
			},
			{
				{},
				{},
				{},
				{
					1,
					(const Rap::TapEvent[])
					{
						{ Event_PaRap, (void*)&parap_block }
					}
				},
				{},
				{}
			}
		},
	}
};
