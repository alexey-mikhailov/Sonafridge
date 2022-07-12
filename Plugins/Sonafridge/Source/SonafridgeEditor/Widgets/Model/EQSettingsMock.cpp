// Sonafridge 2022

#include "EQSettingsMock.h"
#include "AudioDevice.h"
#include "Sonafridge/MathTools.h"

FEQSettingsMock::FEQSettingsMock()
{
	if (auto AudioDevice = GEngine->GetActiveAudioDevice())
	{
		SampleRate = AudioDevice->GetSampleRate();
	}
}

void FEQSettingsMock::Init(TArray<TSharedPtr<FEQBand>> InBands)
{
	Bands = InBands;
}

void FEQSettingsMock::AddBand(TSharedPtr<FEQBand> InBand)
{
	Bands.Add(InBand);
}

void FEQSettingsMock::RemoveBand(TSharedPtr<FEQBand> InBand)
{
	Bands.Remove(InBand);
}

void FEQSettingsMock::ResetBand(TSharedPtr<FEQBand> InBand)
{
	InBand->SetFrequency(1000.f);
	InBand->SetQuality(1.f);
	InBand->SetAmountDb(0.f);
	InBand->SetLoudCompDb(0.f);
}

void FEQSettingsMock::Freeze()
{
	// TODO: Optimize response calculation
	// TODO: bIsFrozen = true ...
}

void FEQSettingsMock::Unfreeze()
{
	// TODO: Optimize response calculation
	// TODO: bIsFrozen = false ...
}

float FEQSettingsMock::Dtft(float InFrequency) const
{
	float Result = 1.f;
	for (TSharedPtr<FEQBand> Band : Bands)
	{
		Result *= Band->Dtft(InFrequency);
	}

	return Result;
}

float FEQSettingsMock::DtftDb(float InFrequency) const
{
	float Result = 1.f;
	for (TSharedPtr<FEQBand> Band : Bands)
	{
		Result *= Band->Dtft(InFrequency);
	}

	return MathLogTool::LinearToVigesibel(Result);
}

