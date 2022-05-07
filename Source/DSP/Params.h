/*
  ==============================================================================

    Params.h
    Created: 3 Jan 2022 9:24:56pm
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

namespace Params    // Mapping between Parameter Names and Unique ID's
{
	enum Names      // List of ID's
	{
		Input_Gain,
		Output_Gain,

		Low_Mid_Crossover_Freq,
		Mid_High_Crossover_Freq,

		Bypass_Low_Band,
		Bypass_Mid_Band,
		Bypass_High_Band,

		Mute_Low_Band,
		Mute_Mid_Band,
		Mute_High_Band,

		Solo_Low_Band,
		Solo_Mid_Band,
		Solo_High_Band,

		Gain_Low_Band,
		Gain_Mid_Band,
		Gain_High_Band,

		Wave_Low_LFO,
		Depth_Low_LFO,
		Symmetry_Low_LFO,
		Invert_Low_LFO,
		Sync_Low_LFO,
		Rate_Low_LFO,
		Multiplier_Low_LFO,
		Relative_Phase_Low_LFO,

		Wave_Mid_LFO,
		Depth_Mid_LFO,
		Symmetry_Mid_LFO,
		Invert_Mid_LFO,
		Sync_Mid_LFO,
		Rate_Mid_LFO,
		Multiplier_Mid_LFO,
		Relative_Phase_Mid_LFO,

		Wave_High_LFO,
		Depth_High_LFO,
		Symmetry_High_LFO,
		Invert_High_LFO,
		Sync_High_LFO,
		Rate_High_LFO,
		Multiplier_High_LFO,
		Relative_Phase_High_LFO,

		Show_Low_Scope,
		Show_Mid_Scope,
		Show_High_Scope,
		Stack_Bands_Scope,
		Show_Cursor_Scope,
		Show_Playhead_Scope,

		Cursor_Position,

		Scope_Point1,
		Scope_Point2,

		Show_FFT,
		FFT_Pickoff
	};

	inline const std::map<Names, juce::String>& GetParams()
	{
		static std::map<Names, juce::String> params =   // List of Names
		{
			{Input_Gain,                "Input Gain"},
			{Output_Gain,               "Output Gain"},

			{Low_Mid_Crossover_Freq,    "Low-Mid Crossover Freq"},
			{Mid_High_Crossover_Freq,   "Mid-High Crossover Freq"},

			{Bypass_Low_Band,           "Bypassed Low Band" },
			{Bypass_Mid_Band,           "Bypassed Mid Band" },
			{Bypass_High_Band,          "Bypassed High Band" },

			{Mute_Low_Band,             "Mute Low Band" },
			{Mute_Mid_Band,             "Mute Mid Band" },
			{Mute_High_Band,            "Mute High Band" },

			{Solo_Low_Band,             "Solo Low Band" },
			{Solo_Mid_Band,             "Solo Mid Band" },
			{Solo_High_Band,            "Solo High Band" },

			{Gain_Low_Band,             "Low Output Gain"},
			{Gain_Mid_Band,             "Mid Output Gain"},
			{Gain_High_Band,            "High Output Gain"},

			{Wave_Low_LFO,              "Waveshape Low Band"},
			{Depth_Low_LFO,             "Depth Low Band"},
			{Symmetry_Low_LFO,          "Symmetry Low Band"},
			{Invert_Low_LFO,            "Invert Low Band"},
			{Sync_Low_LFO,              "Sync to Host Low Band"},
			{Rate_Low_LFO,              "Rate Low Band"},
			{Multiplier_Low_LFO,        "Multiplier Low Band"},
			{Relative_Phase_Low_LFO,    "Relative Phase Low Band"},

			{Wave_Mid_LFO,              "Waveshape Mid Band"},
			{Depth_Mid_LFO,             "Depth Mid Band"},
			{Symmetry_Mid_LFO,          "Symmetry Mid Band"},
			{Invert_Mid_LFO,            "Invert Mid Band"},
			{Sync_Mid_LFO,              "Sync to Host Mid Band"},
			{Rate_Mid_LFO,              "Rate Mid Band"},
			{Multiplier_Mid_LFO,        "Multiplier Mid Band"},
			{Relative_Phase_Mid_LFO,    "Relative Phase Mid Band"},

			{Wave_High_LFO,             "Waveshape High Band"},
			{Depth_High_LFO,            "Depth High Band"},
			{Symmetry_High_LFO,         "Symmetry High Band"},
			{Invert_High_LFO,           "Invert High Band"},
			{Sync_High_LFO,             "Sync to Host High Band"},
			{Rate_High_LFO,             "Rate High Band"},
			{Multiplier_High_LFO,       "Multiplier High Band"},
			{Relative_Phase_High_LFO,   "Relative Phase High Band"},

			{Show_Low_Scope,			"Show Low Band In Scope"},
			{Show_Mid_Scope,			"Show Mid Band In Scope"},
			{Show_High_Scope,			"Show High Band In Scope"},
			{Stack_Bands_Scope,			"Stack All Bands"},
			{Show_Cursor_Scope,			"Show Cursor In Scope"},
			{Show_Playhead_Scope,		"Show Playhead In Scope"},

			{Cursor_Position,			"Scope Cursor Position"},

			{Scope_Point1,				"Scope Point 1"},
			{Scope_Point2,				"Scope Point 2"},

			{Show_FFT,					"Show FFT"},
			{FFT_Pickoff,				"FFT Pickoff Point" },
		};

		return params;
	}
};

template<
    typename APVTS,
    typename Params,
    typename Name
        >

juce::RangedAudioParameter& getParam(APVTS& apvts, const Params& params, const Name& name)
{
    auto param = apvts.getParameter(params.at(name));
    jassert( param != nullptr );
    
    return *param;
}