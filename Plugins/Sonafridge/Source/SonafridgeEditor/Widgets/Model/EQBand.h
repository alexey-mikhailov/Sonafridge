// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "EQBand.generated.h"

UENUM()
enum class EBandType : uint8
{
	None,
	LowCutFast,
	LowCutButterworth,
	HighCutFast,
	HighCutButterworth,
	LowShelf,
	HighShelf,
	BandPass,
	BandCut,
	Notch,
	AllPass,
};

class SONAFRIDGEEDITOR_API FEQBand final
{
	EBandType Type = EBandType::None;
	float Frequency = 1000.f;
	float Quality = 1.f;
	float AmountDb = 0.f;
	float LoudCompDb = 0.f;
	float LoudCompCoef = 0.f;
	
	bool bIsInitialized = false;
	bool bIsEnabled = true;
	float SampleRate = 0.f;

	float A0 = 0.f, A1 = 0.f, A2 = 0.f, B0 = 0.f, B1 = 0.f, B2 = 0.f;

public:
	FEQBand() {}
	~FEQBand() = default;

	void Init(float InSampleRate);
	void SetType(EBandType Value);
	void SetIsEnabled(bool bInValue);
	void SetFrequency(float Value);
	void SetQuality(float Value);
	void SetAmountDb(float Value);
	void SetLoudCompDb(float Value);

	/// Gives response coefficient at certain frequency. 1.f means no changes. 
	float Dtft(float InFrequency) const;

	EBandType GetType() const;
	bool      GetIsEnabled() const;
	float     GetFrequency() const;
	float     GetQuality() const;
	float     GetAmountDb() const;
	float     GetLoudCompDb() const;

private:
	void Recalculate();

	FORCEINLINE void LogIfUninitialized() const
	{
		if (!bIsInitialized)
		{
			UE_LOG(LogTemp, Error, TEXT("EQ band is not initialized. "));
		}
	}
};
