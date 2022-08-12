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
	~FVM_ClathriQ();
	void Init(UClathrispaceSettings*  InSettings,
	          TDelegate<void(int32)>& InPinIndexChanged,
	          float                   InSampleRate);

	float GetSampleRate() const { return SampleRate; }

	void SetIsEnabled(int32 BandIndex, bool bEnabled);
	void SetFrequency(int32 BandIndex, float InFrequency);
	void SetAmountDb(int32 BandIndex, float InAmountDb);
	void SetQuality(int32 BandIndex, float InQuality);
	void SetMakeupDb(int32 BandIndex, float InMakeupDb);

	bool  GetIsEnabled(int32 BandIndex);
	float GetFrequency(int32 BandIndex);
	float GetAmountDb(int32 BandIndex);
	float GetQuality(int32 BandIndex);
	float GetMakeupDb(int32 BandIndex);

	float GetResponseAvgDb(int32 BandIndex);

	bool GetIsEnabled() const { return bIsEnabled; }
	void SetIsEnabled(bool bNewValue) { bIsEnabled = bNewValue; }

	int32 GetSelectedBandIndex() const { return SelectedBandIndex; }
	void  SetSelectedBandIndex(int32 InBandIndex);

	float Dtft(float InFrequency) const;
	float DtftDb(float InFrequency) const;

	FEarPin* PullDataContext() const;
	void     PushDataContext();

	DECLARE_EVENT_OneParam(FVM_ClathriQ, FPinIndexChanged, int32)
	FPinIndexChanged& GetEvent_PinIndexChanged() { return PinIndexChanged; }

	DECLARE_EVENT_OneParam(FVM_ClathriQ, FBandSelectionChanged, int32)
	FBandSelectionChanged& GetEvent_BandSelectionChanged() { return BandSelectionChanged; }

	DECLARE_EVENT_OneParam(FVM_ClathriQ, FBandChanging, int32);
	FBandChanging& GetEvent_BandChanging() { return BandChanging; }

	DECLARE_EVENT_OneParam(FVM_ClathriQ, FBandChanged, int32);
	FBandChanged& GetEvent_BandChanged() { return BandChanged; }

private:
	void Recalculate();

	TWeakObjectPtr<UClathrispaceSettings> Settings = nullptr;
	int32                                 PinIndex = INDEX_NONE;

	FPinIndexChanged      PinIndexChanged;	
	FBandSelectionChanged BandSelectionChanged;
	FBandChanging         BandChanging;
	FBandChanged          BandChanged;

	FDelegateHandle BandChangedDelegateHandle;

	bool  bIsEnabled        = true;
	float SampleRate        = 48000.f;
	int32 SelectedBandIndex = INDEX_NONE;

	TArray<float> Response;
	float ResponseAvgDb_Band1 = 0.f;
	float ResponseAvgDb_Band2 = 0.f;
	float ResponseAvgDb_Band3 = 0.f;
};

