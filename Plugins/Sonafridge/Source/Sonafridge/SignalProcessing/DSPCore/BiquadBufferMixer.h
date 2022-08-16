// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "Sonafridge/SonafridgeCommon.h"

namespace Sonafridge
{
	class FBiquad;

	enum class EBiquadMixerMode
	{
		/// M:M No blending. 
		NoBlending,
		/// 1:M Broadcast mono. 
		BroadcastMono,
		/// 2:M Broadcast downmixed mono from stereo. 
		BroadcastMonoFromStereo,
		/// N:M Broadcast downmixed mono from N channels. 
		BroadcastMonoFromMultichannel,
	};

	/// <summary>
	/// Biquad buffer mixer is biquad filter which calculates biquad result
	/// depending on number of input and output channels and
	/// depending on how far we are from previous buffer.
	///	It may be useful when we have to blend results from buffer to buffer. 
	/// <br><br>
	///	Result will be casted to channel with certain DstChannelIndex. 
	/// </summary>
	class SONAFRIDGE_API FBiquadBufferMixer final
	{
	public:
		FBiquadBufferMixer();

		void Init(float InSampleRate);

		void Reset();
		void NotifyBufferChanged();

		void ProcessInterlaced(const float* SrcBuffer,
		                       float*       DstBuffer,
		                       int32        InNumSamples);

		void ProcessInterlaced(const float*     SrcBuffer,
		                       float*           DstBuffer,
		                       EBiquadMixerMode MixerMode,
		                       int32            InNumSamples,
		                       int32            InNumSrcChannels,
		                       int32            InNumDstChannels,
		                       int32            InDstChannelIndex);

		void SetParams(EEQBandType InType,
		               float       InFrequency,
		               float       InAmountDb,
		               float       InQuality,
		               float       InMakeupDb);

		void SetDoFilter(bool bInDoFilter);
		void SetType(EEQBandType InType);
		void SetFrequency(float InCutoffFrequency);
		void SetAmountDb(float InAmountDb);
		void SetQuality(float InQuality);
		void SetMakeupDb(float InMakeupDb);

		EBiquadMixerMode GetMixerMode() const { return Mode; }

		void SetMode(EBiquadMixerMode InMode) { Mode = InMode; }
		void SetNumSrcChannels(int32 InValue) { NumSrcChannels = InValue; }
		void SetNumDstChannels(int32 InValue) { NumDstChannels = InValue; }
		void SetDstChannelIndex(int32 InValue) { DstChannelIndex = InValue; }

	protected:
		void  CalculateBiquadCoefficients();

		float SampleRate = 0.f;

		EBiquadMixerMode Mode = EBiquadMixerMode::NoBlending;

		bool        bDoFilter = true;
		EEQBandType FilterType = EEQBandType::AttBand;
		float       Frequency = 0.f;
		float       AmountDb = 0.f;
		float       Quality = 0.f;
		float       MakeupCoeffOld = 0.f;
		float       MakeupCoeffNew = 0.f;

		int32 NumSrcChannels = 0;
		int32 NumDstChannels = 0;
		int32 DstChannelIndex = 0;

		TSharedPtr<FBiquad> BiquadNew;
		TSharedPtr<FBiquad> BiquadOld;
	};
}

