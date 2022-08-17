// Sonafridge 2022

#include "BiquadBufferMixer.h"
#include "Biquad.h"
#include "Sonafridge/Tools/MathTools.h"

namespace Sonafridge
{
	FBiquadBufferMixer::FBiquadBufferMixer()
	{
		BiquadNew = MakeShared<FBiquad>();
		BiquadOld = MakeShared<FBiquad>();
	}

	void FBiquadBufferMixer::Init(float InSampleRate)
	{
		BiquadNew->Reset();
		BiquadOld->Reset();
		SampleRate = InSampleRate;
	}

	void FBiquadBufferMixer::ProcessInterlaced(const float* SrcBuffer,
	                                           float*       DstBuffer,
	                                           int32        InNumSamples)
	{
		if (bDoFilter)
		{
			// Mix them with applying effect.

			if (Mode == EBiquadMixerMode::NoBlending)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 SrcIndex = DstChannelIndex;
				int32 DstIndex = DstChannelIndex;
				float BlendAlpha;

				while (++SampleIndex < InNumSamples)
				{
					BlendAlpha = (float) SampleIndex / InNumSamples;
					DstBuffer[DstIndex] = FMath::Lerp(BiquadOld->ProcessAudio(SrcBuffer[SrcIndex]) * MakeupCoeffOld, 
													  BiquadNew->ProcessAudio(SrcBuffer[SrcIndex]) * MakeupCoeffNew, 
													  BlendAlpha);
					SrcIndex += NumSrcChannels;
					DstIndex += NumDstChannels;
				}
			}
			else if (Mode == EBiquadMixerMode::BroadcastMono)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 DstIndex = DstChannelIndex;
				float BlendAlpha;

				while (++SampleIndex < InNumSamples)
				{
					BlendAlpha = (float) SampleIndex / InNumSamples;
					float S1 = BiquadOld->ProcessAudio(SrcBuffer[SampleIndex]) * MakeupCoeffOld;
					float S2 = BiquadNew->ProcessAudio(SrcBuffer[SampleIndex]) * MakeupCoeffNew;
					DstBuffer[DstIndex] = FMath::Lerp(S1, S2, BlendAlpha);
					DstIndex += NumDstChannels;
				}
			}
			else if (Mode == EBiquadMixerMode::BroadcastMonoFromStereo)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 SrcIndex = INDEX_NONE;
				int32 DstIndex = DstChannelIndex;
				float MonoSample, BlendAlpha;

