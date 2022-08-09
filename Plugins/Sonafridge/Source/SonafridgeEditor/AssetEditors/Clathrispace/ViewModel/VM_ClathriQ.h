// Sonafridge 2022

#pragma once

struct FEarPin;
class UClathrispaceSettings;

class SONAFRIDGEEDITOR_API FVM_ClathriQ
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

	FVM_ClathriQ();
	void Init(UClathrispaceSettings* InSettings, float InSampleRate);

	void SetEnabled_Band1(bool bEnabled);
	void SetFrequency_Band1(float InFrequency);
	void SetAmountDb_Band1(float InAmountDb);
	void SetQuality_Band1(float InQuality);
	void SetMakeupDb_Band1(float InMakeupDb);
	void SetEnabled_Band2(bool bEnabled);
	void SetFrequency_Band2(float InFrequency);
	void SetAmountDb_Band2(float InAmountDb);
	void SetQuality_Band2(float InQuality);
	void SetMakeupDb_Band2(float InMakeupDb);
	void SetEnabled_Band3(bool bEnabled);
	void SetFrequency_Band3(float InFrequency);
	void SetAmountDb_Band3(float InAmountDb);
	void SetQuality_Band3(float InQuality);
	void SetMakeupDb_Band3(float InMakeupDb);

	bool  GetIsEnabled() const { return bIsEnabled; }
	float GetSampleRate() const { return SampleRate; }

	void SetIsEnabled(bool bNewValue) { bIsEnabled = bNewValue; }

	int32 GetSelectedBandIndex() const { return SelectedBandIndex; }
	void  SetSelectedBandIndex(int32 InBandIndex);

	float Dtft(float InFrequency) const;
	float DtftDb(float InFrequency) const;

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandSelectionChanged, int32)
	FBandSelectionChanged& GetEvent_BandSelectionChanged() { return BandSelectionChanged; }

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandChanging, int32);
	FBandChanging& GetEvent_BandChanging() { return BandChanging; }

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandChanged, int32);
	FBandChanged& GetEvent_BandChanged() { return BandChanged; }

private:
	void Recalculate();

	FEarPin* PullDataContext() const;
	void     PushDataContext();

	TWeakObjectPtr<UClathrispaceSettings> Settings = nullptr;
	int32                                 PinIndex = INDEX_NONE;
	
	FBandSelectionChanged BandSelectionChanged;
	FBandChanging         BandChanging;
	FBandChanged          BandChanged;

	bool  bIsEnabled        = true;
	float SampleRate        = 48000.f;
	int32 SelectedBandIndex = INDEX_NONE;

	TArray<float> Response;
	float ResponseAvgDb = 0.f;
};

