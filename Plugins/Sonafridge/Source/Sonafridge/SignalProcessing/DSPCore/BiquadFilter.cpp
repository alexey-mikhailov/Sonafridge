// Sonafridge 2022

#include "BiquadFilter.h"
#include "DSP/BiQuadFilter.h"

namespace Sonafridge
{
	float FBiquadFilter::ClampCutoffFrequency(float InCutoffFrequency)
	{
		return FMath::Clamp(InCutoffFrequency, 5.0f, SampleRate / 2.0f - 1.0f);
	}

	FBiquadFilter::FBiquadFilter()
		: SampleRate(0.f)
		, NumChannels(0)
		, bEnabled(true)
		, OutputFactor(1.f)
		, FilterType(EEQBandType::AttBand)
		, GainDB(0.f)
		, Frequency(1000.f)
		, Quality(1.f)
		, Biquad(nullptr)
	{
	}

	FBiquadFilter::~FBiquadFilter()
	{
		if (Biquad)
		{
			delete[] Biquad;
		}
	}

	void FBiquadFilter::Init(const float       InSampleRate,
	                         const int32       InNumChannels,
	                         const float       InOutputDB,
	                         const EEQBandType InFilterType,
	                         const float       InGainDB,
	                         const float       InCutoffFrequency,
	                         const float       InQuality)
	{
		SampleRate = InSampleRate;
		NumChannels = InNumChannels;
		OutputFactor = FMath::Pow(10.0f, InOutputDB / 40.0f);
		FilterType = InFilterType;
		GainDB = InGainDB;
		Frequency = ClampCutoffFrequency(InCutoffFrequency);
		Quality = InQuality;

		if (Biquad)
		{
			delete[] Biquad;
		}

		Biquad = new Audio::FBiquad[NumChannels];
		Reset();
		CalculateBiquadCoefficients();
	}

	int32 FBiquadFilter::GetNumChannels() const
	{
		return NumChannels;
	}

	void FBiquadFilter::ProcessAudioFrame(const float* InFrame, float* OutFrame)
	{
		for (int32 Channel = 0; Channel < NumChannels; ++Channel)
		{
			OutFrame[Channel] = Biquad[Channel].ProcessAudio(InFrame[Channel]) * OutputFactor;
		}
	}

	void FBiquadFilter::ProcessAudio(const float* InBuffer, const int32 InNumSamples, float* OutBuffer)
	{
		if (bEnabled)
		{
			for (int32 SampleIndex = 0; SampleIndex < InNumSamples; SampleIndex += NumChannels)
			{
				ProcessAudioFrame(&InBuffer[SampleIndex], &OutBuffer[SampleIndex]);
			}
		}
		else
		{
			FMemory::Memcpy(OutBuffer, InBuffer, InNumSamples * sizeof(float));
		}
	}

	void FBiquadFilter::Reset()
	{
		for (int32 Channel = 0; Channel < NumChannels; ++Channel)
		{
			Biquad[Channel].Reset();
		}
	}

	void FBiquadFilter::SetParams(const float       InOutputDB,
	                              const EEQBandType InFilterType,
	                              const float       InGainDB,
	                              const float       InCutoffFrequency,
	                              const float       InQuality)
	{
		const float InCutoffFrequencyClamped = ClampCutoffFrequency(InCutoffFrequency);

		OutputFactor = FMath::Pow(10.0f, InOutputDB / 40.0f);

		if (FilterType != InFilterType ||
			!FMath::IsNearlyEqual(Frequency, InCutoffFrequencyClamped) ||
			!FMath::IsNearlyEqual(Quality, InQuality) ||
			!FMath::IsNearlyEqual(GainDB, InGainDB))
		{
			FilterType = InFilterType;
			Frequency = InCutoffFrequencyClamped;
			Quality = InQuality;
			GainDB = InGainDB;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadFilter::SetType(const EEQBandType InFilterType)
	{
		if (FilterType != InFilterType)
		{
			FilterType = InFilterType;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadFilter::SetFrequency(const float InCutoffFrequency)
	{
		const float InCutoffFrequencyClamped = ClampCutoffFrequency(InCutoffFrequency);
		if (Frequency != InCutoffFrequencyClamped)
		{
			Frequency = InCutoffFrequencyClamped;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadFilter::SetQuality(const float InQuality)
	{
		if (Quality != InQuality)
		{
			Quality = InQuality;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadFilter::SetGainDB(const float InGainDB)
	{
		if (GainDB != InGainDB)
		{
			GainDB = InGainDB;
			if (FilterType == EEQBandType::AttBand || 
				FilterType == EEQBandType::AttHigh || 
				FilterType == EEQBandType::AttLow)
			{
				CalculateBiquadCoefficients();
			}
		}
	}

	void FBiquadFilter::SetEnabled(const bool bInEnabled)
	{
		bEnabled = bInEnabled;
	}

	void FBiquadFilter::CalculateBiquadCoefficients()
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

				A0 = Alpha;
				A1 = 0.0f;
				A2 = -Alpha;
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
				const float Amp = FMath::Pow(10.0f, GainDB / 40.0f);
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
				const float Amp = FMath::Pow(10.0f, GainDB / 40.0f);
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
				const float Amp = FMath::Pow(10.0f, GainDB / 40.0f);
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

		for (int32 Channel = 0; Channel < NumChannels; ++Channel)
		{
			Biquad[Channel].A0 = A0;
			Biquad[Channel].A1 = A1;
			Biquad[Channel].A2 = A2;
			Biquad[Channel].B1 = B1;
			Biquad[Channel].B2 = B2;
		}
	}
}
