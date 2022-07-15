// Sonafridge 2022

#pragma once

#include "Sonafridge/SonafridgeCommon.h"
#include "CoreMinimal.h"


class SONAFRIDGEEDITOR_API FVM_SonaQBand final
{
	float SampleRate = 0.f;

	EEQBandType Type = EEQBandType::None;
	float Frequency = 1000.f;
	float Quality = 1.f;
	float AmountDb = 0.f;
	float MakeupDb = 0.f;
	float MakeupCoeff = 0.f;
	
	float A0 = 0.f, A1 = 0.f, A2 = 0.f, B0 = 0.f, B1 = 0.f, B2 = 0.f;

	bool bIsInitialized = false;
	bool bIsEnabled = true;

public:
	FVM_SonaQBand() {}
	~FVM_SonaQBand() = default;

	void Init(float InSampleRate);
	void SetType(EEQBandType Value);
	void SetIsEnabled(bool bInValue);
	void SetFrequency(float Value);
	void SetQuality(float Value);
	void SetAmountDb(float Value);
	void SetMakeupDb(float Value);
	void SetMakeupCoeff(float Value);

	/// Gives response coefficient at certain frequency. 1.f means no changes. 
	float Dtft(float InFrequency) const;

	EEQBandType GetType() const;
	bool        GetIsEnabled() const;
	float       GetFrequency() const;
	float       GetQuality() const;
	float       GetAmountDb() const;
	float       GetMakeupDb() const;

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
