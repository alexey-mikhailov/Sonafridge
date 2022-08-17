// Sonafridge 2022

#pragma once
#include "CoreMinimal.h"

namespace Sonafridge
{
	class SONAFRIDGE_API FDelayBufferMixer final
	{
	public:
		FDelayBufferMixer();

		void Init(const int32 InSampleRate, const float InBufferLengthSec = 2.0f);
		void Reset();

		void  SetDelaySeconds(const float InValue);
		void  SetDelayMilliseconds(const float InValue);
		float GetDelaySamples() const { return PostponeInSamplesNew; }

		/// Processes audio in the delay line, returns the delayed value
		float ProcessAudioSample(const float InAudio);

		/// Postpones the audio buffer for InChannelIndex.
		///	Number of input and output channels must be equal. 
		void ProcessInterlaced(const float* SrcBuffer,
		                       float*       DstBuffer,
		                       float        NumSamples,
		                       int32        NumChannels,
		                       int32        InChannelIndex);
		
		/// Reads the buffer at current read index without writing or incrementing read/write pointers.
		float Read() const;

		/// Writes the input and increment read/write pointers
		void WriteDelayAndInc(const float InDelayInput);

		void NotifyBufferChanged();

	protected:
		// Updates delay line based on any recent changes to settings
		void Update(bool bForce = false);

		int32 SampleRate = {};
		int32 AudioBufferSize = {};
		Audio::AlignedFloatBuffer AudioBuffer = {};
		int32 PostponeInSamplesNew = {};
		int32 PostponeInSamplesOld = {};
		int32 ReadIndex = {};
		int32 WriteIndex = {};
	};

}
