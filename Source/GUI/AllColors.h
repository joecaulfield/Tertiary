/*
  ==============================================================================

    AllColors.h
    Created: 16 Jan 2022 2:29:59pm
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#define USE_LIVE_CONSTANT true

# if USE_LIVE_CONSTANT
# define colorHelper(c) JUCE_LIVE_CONSTANT(c);
# else
# define colorHelper(c) c;
# endif

namespace AllColors
{
	using namespace juce;

	// Color-Scheme for All Universally Band-Color-Coded Items
	namespace BandColors
	{

		// Brightness-Steps For Gradient
		inline float gStep1 = 0.3f;
		inline float gStep2 = 0.5f;
		inline float gStep3 = 0.75f;
		inline float gStep4 = 0.9f;

		// Gradient Color-Scheme For "Low" Color-Coding
		inline Colour GRADIENT_LOW_1()		{ return	Colour(0xff1fcd1f); }
		inline Colour GRADIENT_LOW_2()		{ return	GRADIENT_LOW_1().withMultipliedBrightness(gStep1);  }
		inline Colour GRADIENT_LOW_3()		{ return	GRADIENT_LOW_1().withMultipliedBrightness(gStep2);  }
		inline Colour GRADIENT_LOW_4()		{ return	GRADIENT_LOW_1().withMultipliedBrightness(gStep3);	}
		inline Colour REGION_BORDER_LOW()	{ return	GRADIENT_LOW_1().withMultipliedBrightness(gStep4);  }

		// Gradient Color-Scheme For "Mid" Color-Coding ========
		inline Colour GRADIENT_MID_1()		{ return	Colour(0xffc420c4); }
		inline Colour GRADIENT_MID_2()		{ return	GRADIENT_MID_1().withMultipliedBrightness(gStep1);  }
		inline Colour GRADIENT_MID_3()		{ return	GRADIENT_MID_1().withMultipliedBrightness(gStep2);  }
		inline Colour GRADIENT_MID_4()		{ return	GRADIENT_MID_1().withMultipliedBrightness(gStep3);	}
		inline Colour REGION_BORDER_MID()	{ return	GRADIENT_MID_1().withMultipliedBrightness(gStep4);  }

		// Gradient Color-Scheme For "High" Color-Coding ========
		inline Colour GRADIENT_HIGH_1()		{ return	Colour(0xff4871f4); }
		inline Colour GRADIENT_HIGH_2()		{ return	GRADIENT_HIGH_1().withMultipliedBrightness(gStep1); }
		inline Colour GRADIENT_HIGH_3()		{ return	GRADIENT_HIGH_1().withMultipliedBrightness(gStep2); }
		inline Colour GRADIENT_HIGH_4()		{ return	GRADIENT_HIGH_1().withMultipliedBrightness(gStep3); }
		inline Colour REGION_BORDER_HIGH()	{ return	GRADIENT_HIGH_1().withMultipliedBrightness(gStep4); }

		// Gradient Color-Scheme For "Bypass" Color-Coding ========
		inline Colour GRADIENT_BYPASS_1()		{ return	Colour(0xff808080); }
		inline Colour GRADIENT_BYPASS_2()		{ return	GRADIENT_BYPASS_1().withMultipliedBrightness(gStep1); }
		inline Colour GRADIENT_BYPASS_3()		{ return	GRADIENT_BYPASS_1().withMultipliedBrightness(gStep2); }
		inline Colour GRADIENT_BYPASS_4()		{ return	GRADIENT_BYPASS_1().withMultipliedBrightness(gStep3); }
		inline Colour REGION_BORDER_BYPASS()	{ return	GRADIENT_BYPASS_1().withMultipliedBrightness(gStep4); }

	}


	namespace SliderColors
	{
		// 0 To 100 Sliders =================================================================

		// LOW
		inline ColourGradient BASIC_SLIDER_GRADIENT_LOW(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_LOW_1(),
											bounds.getBottomLeft(),
											BandColors::GRADIENT_LOW_2(),
											bounds.getTopRight(), false);

			gradient.addColour(p1, BandColors::GRADIENT_LOW_3().withBrightness(1.25f));
			gradient.addColour(p2, BandColors::GRADIENT_LOW_4().withBrightness(1.25f));

			return gradient;
		}

		// MID
		inline ColourGradient BASIC_SLIDER_GRADIENT_MID(Rectangle<float> bounds)
		{
			float p1 = 0.33f;
			float p2 = 0.66;

			auto gradient = ColourGradient(	BandColors::GRADIENT_MID_1(),
											bounds.getCentreX(), bounds.getBottom(),
											BandColors::GRADIENT_MID_2(),
											bounds.getCentreX(), bounds.getY(), false);

			gradient.addColour(p1, BandColors::GRADIENT_MID_3().withBrightness(1.25f));
			gradient.addColour(p2, BandColors::GRADIENT_MID_4().withBrightness(1.25f));

			return gradient;
		}

		// HIGH
		inline ColourGradient BASIC_SLIDER_GRADIENT_HIGH(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_HIGH_1(),
											bounds.getBottomRight(),
											BandColors::GRADIENT_HIGH_2(),
											bounds.getTopLeft(), false);

			gradient.addColour(p1, BandColors::GRADIENT_HIGH_3().withBrightness(1.25f));
			gradient.addColour(p2, BandColors::GRADIENT_HIGH_4().withBrightness(1.25f));

			return gradient;
		}

		// BYPASS
		inline ColourGradient BASIC_SLIDER_GRADIENT_BYPASS(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_BYPASS_3(),
											bounds.getBottomRight(),
											BandColors::GRADIENT_BYPASS_4(),
											bounds.getTopLeft(), false);

			gradient.addColour(p1, BandColors::GRADIENT_BYPASS_1());
			gradient.addColour(p2, BandColors::GRADIENT_BYPASS_2());

			return gradient;
		}

		// Center-Based Sliders =================================================================

		// LOW
		inline ColourGradient CENTER_SLIDER_GRADIENT_LOW(Rectangle<float> bounds)
		{
			float p1 = 0.2f;
			float p2 = 0.5f;
			float p3 = 0.8f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_LOW_1(),
											bounds.getTopLeft(),
											BandColors::GRADIENT_LOW_1(),
											bounds.getBottomRight(), false);

			gradient.addColour(p1, BandColors::GRADIENT_LOW_4());
			gradient.addColour(p2, BandColors::GRADIENT_LOW_3());
			gradient.addColour(p3, BandColors::GRADIENT_LOW_4());

			return gradient;
		}

		// MID
		inline ColourGradient CENTER_SLIDER_GRADIENT_MID(Rectangle<float> bounds)
		{
			float p1 = 0.2f;
			float p2 = 0.5f;
			float p3 = 0.8f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_MID_1(),
											bounds.getTopLeft(),
											BandColors::GRADIENT_MID_1(),
											bounds.getBottomRight(), false);

			gradient.addColour(p1, BandColors::GRADIENT_MID_4());
			gradient.addColour(p2, BandColors::GRADIENT_MID_3());
			gradient.addColour(p3, BandColors::GRADIENT_MID_4());

			return gradient;
		}

		// HIGH
		inline ColourGradient CENTER_SLIDER_GRADIENT_HIGH(Rectangle<float> bounds)
		{
			float p1 = 0.2f;
			float p2 = 0.5f;
			float p3 = 0.8f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_HIGH_1(),
											bounds.getTopLeft(),
											BandColors::GRADIENT_HIGH_1(),
											bounds.getBottomRight(), false);

			gradient.addColour(p1, BandColors::GRADIENT_HIGH_4());
			gradient.addColour(p2, BandColors::GRADIENT_HIGH_3());
			gradient.addColour(p3, BandColors::GRADIENT_HIGH_4());

			return gradient;
		}

		// BYPASS
		inline ColourGradient CENTER_SLIDER_GRADIENT_BYPASS(Rectangle<float> bounds)
		{
			float p1 = 0.2f;
			float p2 = 0.5f;
			float p3 = 0.8f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_BYPASS_1(),
											bounds.getTopLeft(),
											BandColors::GRADIENT_BYPASS_1(),
											bounds.getBottomRight(), false);

			gradient.addColour(p1, BandColors::GRADIENT_BYPASS_4());
			gradient.addColour(p2, BandColors::GRADIENT_BYPASS_3());
			gradient.addColour(p3, BandColors::GRADIENT_BYPASS_4());

			return gradient;
		}

		// Option-Select Sliders =================================================================

		// LOW
		inline ColourGradient OPTION_SLIDER_GRADIENT_LOW(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.25f;
			float p3 = 0.75f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_LOW_1(),
											bounds.getBottomLeft(),
											BandColors::GRADIENT_LOW_1(),
											bounds.getTopRight(), true);

			gradient.addColour(p1, BandColors::GRADIENT_LOW_4());
			gradient.addColour(p2, BandColors::GRADIENT_LOW_3());
			gradient.addColour(p1, BandColors::GRADIENT_LOW_4());

			return gradient;
		}

		// MID
		inline ColourGradient OPTION_SLIDER_GRADIENT_MID(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			// Not A Typo (Gradient 1 Used Twice)
			auto gradient = ColourGradient(	BandColors::GRADIENT_MID_1().withMultipliedBrightness(1.5f),
											bounds.getCentreX(), bounds.getBottom(),
											BandColors::GRADIENT_MID_1().withMultipliedBrightness(1.5f),
											bounds.getCentreX(), bounds.getY(), true);

			gradient.addColour(p1, BandColors::GRADIENT_MID_3().withMultipliedBrightness(1.5f));
			gradient.addColour(p2, BandColors::GRADIENT_MID_4().withMultipliedBrightness(1.5f));

			return gradient;
		}

		// HIGH
		inline ColourGradient OPTION_SLIDER_GRADIENT_HIGH(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			// Not A Typo (Gradient 1 Used Twice)
			auto gradient = ColourGradient(	BandColors::GRADIENT_HIGH_1().withMultipliedBrightness(1.5f),
											bounds.getBottomRight(),
											BandColors::GRADIENT_HIGH_1().withMultipliedBrightness(1.5f),
											bounds.getTopLeft(), true);

			gradient.addColour(p1, BandColors::GRADIENT_HIGH_3().withMultipliedBrightness(1.5f));
			gradient.addColour(p2, BandColors::GRADIENT_HIGH_4().withMultipliedBrightness(1.5f));

			return gradient;
		}

		// BYPASS
		inline ColourGradient OPTION_SLIDER_GRADIENT_BYPASS(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			// Not A Typo (Gradient 1 Used Twice)
			auto gradient = ColourGradient(	BandColors::GRADIENT_BYPASS_1().withMultipliedBrightness(1.5f),
											bounds.getBottomRight(),
											BandColors::GRADIENT_BYPASS_1().withMultipliedBrightness(1.5f),
											bounds.getTopLeft(), true);

			gradient.addColour(p1, BandColors::GRADIENT_BYPASS_3().withMultipliedBrightness(1.5f));
			gradient.addColour(p2, BandColors::GRADIENT_BYPASS_4().withMultipliedBrightness(1.5f));

			return gradient;
		}

	}


	namespace PluginEditorColors
	{
		// GRADIENT ITEMS FOR MAIN BACKGROUND ==============================================================
		inline Colour GRADIENT1() { return Colour(0xff000000); }
		inline Colour GRADIENT2() { return Colour(0xff000000); }
		inline Colour GRADIENT3() { return Colour(0xff221313); }
		inline Colour GRADIENT4() { return Colour(0xd71e2038); }

		// BACKGROUND GRADIENT ==========================================================
		inline ColourGradient BACKGROUND_GRADIENT(juce::Rectangle<float> bounds)
		{
			float p1 = 0.5f;
			float p2 = 0.75f;

			auto gradient = juce::ColourGradient(	GRADIENT1(),
													bounds.getTopLeft(),
													GRADIENT2(),
													bounds.getBottomRight(), true);

			gradient.addColour(p1, GRADIENT3());
			gradient.addColour(p2, GRADIENT4());

			return gradient;
		}
	};

	namespace GlobalControlsColors
	{

		// Brightness-Steps For Gradient
		inline float gStep1 = 0.3f;
		inline float gStep2 = 0.5f;
		inline float gStep3 = 0.75f;
		inline float gStep4 = 0.9f;

		inline Colour BASE_COLOR() { return Colour(0xff03010d); }
		//inline Colour BASE_COLOR() { return colorHelper(Colours::red); }

		// BACKGROUND GRADIENT ==========================================================
		inline ColourGradient BACKGROUND_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	BASE_COLOR(),
											bounds.getCentre(),
											BASE_COLOR().withMultipliedAlpha(gStep1),
											bounds.getTopLeft(), true);

			gradient.addColour( p1, BASE_COLOR().withMultipliedAlpha(gStep3));
			gradient.addColour( p2, BASE_COLOR().withMultipliedAlpha(gStep4));

			return gradient;
		}
	};


	namespace WaveControlsColors
	{
		// Brightness-Steps For Gradient
		inline float gStep1 = 0.3f;
		inline float gStep2 = 0.5f;
		inline float gStep3 = 0.75f;
		inline float gStep4 = 0.9f;

		inline Colour BASE_COLOR() { return Colour(0xff20021d); }
		//inline Colour BASE_COLOR() { return colorHelper(Colours::red); }

		// BACKGROUND GRADIENT ==========================================================
		inline ColourGradient BACKGROUND_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	BASE_COLOR(),
											bounds.getCentre(),
											BASE_COLOR().withMultipliedAlpha(gStep1),
											bounds.getTopLeft(), true);

			gradient.addColour( p1, BASE_COLOR().withMultipliedAlpha(gStep3));
			gradient.addColour( p2, BASE_COLOR().withMultipliedAlpha(gStep4));

			return gradient;
		}

		// WINDOW BACKGROUND OPACITY ===========================================================================
		inline float BACKGROUND_OPACITY() { return 1.f; }

		// WINDOW BORDER COLOR =======================================================================
		inline Colour BORDER_OUTLINE_COLOR() { return Colour(0x00000000); }		// Border Color

		// WINDOW BORDER THICKNESS ====================================================================
		inline float BORDER_OUTLINE_THICKNESS() { return 3.f; }

		inline Colour TEXT_BACKGROUND_FILL() {return Colour(0xffffffff); }	// Text Background Fill
};


	namespace TimingControlsColors
	{
		// WINDOW BACKGROUND GRADIENT COLORS ==============================================================
			//inline Colour GRADIENT1() { return colorHelper(Colours::red); }
			//inline Colour GRADIENT2() { return colorHelper(Colours::green); }
			//inline Colour GRADIENT3() { return colorHelper(Colours::blue); }
			//inline Colour GRADIENT4() { return colorHelper(Colours::yellow); }

		inline Colour GRADIENT1() { return Colour(0xff4f544f); }
		inline Colour GRADIENT2() { return Colour(0xffd9d6d6); }
		inline Colour GRADIENT3() { return Colour(0xff919990); }
		inline Colour GRADIENT4() { return Colour(0xff95958c); }

		// WINDOW BACKGROUND GRADIENT ==========================================================
		inline ColourGradient BACKGROUND_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.3f;
			float p2 = 0.7f;

			auto gradient = ColourGradient(	GRADIENT1(),
											bounds.getTopLeft(),
											GRADIENT2(),
											bounds.getBottomRight(), false);

			gradient.addColour(p1, GRADIENT3());
			gradient.addColour(p2, GRADIENT4());

			return gradient;
		}

		// WINDOW BACKGROUND OPACITY ===========================================================================
		inline float BACKGROUND_OPACITY() { return 0.75f; }

		// WINDOW BORDER COLOR =======================================================================
		inline Colour BORDER_OUTLINE_COLOR() { return Colour(0x00000000); }		// Border Color

		// WINDOW BORDER THICKNESS ====================================================================
		inline float BORDER_OUTLINE_THICKNESS() { return 3.f; }

		inline Colour TEXT_BACKGROUND_FILL() {return Colour(0xffffffff); }	// Text Background Fill
	};


	namespace GainControlsColors
	{
		// WINDOW BACKGROUND GRADIENT COLORS ==============================================================
			//inline Colour GRADIENT1() { return colorHelper(Colours::red); }
			//inline Colour GRADIENT2() { return colorHelper(Colours::green); }
			//inline Colour GRADIENT3() { return colorHelper(Colours::blue); }
			//inline Colour GRADIENT4() { return colorHelper(Colours::yellow); }

		inline Colour GRADIENT1() { return Colour(0xff4f544f); }
		inline Colour GRADIENT2() { return Colour(0xffd9d6d6); }
		inline Colour GRADIENT3() { return Colour(0xff919990); }
		inline Colour GRADIENT4() { return Colour(0xff95958c); }

		// WINDOW BACKGROUND GRADIENT ==========================================================
		inline ColourGradient BACKGROUND_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.3f;
			float p2 = 0.7f;

			auto gradient = ColourGradient(	GRADIENT1(),
											bounds.getTopLeft(),
											GRADIENT2(),
											bounds.getBottomRight(), false);

			gradient.addColour(p1, GRADIENT3());
			gradient.addColour(p2, GRADIENT4());

			return gradient;
		}

		// WINDOW BACKGROUND OPACITY ===========================================================================
		inline float BACKGROUND_OPACITY() { return 0.75f; }

		// WINDOW BORDER COLOR =======================================================================
		inline Colour BORDER_OUTLINE_COLOR() { return Colour(0x00000000); }		// Border Color

		// WINDOW BORDER THICKNESS ====================================================================
		inline float BORDER_OUTLINE_THICKNESS() { return 3.f; }

		inline Colour TEXT_BACKGROUND_FILL() { return Colour(0xffffffff); }	// Text Background Fill
	};


	namespace CrossoverControlsColors
	{
		// BACKGROUND GRADIENT COLORS
		inline Colour GRADIENT1() { return Colour(0xff4f544f); }
		inline Colour GRADIENT2() { return Colour(0xffd9d6d6); }
		inline Colour GRADIENT3() { return Colour(0xff919990); }
		inline Colour GRADIENT4() { return Colour(0xff95958c); }

		// WINDOW BACKGROUND GRADIENT ==========================================================
		inline ColourGradient BACKGROUND_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	GRADIENT1(),
											bounds.getTopLeft(),
											GRADIENT2(),
											bounds.getBottomRight(), false);

			gradient.addColour(p1, GRADIENT3());
			gradient.addColour(p2, GRADIENT4());

			return gradient;
		}

		// WINDOW BACKGROUND OPACITY ===========================================================================
		inline float BACKGROUND_OPACITY() { return 0.75f; }

		// WINDOW BORDER COLOR =======================================================================
		inline Colour BORDER_OUTLINE_COLOR() { return Colour(0x00000000); }		// Border Color

		// WINDOW BORDER THICKNESS ====================================================================
		inline float BORDER_OUTLINE_THICKNESS() { return 3.f; }

		inline Colour TEXT_BACKGROUND_FILL() { return Colour(0xffffffff); }	// Text Background Fill
	};


	namespace InputOutputMeterColors
	{
		//// GRILL BACKGROUND GRADIENT ==========================================================
		//inline Colour GRADIENT1() { return juce::Colour(0xff353131); }
		//inline Colour GRADIENT2() { return juce::Colour(0xff474141); }
		//inline Colour GRADIENT3() { return juce::Colour(0xff92929a); }
		//inline Colour GRADIENT4() { return juce::Colour(0xff51514e); }

		//inline ColourGradient GRILL_GRADIENT(Rectangle<float> bounds)
		//{
		//	float p1 = 0.25f;
		//	float p2 = 0.75f;

		//	auto gradient =	ColourGradient(GRADIENT1(),
		//					bounds.getTopLeft(),
		//					GRADIENT2(),
		//					bounds.getBottomRight(), true);

		//	gradient.addColour(p1, GRADIENT3());
		//	gradient.addColour(p2, GRADIENT4());

		//	return gradient;
		//}
	};


	namespace OscilloscopeColors
	{
		// BACKGROUND FILL ==========================================================

		// Brightness-Steps For Gradient
		inline float gStep1 = 0.3f;
		inline float gStep2 = 0.5f;
		inline float gStep3 = 0.75f;
		inline float gStep4 = 0.9f;

		inline Colour BASE_COLOR() { return Colour(0xff181212); }


		inline ColourGradient BACKGROUND_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	BASE_COLOR(),
											bounds.getCentre(),
											BASE_COLOR().withMultipliedAlpha(gStep1),
											bounds.getTopLeft(), false);

			gradient.addColour( p1, BASE_COLOR().withMultipliedAlpha(gStep3));
			gradient.addColour( p2, BASE_COLOR().withMultipliedAlpha(gStep4));

			return gradient;
		}

		// SCROLL SLIDER GRADIENT
		//inline Colour GRADIENT_SCROLL_1() { return juce::Colour(0xff201f1f); }
		//inline Colour GRADIENT_SCROLL_2() { return juce::Colour(0xff393333); }
		//inline Colour GRADIENT_SCROLL_3() { return juce::Colour(0xffa0a0a9); }
		//inline Colour GRADIENT_SCROLL_4() { return juce::Colour(0xffffffff); }
		//inline Colour GRADIENT_SCROLL_5() { return juce::Colour(0xff9f7b8d); }

		//inline ColourGradient SCROLL_GRADIENT(Rectangle<float> bounds, float scrollRange)
		//{
		//	float p1 = 0.25f;
		//	float p2 = 0.75f;

		//	scrollRange = juce::jmap(scrollRange, 0.1f, 0.9f);

		//	auto gradient =	ColourGradient(GRADIENT_SCROLL_1(),
		//					bounds.getTopLeft(),
		//					GRADIENT_SCROLL_2(),
		//					bounds.getBottomRight(), false);

		//	gradient.addColour(scrollRange/2.f, GRADIENT_SCROLL_3());
		//	gradient.addColour(scrollRange, GRADIENT_SCROLL_4());
		//	gradient.addColour((1.f+scrollRange)/2.f, GRADIENT_SCROLL_5());
		//	

		//	return gradient;
		//}

		// LOW WAVEFORM GRADIENT ===================================================================================
		inline ColourGradient WAVEFORM_LOW_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_LOW_1(),
											bounds.getBottomLeft(),
											BandColors::GRADIENT_LOW_2(),
											bounds.getTopRight(), true);

			gradient.addColour(p1, BandColors::GRADIENT_LOW_3());
			gradient.addColour(p2, BandColors::GRADIENT_LOW_4());

			return gradient;
		}

		// MID WAVEFORM GRADIENT  ==========================================================
		inline ColourGradient WAVEFORM_MID_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_MID_1(),
											bounds.getBottomLeft(),
											BandColors::GRADIENT_MID_2(),
											bounds.getTopRight(), true);

			gradient.addColour(p1, BandColors::GRADIENT_MID_3());
			gradient.addColour(p2, BandColors::GRADIENT_MID_4());

			return gradient;
		}

		// HIGH WAVEFORM GRADIENT  ==========================================================
		inline ColourGradient WAVEFORM_HIGH_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_HIGH_1(),
											bounds.getBottomRight(),
											BandColors::GRADIENT_HIGH_2(),
											bounds.getTopLeft(), true);

			gradient.addColour(p1, BandColors::GRADIENT_HIGH_3());
			gradient.addColour(p2, BandColors::GRADIENT_HIGH_4());

			return gradient;
		}

		// BYPASS REGION GRADIENT  ========================================================
		inline ColourGradient WAVEFORM_BYPASS_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient( BandColors::GRADIENT_BYPASS_1(),
											bounds.getBottomLeft(),
											BandColors::GRADIENT_BYPASS_2(),
											bounds.getTopRight(), true);

			gradient.addColour(p1, BandColors::GRADIENT_BYPASS_3());
			gradient.addColour(p2, BandColors::GRADIENT_BYPASS_4());

			return gradient;
		}

		// REGION BORDERS  ==========================================================
		inline Colour REGION_BORDER_COLOR_LOW() { return BandColors::REGION_BORDER_LOW(); }
		inline Colour REGION_BORDER_COLOR_MID() { return BandColors::REGION_BORDER_MID(); }
		inline Colour REGION_BORDER_COLOR_HIGH() { return BandColors::REGION_BORDER_HIGH(); }
		inline Colour REGION_BORDER_COLOR_BYPASS() { return BandColors::REGION_BORDER_BYPASS(); }

		// OUTER BORDER COLOR =======================================================================================
	/*	inline Colour BORDER_COLOR() { return juce::Colour(0xffffffff); }*/

		// AXIS =======================================================================================
		//const Colour GRID_LINES_LOW		(169, 169, 169);	// Grid - Low
		//const Colour GRID_LINES_MID		(169, 169, 169);	// Grid - Mid
		//const Colour GRID_LINES_HIGH	(169, 169, 169);	// Grid - High
		//const Colour GRID_LINES_NONE	(169, 169, 169);	// Grid - None

		// CURSOR =====================================================================================
		const Colour CURSOR_LINE (255, 255, 255);	// Cursor



		// BUTTONS ===================================================================================
		const Colour BUTTON_BACKGROUND_FILL (255, 255, 255);
		const Colour BUTTON_TEXT_COLOR(0, 0, 0);
	};


	namespace FrequencyResponseColors
	{
		// LOW REGION GRADIENT  ==========================================================
		inline ColourGradient LOW_REGION_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient( BandColors::GRADIENT_LOW_1(),
											bounds.getBottomLeft(),
											BandColors::GRADIENT_LOW_2(),
											bounds.getTopRight(), true);

			gradient.addColour(p1, BandColors::GRADIENT_LOW_3());
			gradient.addColour(p2, BandColors::GRADIENT_LOW_4());

			return gradient;
		}

		// MID REGION GRADIENT  ==========================================================
		inline ColourGradient MID_REGION_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.33f;
			float p2 = 0.66;

			auto gradient = ColourGradient(	BandColors::GRADIENT_MID_1(),
											bounds.getCentreX(), bounds.getBottom(),
											BandColors::GRADIENT_MID_2(),
											bounds.getCentreX(), bounds.getY(), true);

			gradient.addColour(p1, BandColors::GRADIENT_MID_3());
			gradient.addColour(p2, BandColors::GRADIENT_MID_4());

			return gradient;
		}

		// HIGH REGION GRADIENT  ==========================================================
		inline ColourGradient HIGH_REGION_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	BandColors::GRADIENT_HIGH_1(),
											bounds.getBottomRight(),
											BandColors::GRADIENT_HIGH_2(),
											bounds.getTopLeft(), true);

			gradient.addColour(p1, BandColors::GRADIENT_HIGH_3());
			gradient.addColour(p2, BandColors::GRADIENT_HIGH_4());

			return gradient;
		}

		// BYPASS REGION GRADIENT  ========================================================
		inline ColourGradient BYPASS_REGION_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient( BandColors::GRADIENT_BYPASS_1(),
											bounds.getBottomLeft(),
											BandColors::GRADIENT_BYPASS_2(),
											bounds.getTopRight(), true);

			gradient.addColour(p1, BandColors::GRADIENT_BYPASS_3());
			gradient.addColour(p2, BandColors::GRADIENT_BYPASS_4());

			return gradient;
		}

		// REGION BORDERS  ==========================================================
		inline Colour REGION_BORDER_COLOR_LOW() { return BandColors::REGION_BORDER_LOW(); }
		inline Colour REGION_BORDER_COLOR_MID() { return BandColors::REGION_BORDER_MID(); }
		inline Colour REGION_BORDER_COLOR_HIGH() { return BandColors::REGION_BORDER_HIGH(); }
		inline Colour REGION_BORDER_COLOR_BYPASS() { return BandColors::REGION_BORDER_BYPASS(); }

		// BACKGROUND FILL ==========================================================

		// Brightness-Steps For Gradient
		inline float gStep1 = 0.3f;
		inline float gStep2 = 0.5f;
		inline float gStep3 = 0.75f;
		inline float gStep4 = 0.9f;

		inline Colour BASE_COLOR() { return Colour(0xff03010d); }

		inline ColourGradient BACKGROUND_GRADIENT(Rectangle<float> bounds)
		{
			float p1 = 0.25f;
			float p2 = 0.5f;

			auto gradient = ColourGradient(	BASE_COLOR(),
											bounds.getCentre(),
											BASE_COLOR().withMultipliedAlpha(gStep1),
											bounds.getTopLeft(), true);

			gradient.addColour( p1, BASE_COLOR().withMultipliedAlpha(gStep3));
			gradient.addColour( p2, BASE_COLOR().withMultipliedAlpha(gStep4));

			return gradient;
		}

	};
}