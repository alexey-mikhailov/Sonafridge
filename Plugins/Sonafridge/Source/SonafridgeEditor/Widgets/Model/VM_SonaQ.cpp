// Sonafridge 2022

#include "VM_SonaQ.h"
#include "AudioDevice.h"
#include "Sonafridge/MathTools.h"

/// Frequency logarithmic audible minimum. 
const float FVM_SonaQ::FLMin = FMath::Loge(20.f);

/// Frequency logarithmic audible maximun. 
const float FVM_SonaQ::FLMax = FMath::Loge(20000.f);

/// Frequency logarithmic step. 
const float FVM_SonaQ::FLS = (FLMax - FLMin) * ResolutionStep;

FVM_SonaQ::FVM_SonaQ()
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
}

void FVM_SonaQ::Init(float InSampleRate)
{
	SampleRate = InSampleRate;
}

void FVM_SonaQ::SetBands(TArray<TSharedPtr<FVM_SonaQBand>> InBands)
{
	Bands = InBands;
}

void FVM_SonaQ::AddBand(TSharedPtr<FVM_SonaQBand> InBand)
{
	Bands.Add(InBand);
}

void FVM_SonaQ::RemoveBand(TSharedPtr<FVM_SonaQBand> InBand)
{
	Bands.Remove(InBand);
}

void FVM_SonaQ::ResetBand(TSharedPtr<FVM_SonaQBand> InBand)
{
	InBand->SetFrequency(1000.f);
	InBand->SetQuality(1.f);
	InBand->SetAmountDb(0.f);
	InBand->SetMakeupDb(0.f);
}

void FVM_SonaQ::Freeze()
{
	// TODO: Optimize response calculation
	// TODO: bIsFrozen = true ...
}

void FVM_SonaQ::Unfreeze()
{
	// TODO: Optimize response calculation
	// TODO: bIsFrozen = false ...
}

void FVM_SonaQ::SetSelectedBand(TSharedPtr<FVM_SonaQBand> InBand)
{
	SelectedBand = InBand;
	SelectedBandIndex = -1;

	for (int32 Index = 0; Index < Bands.Num(); ++Index)
	{
		auto Band = Bands[Index];

		if (Band == InBand)
		{
			SelectedBandIndex = Index;
			break;
		}
	}

	BandSelectionChanged.Broadcast(SelectedBand);
}

void FVM_SonaQ::SetSelectedBandIndex(int32 InBandIndex)
{
	SelectedBand = {};
	SelectedBandIndex = InBandIndex;

	for (int32 Index = 0; Index < Bands.Num(); ++Index)
	{
		auto& Band = Bands[Index];

		if (Index == InBandIndex)
		{
			SelectedBand = Band;
			break;
		}
	}

	BandSelectionChanged.Broadcast(SelectedBand);
}

float FVM_SonaQ::Dtft(float InFrequency) const
{
	float Result = 1.f;
	for (TSharedPtr<FVM_SonaQBand> Band : Bands)
	{
		Result *= Band->Dtft(InFrequency);
	}

	return Result;
}

float FVM_SonaQ::DtftDb(float InFrequency) const
{
	float Result = 1.f;
	for (TSharedPtr<FVM_SonaQBand> Band : Bands)
	{
		Result *= Band->Dtft(InFrequency);
	}

	return MathLogTool::LinearToVigesibel(Result);
}

