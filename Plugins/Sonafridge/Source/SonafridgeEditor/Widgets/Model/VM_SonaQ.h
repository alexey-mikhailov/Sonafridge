// Sonafridge 2022

#pragma once

#include "VM_SonaQBand.h"

class SONAFRIDGEEDITOR_API FVM_SonaQ
{
	bool bIsEnabled = true;
	float SampleRate = 44100.f;
	TArray<TSharedPtr<FVM_SonaQBand>> Bands;

public:
	FVM_SonaQ();
	void Init(TArray<TSharedPtr<FVM_SonaQBand>> InBands);

	bool                                     GetIsEnabled() const { return bIsEnabled; }
	float                                    GetSampleRate() const { return SampleRate; }
	const TArray<TSharedPtr<FVM_SonaQBand>>& GetBands() const { return Bands; }

	void SetIsEnabled(bool bNewValue) { bIsEnabled = bNewValue; }

	void AddBand(TSharedPtr<FVM_SonaQBand> InBand);
	void RemoveBand(TSharedPtr<FVM_SonaQBand> InBand);
	void ResetBand(TSharedPtr<FVM_SonaQBand> InBand);
	void Freeze();
	void Unfreeze();

	float Dtft(float InFrequency) const;
	float DtftDb(float InFrequency) const;
};

