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

	Response.SetNum(Resolution + 1);
	for (int32 Index = 0; Index < Resolution + 1; ++Index) Response[Index] = 1.f;
}

FVM_ClathriQ::~FVM_ClathriQ()
{
	BandChanged.Remove(BandChangedDelegateHandle);
}

void FVM_ClathriQ::Init(UClathrispaceSettings*  InSettings,
                        TDelegate<void(int32)>& InPinIndexChanged,
                        float                   InSampleRate)
{
	InPinIndexChanged.BindLambda([this](int32 NewPinIndex)
	{
		PinIndex = NewPinIndex;
		Recalculate();
		PinIndexChanged.Broadcast(NewPinIndex);
	});

	Settings = InSettings;
	SampleRate = InSampleRate;

	BandChangedDelegateHandle = BandChanged.AddLambda([this](int32 InBandIndex) { Settings->GetEvent_InternallyChanged().Broadcast(); });
}

void FVM_ClathriQ::SetIsEnabled(int32 BandIndex, bool bEnabled)
{
	if (FEarPin* Pin = PullDataContext())
	{
		switch (BandIndex)
		{
			case 0:	Pin->bBand1Enabled = bEnabled; break;
			case 1:	Pin->bBand2Enabled = bEnabled; break;
			case 2:	Pin->bBand3Enabled = bEnabled; break;
			default:
			{
				UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::SetIsEnabled: Argument BandIndex is out of range. "));
			}
		}
		
		Recalculate();
	}
	else
	{
		UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::SetIsEnabled: Could not get data context. "));
	}
}

void FVM_ClathriQ::SetFrequency(int32 BandIndex, float InFrequency)
{
	if (FEarPin* Pin = PullDataContext())
	{
		switch (BandIndex)
		{
			case 0:	Pin->Band1Frequency = InFrequency; break;
			case 1:	Pin->Band2Frequency = InFrequency; break;
			case 2:	Pin->Band3Frequency = InFrequency; break;
			default:
			{
				UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::SetFrequency: Argument BandIndex is out of range. "));
			}
		}
		
		Recalculate();
	}
	else
	{
		UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::SetFrequency: Could not get data context. "));
	}
}

void FVM_ClathriQ::SetAmountDb(int32 BandIndex, float InAmountDb)
{
	if (FEarPin* Pin = PullDataContext())
	{
		switch (BandIndex)
		{
			case 0:	Pin->Band1AmountDb = InAmountDb; break;
			case 1:	Pin->Band2AmountDb = InAmountDb; break;
			case 2:	Pin->Band3AmountDb = InAmountDb; break;
			default:
			{
				UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::SetAmountDb: Argument BandIndex is out of range. "));
			}
		}
		
		Recalculate();
	}
	else
	{
		UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::SetAmountDb: Could not get data context. "));
	}
}

void FVM_ClathriQ::SetQuality(int32 BandIndex, float InQuality)
{
	if (FEarPin* Pin = PullDataContext())
	{
		switch (BandIndex)
		{
			case 0:	Pin->Band1Quality = InQuality; break;
			case 1:	Pin->Band2Quality = InQuality; break;
			case 2:	Pin->Band3Quality = InQuality; break;
			default:
			{
				UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::SetQuality: Argument BandIndex is out of range. "));
			}
		}
		
		Recalculate();
	}
	else
	{
		UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::SetQuality: Could not get data context. "));
	}
}

void FVM_ClathriQ::SetMakeupDb(int32 BandIndex, float InMakeupDb)
{
	if (FEarPin* Pin = PullDataContext())
	{
		switch (BandIndex)
		{
			case 0:	Pin->Band1MakeupDb = InMakeupDb; break;
			case 1:	Pin->Band2MakeupDb = InMakeupDb; break;
			case 2:	Pin->Band3MakeupDb = InMakeupDb; break;
			default:
			{
				UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::SetMakeupDb: Argument BandIndex is out of range. "));
			}
		}
		
		Recalculate();
	}
	else
	{
		UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::SetMakeupDb: Could not get data context. "));
	}
}

bool FVM_ClathriQ::GetIsEnabled(int32 BandIndex)
{
	if (FEarPin* Pin = PullDataContext())
	{
		switch (BandIndex)
		{
			case 0: return Pin->bBand1Enabled;
			case 1: return Pin->bBand2Enabled;
			case 2: return Pin->bBand3Enabled;
			default:
			{
				UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::GetIsEnabled: Argument BandIndex is out of range. "));
				return false;
			}
		}
	}

	UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::GetIsEnabled: Could not get data context. "));
	return false;
}

float FVM_ClathriQ::GetFrequency(int32 BandIndex)
{
	if (FEarPin* Pin = PullDataContext())
	{
		switch (BandIndex)
		{
			case 0: return Pin->Band1Frequency;
			case 1: return Pin->Band2Frequency;
			case 2: return Pin->Band3Frequency;
			default:
			{
				UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::GetFrequency: Argument BandIndex is out of range. "));
				return 1000.f;
			}
		}
	}

	UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::GetFrequency: Could not get data context. "));
	return 1000.f;
}

