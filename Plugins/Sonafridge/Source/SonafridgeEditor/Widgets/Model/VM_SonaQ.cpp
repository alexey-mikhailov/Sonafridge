// Sonafridge 2022

#include "VM_SonaQ.h"
#include "AudioDevice.h"
#include "Sonafridge/MathTools.h"

FVM_SonaQ::FVM_SonaQ()
{
	if (auto AudioDevice = GEngine->GetActiveAudioDevice())
	{
		SampleRate = AudioDevice->GetSampleRate();
	}
}

void FVM_SonaQ::Init(TArray<TSharedPtr<FVM_SonaQBand>> InBands)
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
	InBand->SetLoudCompDb(0.f);
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

