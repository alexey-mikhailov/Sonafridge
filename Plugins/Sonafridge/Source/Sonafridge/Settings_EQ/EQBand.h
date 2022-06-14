#pragma once

#include "CoreMinimal.h"
#include "EQBand.generated.h"

UENUM()
enum class EBandType
{
	None,
	Lowpass,
	LowPass_Butterworth,
	Highpass,
	HighPass_Butterworth,
	LowShelf,
	HighShelf,
	Bandpass,
	Bandcut,
	Notch,
	AllPass,
};

USTRUCT(BlueprintType)
struct SONAFRIDGE_API FEQBand
{
	GENERATED_BODY()

	FEQBand() {}
	virtual ~FEQBand() = default;

	void Init(float InSampleRate);
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

protected:
	virtual void Recalculate();

	EBandType Type = EBandType::None;
	float Frequency = 1000.f;
	float Quality = 1.f;
	float AmountDb = 0.f;
	float LoudCompDb = 0.f;
	float LoudCompCoef = 0.f;
	
	bool bIsInitialized = false;
	bool bIsEnabled = true;
	float SampleRate = 0.f;
	float Omega = 0.f;

	float Alpha = 0.f;
	float A0 = 0.f, A1 = 0.f, A2 = 0.f, B0 = 0.f, B1 = 0.f, B2 = 0.f;

private:
	FORCEINLINE void LogIfUninitialized() const
	{
		if (!bIsInitialized)
		{
			UE_LOG(LogTemp, Error, TEXT("EQ band is not initialized. "));
		}
	}
};

USTRUCT(BlueprintType)
struct SONAFRIDGE_API FEQBand_BandCut : public FEQBand
{
	GENERATED_BODY()

	FEQBand_BandCut();

	virtual void Recalculate() override;
};


