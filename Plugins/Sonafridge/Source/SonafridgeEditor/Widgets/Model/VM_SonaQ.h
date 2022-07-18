// Sonafridge 2022

#pragma once

#include "VM_SonaQBand.h"

class SONAFRIDGEEDITOR_API FVM_SonaQ
{
public:
	static constexpr float FrequencyMin = 20.f;
	static constexpr float FrequencyMax = 20000.f;
	static constexpr float DynamicMin = -48.f;
	static constexpr float DynamicMax = +48.f;
	static constexpr int32 Resolution = 1000;
	static constexpr float ResolutionStep = 1.f / Resolution;

	/// Frequency logarithmic audible minimum. 
	static const float FLMin;

	/// Frequency logarithmic audible maximun. 
	static const float FLMax;

	/// Frequency logarithmic step. 
	static const float FLS;

	FVM_SonaQ();
	void Init(float InSampleRate);
	void SetBands(TArray<TSharedPtr<FVM_SonaQBand>> InBands);

	bool                                     GetIsEnabled() const { return bIsEnabled; }
	float                                    GetSampleRate() const { return SampleRate; }
	const TArray<TSharedPtr<FVM_SonaQBand>>& GetBands() const { return Bands; }

	void SetIsEnabled(bool bNewValue) { bIsEnabled = bNewValue; }

	void AddBand(TSharedPtr<FVM_SonaQBand> InBand);
	void RemoveBand(TSharedPtr<FVM_SonaQBand> InBand);
	void ResetBand(TSharedPtr<FVM_SonaQBand> InBand);
	void Freeze();
	void Unfreeze();

	TSharedPtr<FVM_SonaQBand> GetSelectedBand() const { return SelectedBand; }
	void                      SetSelectedBand(TSharedPtr<FVM_SonaQBand> InBand);

	int32 GetSelectedBandIndex() const { return SelectedBandIndex; }
	void  SetSelectedBandIndex(int32 InBandIndex);

	float Dtft(float InFrequency) const;
	float DtftDb(float InFrequency) const;

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandSelectionChanged, TSharedPtr<FVM_SonaQBand>)
	FBandSelectionChanged& GetEvent_BandSelectionChanged() { return BandSelectionChanged; }

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandChanging, TSharedPtr<FVM_SonaQBand>);
	FBandChanging& GetEvent_BandChanging() { return BandChanging; }

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandChanged, TSharedPtr<FVM_SonaQBand>);
	FBandChanged& GetEvent_BandChanged() { return BandChanged; }

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandAdded, TSharedPtr<FVM_SonaQBand>);
	FBandAdded& GetEvent_BandAdded() { return BandAdded; }

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandRemoved, TSharedPtr<FVM_SonaQBand>);
	FBandRemoved& GetEvent_BandRemoved() { return BandRemoved; }

private:
	FBandSelectionChanged BandSelectionChanged;
	FBandChanging         BandChanging;
	FBandChanged          BandChanged;
	FBandAdded            BandAdded;
	FBandRemoved          BandRemoved;

	bool                              bIsEnabled = true;
	float                             SampleRate = 48000.f;
	TArray<TSharedPtr<FVM_SonaQBand>> Bands;
	TSharedPtr<FVM_SonaQBand>         SelectedBand;
	int32                             SelectedBandIndex = -1;
};

