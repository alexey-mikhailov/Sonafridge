// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "Sonafridge/SonafridgeCommon.h"

namespace Audio
{
	class FBiquad;
}

namespace Sonafridge
{
	// Biquad filter class which wraps a biquad filter struct
	// Handles multi-channel audio to avoid calculating filter coefficients for multiple channels of audio.
	class SONAFRIDGE_API FBiquadFilter
	{
	public:
		// Constructor
		FBiquadFilter();
		// Destructor
		virtual ~FBiquadFilter();

		// Initialize the filter
		void Init(const float       InSampleRate,
		          const int32       InNumChannels,
		          const float       InOutputDb,
		          const EEQBandType InType,
		          const float       InGainDb,
		          const float       InCutoffFrequency = 20000.0f,
		          const float       InQuality = 1.0f);

		// Returns number of channels initialized with
		int32 GetNumChannels() const;

		// Resets the filter state
		void Reset();

		// Processes a single frame of audio
		void ProcessAudioFrame(const float* InFrame, float* OutFrame);

		// Process a buffer of audio
		void ProcessAudio(const float* InBuffer, const int32 InNumSamples, float* OutBuffer);

		// Sets all filter parameters with one function
		void SetParams(const float       InOutputDB,
		               const EEQBandType InFilterType,
		               const float       InGainDB,
		               const float       InCutoffFrequency,
		               const float       InQuality);

		// Sets the type of the filter to use
		void SetType(const EEQBandType InType);

		// Sets the filter frequency
		void SetFrequency(const float InCutoffFrequency);

		// Sets the quality of the filter
		void SetQuality(const float InQuality);

		// Sets the gain of the filter in decibels
		void SetGainDB(const float InGainDB);

		// Sets whether or no this filter is enabled (if disabled audio is passed through)
		void SetEnabled(const bool bInEnabled);

	protected:

		// Function computes biquad coefficients based on current filter settings
		void CalculateBiquadCoefficients();

		// Function used to clamp the cutoff frequency.
		float ClampCutoffFrequency(float InCutoffFrequency);

		// The sample rate of the filter
		float SampleRate;

		// Number of channels in the filter
		int32 NumChannels;

		// Whether or not this filter is enabled (will bypass otherwise)
		bool bEnabled;

		// The output of the filter in DB
		float OutputFactor;

		// What kind of filter to use when computing coefficients
		EEQBandType FilterType;

		// The gain of the filter in DB (for filters that use gain)
		float GainDB;

		// Current frequency of the filter
		float Frequency;

		// Current quality (anti-bandwidth, resonance) of the filter
		float Quality;

		// Biquad filter objects for each channel
		Audio::FBiquad* Biquad;
	};
}