float FVM_ClathriQ::GetAmountDb(int32 BandIndex)
{
	if (FEarPin* Pin = PullDataContext())
	{
		switch (BandIndex)
		{
			case 0: return Pin->Band1AmountDb;
			case 1: return Pin->Band2AmountDb;
			case 2: return Pin->Band3AmountDb;
			default:
			{
				UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::GetAmountDb: Argument BandIndex is out of range. "));
				return 0.f;
			}
		}
	}

	UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::GetAmountDb: Could not get data context. "));
	return 0.f;
}

float FVM_ClathriQ::GetQuality(int32 BandIndex)
{
	if (FEarPin* Pin = PullDataContext())
	{
		switch (BandIndex)
		{
			case 0: return Pin->Band1Quality;
			case 1: return Pin->Band2Quality;
			case 2: return Pin->Band3Quality;
			default:
			{
				UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::GetQuality: Argument BandIndex is out of range. "));
				return 1.f;
			}
		}
	}

	UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::GetQuality: Could not get data context. "));
	return 1.f;
}

float FVM_ClathriQ::GetMakeupDb(int32 BandIndex)
{
	if (FEarPin* Pin = PullDataContext())
	{
		switch (BandIndex)
		{
			case 0: return Pin->Band1MakeupDb;
			case 1: return Pin->Band2MakeupDb;
			case 2: return Pin->Band3MakeupDb;
			default:
			{
				UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::GetMakeupDb: Argument BandIndex is out of range. "));
				return 0.f;
			}
		}
	}

	UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::GetMakeupDb: Could not get data context. "));
	return 0.f;
}

float FVM_ClathriQ::GetResponseAvgDb(int32 BandIndex)
{
	switch (BandIndex)
	{
		case 0: return ResponseAvgDb_Band1;
		case 1: return ResponseAvgDb_Band2;
		case 2: return ResponseAvgDb_Band3;
		default:
		{
			UE_LOG(LogSonafridgeEditor, Error, TEXT("FVM_ClathriQ::GetMakeupDb: Argument BandIndex is out of range. "));
			return 0.f;
		}
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

FEarPin* FVM_ClathriQ::PullDataContext() const
{
	if (PinIndex != INDEX_NONE && Settings.IsValid())
	{
		if (Settings->GetEarData().EarPins.Num() > PinIndex)
		{
			return &Settings->GetEarData().EarPins[PinIndex];
		}
	}

	return nullptr;
}

void FVM_ClathriQ::PushDataContext()
{
	if (Settings.IsValid())
	{
		if (!Settings->MarkPackageDirty())
		{
			UE_LOG(LogSonafridgeEditor,
			       Error,
			       TEXT("FVM_ClathriQ::PushDataContext: Could not mark package dirty. "));
		}
	}
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

		FEcn Angle0, Angle1, Angle2;

		for (int32 I = 0; I < Resolution + 1; ++I)
		{
			Response[I] = 1.f;
		}

		// Band1. Low-shelf filter.
		if (Pin->bBand1Enabled)
		{
			ResponseAvgDb_Band1 = 0.f;
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

				ResponseAvgDb_Band1 += ThisDb;
				Response[I] *= ThisResponse;
			}

			ResponseAvgDb_Band1 /= Resolution + 1;
		}

		// Band 2. Band-cut filter.
		if (Pin->bBand2Enabled)
		{
			ResponseAvgDb_Band2 = 0.f;
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

				ResponseAvgDb_Band2 += ThisDb;
				PrevResponse = ThisResponse;
				Response[I] *= ThisResponse;
			}

			ResponseAvgDb_Band2 /= Resolution + 1;
		}

		// Band 3. High-shelf filter.
		if (Pin->bBand3Enabled)
		{
			ResponseAvgDb_Band3 = 0.f;
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

				ResponseAvgDb_Band2 += ThisDb;
				Response[I] *= ThisResponse;
			}

			ResponseAvgDb_Band3 /= Resolution + 1;
		}

		if (bIsEnabled)
		{
			float MakeupCoeff1 = Pin->bBand1Enabled
			                     ? MathLogTool::VigesibelToLinear(GetMakeupDb(0))
			                     : 1.f;

			float MakeupCoeff2 = Pin->bBand2Enabled
			                     ? MathLogTool::VigesibelToLinear(GetMakeupDb(1))
			                     : 1.f;

			float MakeupCoeff3 = Pin->bBand3Enabled
			                     ? MathLogTool::VigesibelToLinear(GetMakeupDb(2))
			                     : 1.f;

			for (int32 I = 0; I < Resolution + 1; ++I)
			{
				Response[I] = Response[I] * MakeupCoeff1 * MakeupCoeff2 * MakeupCoeff3;
			}
		}
	}
}




