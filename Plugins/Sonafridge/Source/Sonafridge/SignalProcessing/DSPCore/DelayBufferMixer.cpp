// Sonafridge 2022

#include "DelayBufferMixer.h"

namespace Sonafridge
{
	FDelayBufferMixer::FDelayBufferMixer()
	{
		Reset();
	}

	void FDelayBufferMixer::Init(const int32 InSampleRate, const float InBufferLengthSec)
	{
		SampleRate = InSampleRate;
		AudioBufferSize = static_cast<int32>(InBufferLengthSec * InSampleRate);
		AudioBuffer.SetNumZeroed(AudioBufferSize);

		Reset();
	}

	void FDelayBufferMixer::Reset()
	{
		AudioBuffer.SetNumZeroed(AudioBufferSize);
		WriteIndex = 0;
		ReadIndex = 0;

		Update(true);
	}

	void FDelayBufferMixer::SetDelaySeconds(const float InValue)
	{
		const int32 NewDelayInSamples = static_cast<int32>(InValue * SampleRate);
		PostponeInSamplesNew = FMath::Clamp(NewDelayInSamples, 0, AudioBufferSize);
		Update(true);
	}

	void FDelayBufferMixer::SetDelayMilliseconds(const float InValue)
	{
		const int32 NewDelayInSamples = static_cast<int32>(.001f * InValue * SampleRate);
		PostponeInSamplesNew = FMath::Clamp(NewDelayInSamples, 0, AudioBufferSize);
		Update(true);
	}

	float FDelayBufferMixer::ProcessAudioSample(const float InAudio)
	{
		Update();

		const float Yn = PostponeInSamplesNew == 0 ? InAudio : Read();
		WriteDelayAndInc(InAudio);
		return Yn;
	}

	void FDelayBufferMixer::ProcessInterlaced(const float* SrcBuffer,
	                                          float*       DstBuffer,
	                                          float        NumSamples,
	                                          int32        NumChannels,
	                                          int32        InChannelIndex)
	{
		int32 SampleIndex = INDEX_NONE;
		int32 ChIndex = InChannelIndex;
		float BlendAlpha;

		while (++SampleIndex < NumSamples)
		{
			BlendAlpha = (float) SampleIndex / NumSamples;
			float PostponeInSamples = FMath::Lerp(PostponeInSamplesOld, PostponeInSamplesNew, BlendAlpha);

			// Update indices. 
			ReadIndex = WriteIndex - PostponeInSamples;
			if (ReadIndex < 0)
			{
				ReadIndex += AudioBufferSize;
			}

			// In case of both of buffers are equal, src will be overwritten.
			// So, cache it.
			static float Tmp;
			Tmp = SrcBuffer[ChIndex];

			DstBuffer[ChIndex] = PostponeInSamples == 0 ? SrcBuffer[ChIndex] : Read();
			WriteDelayAndInc(Tmp);
			ChIndex += NumChannels;
		}
	}

	float FDelayBufferMixer::Read() const
	{
		return AudioBuffer[ReadIndex];
	}

	void FDelayBufferMixer::WriteDelayAndInc(const float InDelayInput)
	{
		AudioBuffer[WriteIndex] = InDelayInput; 

		++WriteIndex;
		if (WriteIndex >= AudioBufferSize)
		{
			WriteIndex = 0;
		}

		++ReadIndex;
		if (ReadIndex >= AudioBufferSize)
		{
			ReadIndex = 0;
		}
	}

	void FDelayBufferMixer::NotifyBufferChanged()
	{
		PostponeInSamplesOld = PostponeInSamplesNew;
	}

	void FDelayBufferMixer::Update(bool bForce)
	{
		PostponeInSamplesNew = FMath::Clamp(PostponeInSamplesNew, 0, AudioBufferSize);

		ReadIndex = WriteIndex - PostponeInSamplesNew;
		if (ReadIndex < 0)
		{
			ReadIndex += AudioBufferSize;
		}
	}
}
