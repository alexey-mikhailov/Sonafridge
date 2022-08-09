// Sonafridge 2022

#include "VM_ClathriQ.h"
#include "AudioDevice.h"
#include "Sonafridge/Attenuator/Clathrispace.h"
#include "Sonafridge/Tools/MathTools.h"

/// Frequency logarithmic audible minimum. 
const float FVM_ClathriQ::FLMin = FMath::Loge(20.f);

/// Frequency logarithmic audible maximun. 
const float FVM_ClathriQ::FLMax = FMath::Loge(20000.f);

/// Frequency logarithmic step. 
const float FVM_ClathriQ::FLS = (FLMax - FLMin) * ResolutionStep;

FVM_ClathriQ::FVM_ClathriQ()
{
	// We cannot obtain SampleRate from SFX due to encapsulation
	// but we can do it this way.
	// Note that design time sample rate is not very important visualization input quality metric. 
	if (auto AudioDevice = GEngine->GetActiveAudioDevice())
	{
		SampleRate = AudioDevice->GetSampleRate();
	}
	else
	{
		// Last known default value from project template settings. 
		SampleRate = 48000.f;
	}

	Response.SetNumZeroed(Resolution + 1);
}

void FVM_ClathriQ::Init(UClathrispaceSettings* InSettings, float InSampleRate)
{
	Settings = InSettings;
	SampleRate = InSampleRate;
}

void FVM_ClathriQ::SetEnabled_Band1(bool bEnabled)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->bBand1Enabled = bEnabled;
		Recalculate();
	}
}

void FVM_ClathriQ::SetFrequency_Band1(float InFrequency)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band1Frequency = InFrequency;
		Recalculate();
	}
}

void FVM_ClathriQ::SetAmountDb_Band1(float InAmountDb)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band1AmountDb = InAmountDb;
		Recalculate();
	}
}

void FVM_ClathriQ::SetQuality_Band1(float InQuality)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band1Quality = InQuality;
		Recalculate();
	}
}

void FVM_ClathriQ::SetMakeupDb_Band1(float InMakeupDb)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band1MakeupDb = InMakeupDb;
		Recalculate();
	}
}

void FVM_ClathriQ::SetEnabled_Band2(bool bEnabled)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->bBand2Enabled = bEnabled;
		Recalculate();
	}
}

void FVM_ClathriQ::SetFrequency_Band2(float InFrequency)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band2Frequency = InFrequency;
		Recalculate();
	}
}

void FVM_ClathriQ::SetAmountDb_Band2(float InAmountDb)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band2AmountDb = InAmountDb;
		Recalculate();
	}
}

void FVM_ClathriQ::SetQuality_Band2(float InQuality)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band2Quality = InQuality;
		Recalculate();
	}
}

void FVM_ClathriQ::SetMakeupDb_Band2(float InMakeupDb)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band2MakeupDb = InMakeupDb;
		Recalculate();
	}
}

void FVM_ClathriQ::SetEnabled_Band3(bool bEnabled)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->bBand3Enabled = bEnabled;
		Recalculate();
	}
}

void FVM_ClathriQ::SetFrequency_Band3(float InFrequency)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band3Frequency = InFrequency;
		Recalculate();
	}
}

void FVM_ClathriQ::SetAmountDb_Band3(float InAmountDb)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band3AmountDb = InAmountDb;
		Recalculate();
	}
}

void FVM_ClathriQ::SetQuality_Band3(float InQuality)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band3Quality = InQuality;
		Recalculate();
	}
}

void FVM_ClathriQ::SetMakeupDb_Band3(float InMakeupDb)
{
	if (FEarPin* Pin = PullDataContext())
	{
		Pin->Band3MakeupDb = InMakeupDb;
		Recalculate();
	}
}

void FVM_ClathriQ::SetSelectedBandIndex(int32 InBandIndex)
{
	SelectedBandIndex = InBandIndex;
	BandSelectionChanged.Broadcast(InBandIndex);
}

float FVM_ClathriQ::Dtft(float InFrequency) const
{
	float Index = (FMath::Loge(InFrequency) - FLMin) / FLS;

	int32 Index1 = FMath::FloorToInt(Index);
	int32 Index2 = FMath::CeilToInt(Index);
	float Alpha = FMath::Frac(Index);
	return FMath::Lerp(Response[Index1], Response[Index2], Alpha);
}