				while (++SampleIndex < InNumSamples)
				{
					MonoSample = .5f * (SrcBuffer[++SrcIndex] + SrcBuffer[++SrcIndex]);
					BlendAlpha = (float) SampleIndex / InNumSamples;
					DstBuffer[DstIndex] = FMath::Lerp(BiquadOld->ProcessAudio(MonoSample) * MakeupCoeffOld, 
													  BiquadNew->ProcessAudio(MonoSample) * MakeupCoeffNew, 
													  BlendAlpha);
					DstIndex += NumDstChannels;
				}
			}
			else if (Mode == EBiquadMixerMode::BroadcastMonoFromMultichannel)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 SrcIndex = INDEX_NONE;
				int32 DstIndex = DstChannelIndex;
				float MonoSample = 0.f, BlendAlpha;

				while (++SampleIndex < InNumSamples)
				{
					DstBuffer[DstIndex] = 0.f;
					for (int32 I = 0; I < NumSrcChannels; ++I)
					{
						MonoSample += SrcBuffer[++SrcIndex] / NumSrcChannels;
					}

					BlendAlpha = (float) SampleIndex / InNumSamples;
					DstBuffer[DstIndex] = FMath::Lerp(BiquadOld->ProcessAudio(MonoSample) * MakeupCoeffOld, 
													  BiquadNew->ProcessAudio(MonoSample) * MakeupCoeffNew, 
													  BlendAlpha);
					DstIndex += NumDstChannels;
				}
			}
		}
		else
		{
			// Mix them without applying effect.

			if (Mode == EBiquadMixerMode::NoBlending)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 SrcIndex = DstChannelIndex;
				int32 DstIndex = DstChannelIndex;
				while (++SampleIndex < InNumSamples)
				{
					DstBuffer[DstIndex] = SrcBuffer[SrcIndex];
					SrcIndex += NumSrcChannels;
					DstIndex += NumDstChannels;
				}
			}
			else if (Mode == EBiquadMixerMode::BroadcastMono)
			{
				int32 SrcIndex = 0;
				int32 DstIndex = DstChannelIndex;
				while (SrcIndex < InNumSamples)
				{
					DstBuffer[DstIndex] = SrcBuffer[SrcIndex];
					DstIndex += NumDstChannels;
					++SrcIndex;
				}
			}
			else if (Mode == EBiquadMixerMode::BroadcastMonoFromStereo)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 SrcIndex = INDEX_NONE;
				int32 DstIndex = DstChannelIndex;
				while (++SampleIndex < InNumSamples)
				{
					DstBuffer[DstIndex]  = .5f * SrcBuffer[++SrcIndex];
					DstBuffer[DstIndex] += .5f * SrcBuffer[++SrcIndex];
					DstIndex += NumDstChannels;
				}
			}
			else if (Mode == EBiquadMixerMode::BroadcastMonoFromMultichannel)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 SrcIndex = INDEX_NONE;
				int32 DstIndex = DstChannelIndex;
				while (++SampleIndex < InNumSamples)
				{
					DstBuffer[DstIndex] = 0.f;
					for (int32 I = 0; I < NumSrcChannels; ++I)
					{
						DstBuffer[DstIndex] += SrcBuffer[++SrcIndex] / NumSrcChannels;
					}
					DstIndex += NumDstChannels;
				}
			}
		}
	}

	void FBiquadBufferMixer::ProcessInterlaced(const float*     SrcBuffer,
	                                           float*           DstBuffer,
	                                           EBiquadMixerMode InMode,
	                                           int32            InNumSamples,
	                                           int32            InNumSrcChannels,
	                                           int32            InNumDstChannels,
	                                           int32            InChannelIndex)
	{
		if (bDoFilter)
		{
			// Mix them with applying effect.

			if (InMode == EBiquadMixerMode::NoBlending)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 ChIndex = InChannelIndex;
				float BlendAlpha;

				while (++SampleIndex < InNumSamples)
				{
					BlendAlpha = (float) SampleIndex / InNumSamples;
					DstBuffer[ChIndex] = FMath::Lerp(BiquadOld->ProcessAudio(SrcBuffer[ChIndex]) * MakeupCoeffOld, 
													 BiquadNew->ProcessAudio(SrcBuffer[ChIndex]) * MakeupCoeffNew, 
													 BlendAlpha);
					ChIndex += InNumSrcChannels;
				}
			}
			else if (InMode == EBiquadMixerMode::BroadcastMono)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 DstIndex = InChannelIndex;
				float BlendAlpha;

				while (++SampleIndex < InNumSamples)
				{
					BlendAlpha = (float) SampleIndex / InNumSamples;
					float S1 = BiquadOld->ProcessAudio(SrcBuffer[SampleIndex]) * MakeupCoeffOld;
					float S2 = BiquadNew->ProcessAudio(SrcBuffer[SampleIndex]) * MakeupCoeffNew;
					DstBuffer[DstIndex] = FMath::Lerp(S1, S2, BlendAlpha);
					DstIndex += InNumDstChannels;
				}
			}
			else if (InMode == EBiquadMixerMode::BroadcastMonoFromStereo)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 SrcIndex = INDEX_NONE;
				int32 DstIndex = InChannelIndex;
				float MonoSample, BlendAlpha;

				while (++SampleIndex < InNumSamples)
				{
					MonoSample = .5f * (SrcBuffer[++SrcIndex] + SrcBuffer[++SrcIndex]);
					BlendAlpha = (float) SampleIndex / InNumSamples;
					DstBuffer[DstIndex] = FMath::Lerp(BiquadOld->ProcessAudio(MonoSample) * MakeupCoeffOld, 
													  BiquadNew->ProcessAudio(MonoSample) * MakeupCoeffNew, 
													  BlendAlpha);
					DstIndex += InNumDstChannels;
				}
			}
			else if (InMode == EBiquadMixerMode::BroadcastMonoFromMultichannel)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 SrcIndex = INDEX_NONE;
				int32 DstIndex = InChannelIndex;
				float MonoSample = 0.f, BlendAlpha;

				while (++SampleIndex < InNumSamples)
				{
					DstBuffer[DstIndex] = 0.f;
					for (int32 I = 0; I < InNumSrcChannels; ++I)
					{
						MonoSample += SrcBuffer[++SrcIndex] / InNumSrcChannels;
					}

					BlendAlpha = (float) SampleIndex / InNumSamples;
					DstBuffer[DstIndex] = FMath::Lerp(BiquadOld->ProcessAudio(MonoSample) * MakeupCoeffOld, 
													  BiquadNew->ProcessAudio(MonoSample) * MakeupCoeffNew, 
													  BlendAlpha);
					DstIndex += InNumDstChannels;
				}
			}
		}
		else
		{
			// Mix them without applying effect.

			if (InMode == EBiquadMixerMode::NoBlending)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 SrcIndex = InChannelIndex;
				int32 DstIndex = InChannelIndex;
				while (++SampleIndex < InNumSamples)
				{
					DstBuffer[DstIndex] = SrcBuffer[SrcIndex];
					SrcIndex += InNumSrcChannels;
					DstIndex += InNumDstChannels;
				}
			}
			else if (InMode == EBiquadMixerMode::BroadcastMono)
			{
				int32 SrcIndex = 0;
				int32 DstIndex = InChannelIndex;
				while (SrcIndex < InNumSamples)
				{
					DstBuffer[DstIndex] = SrcBuffer[SrcIndex];
					DstIndex += InNumDstChannels;
					++SrcIndex;
				}
			}
			else if (InMode == EBiquadMixerMode::BroadcastMonoFromStereo)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 SrcIndex = INDEX_NONE;
				int32 DstIndex = InChannelIndex;
				while (++SampleIndex < InNumSamples)
				{
					DstBuffer[DstIndex]  = .5f * SrcBuffer[++SrcIndex];
					DstBuffer[DstIndex] += .5f * SrcBuffer[++SrcIndex];
					DstIndex += InNumDstChannels;
				}
			}
			else if (InMode == EBiquadMixerMode::BroadcastMonoFromMultichannel)
			{
				int32 SampleIndex = INDEX_NONE;
				int32 SrcIndex = INDEX_NONE;
				int32 DstIndex = InChannelIndex;
				while (++SampleIndex < InNumSamples)
				{
					DstBuffer[DstIndex] = 0.f;
					for (int32 I = 0; I < InNumSrcChannels; ++I)
					{
						DstBuffer[DstIndex] += SrcBuffer[++SrcIndex] / InNumSrcChannels;
					}
					DstIndex += InNumDstChannels;
				}
			}
		}
	}

	void FBiquadBufferMixer::Reset()
	{
		BiquadNew->Reset();
	}

	void FBiquadBufferMixer::NotifyBufferChanged()
	{
		MakeupCoeffOld = MakeupCoeffNew;
		*BiquadOld = *BiquadNew;
	}

	void FBiquadBufferMixer::SetParams(EEQBandType InType,
	                             float       InFrequency,
	                             float       InAmountDb,
	                             float       InQuality,
	                             float       InMakeupDb)
	{
		MakeupCoeffNew = MathLogTool::VigesibelToLinear(InMakeupDb);
		InFrequency = FMath::Clamp(InFrequency, 5.f, FMath::Min(20000.f, .5f * SampleRate));

		if (FilterType != InType ||
			!FMath::IsNearlyEqual(Frequency, InFrequency) ||
			!FMath::IsNearlyEqual(Quality, InQuality) ||
			!FMath::IsNearlyEqual(AmountDb, InAmountDb))
		{
			FilterType = InType;
			Frequency = InFrequency;
			AmountDb = InAmountDb;
			Quality = InQuality;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadBufferMixer::SetDoFilter(const bool bInDoFilter)
	{
		bDoFilter = bInDoFilter;
	}

	void FBiquadBufferMixer::SetType(EEQBandType InFilterType)
	{
		if (FilterType != InFilterType)
		{
			FilterType = InFilterType;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadBufferMixer::SetFrequency(float InFrequency)
	{
		InFrequency = FMath::Clamp(InFrequency, 5.f, FMath::Min(20000.f, .5f * SampleRate));
		if (Frequency != InFrequency)
		{
			Frequency = InFrequency;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadBufferMixer::SetAmountDb(const float InAmountDb)
	{
		if (AmountDb != InAmountDb)
		{
			AmountDb = InAmountDb;
			if (FilterType == EEQBandType::AttBand || 
				FilterType == EEQBandType::AttHigh || 
				FilterType == EEQBandType::AttLow)
			{
				CalculateBiquadCoefficients();
			}
		}
	}

	void FBiquadBufferMixer::SetQuality(const float InQuality)
	{
		if (Quality != InQuality)
		{
			Quality = InQuality;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadBufferMixer::SetMakeupDb(float InMakeupDb)
	{
		MakeupCoeffNew = MathLogTool::VigesibelToLinear(InMakeupDb);
	}

	void FBiquadBufferMixer::CalculateBiquadCoefficients()
	{
		float A0;
		float A1;
		float A2;
		float B0;
		float B1;
		float B2;

		switch (FilterType)
		{
			default:
			case EEQBandType::CutLowFast:
			{
				const float Omega = 2.0f * PI * Frequency / SampleRate;
				const float Sn = FMath::Sin(Omega);
				const float Cs = FMath::Cos(Omega);
				const float Alpha = .5f * Sn / Quality;

				A0 =  (1.0f + Cs) / 2.0f;
				A1 = -(1.0f + Cs);
				A2 =  (1.0f + Cs) / 2.0f;
				B0 =  1.0f + Alpha;
				B1 = -2.0f * Cs;
				B2 =  1.0f - Alpha;
			}
			break;

			case EEQBandType::CutLowButterworth:
			{
				const float Lambda = FMath::Tan(PI * Frequency / SampleRate);
				const float LambdaScaled = UE_SQRT_2 * Lambda / Quality;
				const float LambdaSq = Lambda * Lambda;

				A0 = 1.f / (1.f + LambdaScaled + LambdaSq);
				A1 = -2.f * A0;
				A2 = A0;

				B0 = 1.f;
				B1 = -A1 * (LambdaSq - 1.f);
				B2 = A0 * (1.f - LambdaScaled + LambdaSq);
			}
			break;

			case EEQBandType::CutHighFast:
			{
				const float Omega = 2.0f * PI * Frequency / SampleRate;
				const float Sn = FMath::Sin(Omega);
				const float Cs = FMath::Cos(Omega);
				const float Alpha = .5f * Sn / Quality;

				A0 =  (1.0f - Cs) / 2.0f;
				A1 =  (1.0f - Cs);
				A2 =  (1.0f - Cs) / 2.0f;
				B0 =  1.0f + Alpha;
				B1 = -2.0f * Cs;
				B2 =  1.0f - Alpha;
			}
			break;

			case EEQBandType::CutHighButterworth:
			{
				float Lambda = 1.f / FMath::Tan(PI * Frequency / SampleRate);
				float LambdaScaled = UE_SQRT_2 * Lambda / Quality;
				float LambdaSq = Lambda * Lambda;

				A0 = 1.f / (1.f + LambdaScaled + LambdaSq);
				A1 = 2.f * A0;
				A2 = A0;

				B0 = 1.f;
				B1 = A1 * (1.f - LambdaSq);
				B2 = A0 * (1.f - LambdaScaled + LambdaSq);
			}
			break;

			case EEQBandType::PassBand:
			{
				const float Omega = 2.0f * PI * Frequency / SampleRate;
				const float Sn = FMath::Sin(Omega);
				const float Cs = FMath::Cos(Omega);
				const float Alpha = .5f * Sn / Quality;

				A0 = Alpha * MakeupCoeffNew;
				A1 = 0.0f;
				A2 = -Alpha * MakeupCoeffNew;
				B0 = 1.0f + Alpha;
				B1 = -2.0f * Cs;
				B2 = 1.0f - Alpha;
			}
			break;

			case EEQBandType::PassAll:
			{
				const float Omega = 2.0f * PI * Frequency / SampleRate;
				const float Sn = FMath::Sin(Omega);
				const float Cs = FMath::Cos(Omega);
				const float Alpha = .5f * Sn / Quality;

				A0 = 1.0f - Alpha;
				A1 = -2.0f * Cs;
				A2 = 1.0f + Alpha;
				B0 = 1.0f + Alpha;
				B1 = -2.0f * Cs;
				B2 = 1.0f - Alpha;
			}
			break;

			case EEQBandType::AttLow:
			{
				const float Amp = FMath::Pow(10.0f, AmountDb / 40.0f);
				const float Omega = 2.0f * PI * Frequency / SampleRate;
				const float Sn = FMath::Sin(Omega);
				const float Cs = FMath::Cos(Omega);
				const float BetaSn = Sn * FMath::Sqrt(2.0f * Amp);

				A0 = Amp * ((Amp + 1) - (Amp - 1) * Cs + BetaSn);
				A1 = 2 * Amp * ((Amp - 1) - (Amp + 1) * Cs);
				A2 = Amp * ((Amp + 1) - (Amp - 1) * Cs - BetaSn);
				B0 = (Amp + 1) + (Amp - 1) * Cs + BetaSn;
				B1 = -2 * ((Amp - 1) + (Amp + 1) * Cs);
				B2 = (Amp + 1) + (Amp - 1) * Cs - BetaSn;
			}
			break;

			case EEQBandType::AttHigh:
			{
				const float Amp = FMath::Pow(10.0f, AmountDb / 40.0f);
				const float Omega = 2.0f * PI * Frequency / SampleRate;
				const float Sn = FMath::Sin(Omega);
				const float Cs = FMath::Cos(Omega);
				const float BetaSn = Sn * FMath::Sqrt(2.0f * Amp);

				A0 = Amp * ((Amp + 1) + (Amp - 1) * Cs + BetaSn);
				A1 = -2 * Amp * ((Amp - 1) + (Amp + 1) * Cs);
				A2 = Amp * ((Amp + 1) + (Amp - 1) * Cs - BetaSn);
				B0 = (Amp + 1) - (Amp - 1) * Cs + BetaSn;
				B1 = 2 * ((Amp - 1) - (Amp + 1) * Cs);
				B2 = (Amp + 1) - (Amp - 1) * Cs - BetaSn;
			}
			break;

			case EEQBandType::AttBand:
			{
				const float Amp = FMath::Pow(10.0f, AmountDb / 40.0f);
				const float Omega = 2.0f * PI * Frequency / SampleRate;
				const float Sn = FMath::Sin(Omega);
				const float Cs = FMath::Cos(Omega);
				const float Alpha = .5f * Sn / Quality;

				A0 = 1.0f + (Alpha * Amp);
				A1 = -2.0f * Cs;
				A2 = 1.0f - (Alpha * Amp);
				B0 = 1.0f + (Alpha / Amp);
				B1 = -2.0f * Cs;
				B2 = 1.0f - (Alpha / Amp);
			}
			break;

			case EEQBandType::Notch:
			{
				const float Omega = 2.0f * PI * Frequency / SampleRate;
				const float Sn = FMath::Sin(Omega);
				const float Cs = FMath::Cos(Omega);
				const float Alpha = .5f * Sn / Quality;

				A0 = 1.0f;
				A1 = -2.0f * Cs;
				A2 = 1.0f;
				B0 = 1.0f + Alpha;
				B1 = -2.0f * Cs;
				B2 = 1.0f - Alpha;
			}
			break;
		}

		A0 /= B0;
		A1 /= B0;
		A2 /= B0;
		B1 /= B0;
		B2 /= B0;

		BiquadNew->A0 = A0;
		BiquadNew->A1 = A1;
		BiquadNew->A2 = A2;
		BiquadNew->B1 = B1;
		BiquadNew->B2 = B2;
	}
}