float FVM_ClathriQ::DtftDb(float InFrequency) const
{
	float Index = (FMath::Loge(InFrequency) - FLMin) / FLS;

	int32 Index1 = FMath::FloorToInt(Index);
	int32 Index2 = FMath::CeilToInt(Index);
	float Alpha = FMath::Frac(Index);

	return MathLogTool::LinearToVigesibel
	(
		FMath::Lerp(Response[Index1], Response[Index2], Alpha)
	);
}

void FVM_ClathriQ::Recalculate()
{
	if (FEarPin* Pin = PullDataContext())
	{
		//
		// Calc biquad coefficients.
		//

		FVector Band1A, Band1B, Band2A, Band2B, Band3A, Band3B;

		float GainSqrt;;
		float Omega;
		float Sn;
		float Cs;
		float Alpha;
		float DoubleAlphaGain;

		// Band 1. Low-shelf filter. 
		GainSqrt = FMath::Pow(10, Pin->Band1AmountDb / 40.f);
		Omega = 2.f * PI * Pin->Band1Frequency / SampleRate;
		Sn = FMath::Sin(Omega);
		Cs = FMath::Cos(Omega);
		Alpha = .5f * Sn / Pin->Band1Quality;
		DoubleAlphaGain = 2.f * Alpha * FMath::Sqrt(GainSqrt);

		Band1A[0] = GainSqrt + 1.f + Cs * (GainSqrt - 1.f) + DoubleAlphaGain;
		Band1A[1] = -2.f * (GainSqrt - 1.f + Cs * (GainSqrt + 1.f));
		Band1A[2] = GainSqrt + 1.f + Cs * (GainSqrt - 1.f) - DoubleAlphaGain;
		Band1B[0] = GainSqrt * (GainSqrt + 1.f - Cs * (GainSqrt - 1.f) + DoubleAlphaGain);
		Band1B[1] = +2.f * GainSqrt * (GainSqrt - 1.f - Cs * (GainSqrt + 1.f));
		Band1B[2] = GainSqrt * (GainSqrt + 1.f - Cs * (GainSqrt - 1.f) - DoubleAlphaGain);

		// Band 2. Band-cut filter.
		GainSqrt = FMath::Pow(10, Pin->Band2AmountDb / 40.f);
		Omega = 2.f * PI * Pin->Band2Frequency / SampleRate;
		Sn = FMath::Sin(Omega);
		Cs = FMath::Cos(Omega);
		Alpha = .5f * Sn / Pin->Band2Quality;

		Band2A[0] = 1.f + Alpha / GainSqrt;
		Band2A[1] = -2.f * Cs;
		Band2A[2] = 1.f - Alpha / GainSqrt;
		Band2B[0] = 1.f + Alpha * GainSqrt;
		Band2B[1] = -2.f * Cs;
		Band2B[2] = 1.f - Alpha * GainSqrt;

		// Band 3. High-shelf filter.
		GainSqrt = FMath::Pow(10, Pin->Band3AmountDb / 40.f);
		Omega = 2.f * PI * Pin->Band3Frequency / SampleRate;
		Sn = FMath::Sin(Omega);
		Cs = FMath::Cos(Omega);
		Alpha = .5f * Sn / Pin->Band3Quality;
		DoubleAlphaGain = 2.f * Alpha * FMath::Sqrt(GainSqrt);

		Band3A[0] = GainSqrt + 1.f - Cs * (GainSqrt - 1.f) + DoubleAlphaGain;
		Band3A[1] = +2.f * (GainSqrt - 1.f - Cs * (GainSqrt + 1.f));
		Band3A[2] = GainSqrt + 1.f - Cs * (GainSqrt - 1.f) - DoubleAlphaGain;
		Band3B[0] = GainSqrt * (GainSqrt + 1.f + Cs * (GainSqrt - 1.f) + DoubleAlphaGain);
		Band3B[1] = -2.f * GainSqrt * (GainSqrt - 1.f + Cs * (GainSqrt + 1.f));
		Band3B[2] = GainSqrt * (GainSqrt + 1.f + Cs * (GainSqrt - 1.f) - DoubleAlphaGain);

		//
		// Bake response.
		//

		ResponseAvgDb = 0.f;

		for (int32 I = 0; I < Resolution + 1; ++I)
		{
			Response[I] = 1.f;
		}

		FEcn Angle0, Angle1, Angle2;
		for (int32 I = 0; I < Resolution + 1; ++I)
		{
			float F = FMath::Exp(I * FLS + FLMin);
			Omega = 2.f * PI * F / SampleRate;
			Angle0 = FEcn(Omega) ^ -0;
			Angle1 = FEcn(Omega) ^ -1;
			Angle2 = FEcn(Omega) ^ -2;

			float ThisResponse = 
			(
				(Band1B[0] * Angle0 + Band1B[1] * Angle1 + Band1B[2] * Angle2) /
				(Band1A[0] * Angle0 + Band1A[1] * Angle1 + Band1A[2] * Angle2)
			)
			.Radius;

			float ThisDb = MathLogTool::LinearToVigesibel(ThisResponse);

			float Alpha96Db = (ThisDb - DynamicMin) / (DynamicMax - DynamicMin);
			ThisDb *= 2.f * Alpha96Db;

			ResponseAvgDb += ThisDb;
			Response[I] *= ThisResponse;
		}

		float PrevResponse = 1.f;
		for (int32 I = 0; I < Resolution + 1; ++I)
		{
			float F = FMath::Exp(I * FLS + FLMin);
			Omega = 2.f * PI * F / SampleRate;
			Angle0 = FEcn(Omega) ^ -0;
			Angle1 = FEcn(Omega) ^ -1;
			Angle2 = FEcn(Omega) ^ -2;

			float ThisResponse = 
			(
				(Band2B[0] * Angle0 + Band2B[1] * Angle1 + Band2B[2] * Angle2) /
				(Band2A[0] * Angle0 + Band2A[1] * Angle1 + Band2A[2] * Angle2)
			)
			.Radius;

			float ThisDb = MathLogTool::LinearToVigesibel(ThisResponse);
			float PrevDb = MathLogTool::LinearToVigesibel(PrevResponse);

			float Alpha96Db = (ThisDb - DynamicMin) / (DynamicMax - DynamicMin);
			ThisDb *= 1.5f * Alpha96Db;
			Alpha96Db = (PrevDb - DynamicMin) / (DynamicMax - DynamicMin);
			PrevDb *= 1.5f * Alpha96Db;

			const float Diff = FMath::Abs(ThisDb - PrevDb);
			ThisDb *= 4.f * FMath::Pow(Diff, .5f);

			ResponseAvgDb += ThisDb;
			PrevResponse = ThisResponse;
			Response[I] *= ThisResponse;
		}

		for (int32 I = 0; I < Resolution + 1; ++I)
		{
			float F = FMath::Exp(I * FLS + FLMin);
			Omega = 2.f * PI * F / SampleRate;
			Angle0 = FEcn(Omega) ^ -0;
			Angle1 = FEcn(Omega) ^ -1;
			Angle2 = FEcn(Omega) ^ -2;

			float ThisResponse = 
			(
				(Band3B[0] * Angle0 + Band3B[1] * Angle1 + Band3B[2] * Angle2) /
				(Band3A[0] * Angle0 + Band3A[1] * Angle1 + Band3A[2] * Angle2)
			)
			.Radius;

			float ThisDb = MathLogTool::LinearToVigesibel(ThisResponse);

			float Alpha96Db = (ThisDb - DynamicMin) / (DynamicMax - DynamicMin);
			ThisDb *= 2.f * Alpha96Db;

			ResponseAvgDb += ThisDb;
			Response[I] *= ThisResponse;
		}

		constexpr float NumberOfBands = 3.f;
		ResponseAvgDb /= NumberOfBands * (Resolution + 1);
	}
}

FEarPin* FVM_ClathriQ::PullDataContext() const
{
	if (Settings.IsValid())
	{
		if (Settings->GetEarData().EarPinsL.Num() > PinIndex)
		{
			return &Settings->GetEarData().EarPinsL[PinIndex];
		}
	}

	return nullptr;
}

void FVM_ClathriQ::PushDataContext()
{
	if (Settings.IsValid())
	{
		Settings->CopyLeftToRight();

		if (!Settings->MarkPackageDirty())
		{
			UE_LOG(LogSonafridgeEditor,
			       Error,
			       TEXT("FVM_ClathriQ::PushDataContext: Could not mark package dirty. "));
		}
	}
}



