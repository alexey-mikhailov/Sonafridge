// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundEffectSource.h"
#include "DSPCore/BiquadFilter.h"
#include "Sonafridge/SonafridgeCommon.h"
#include "SonaMacroConvention.h"
#include "SFX_SonaQ.generated.h"

class FVM_SonaQ;

namespace Sonafridge
{
	class FBiquadFilter;
}

USTRUCT(BlueprintType)
struct SONAFRIDGE_API FSFXSettings_SonaQBand
{
	GENERATED_USTRUCT_BODY()

	// Whether or not the band is enabled. Allows changing bands on the fly.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SourceEffect|Preset")
	uint32 bEnabled : 1;

	// The type of the filter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SourceEffect|Preset")
	EEQBandType Type;

	// The cutoff frequency of the band
	UPROPERTY
	(
		EditAnywhere, 
		BlueprintReadWrite, 
		Category = "SourceEffect|Preset", 
		meta = 
		(
			ClampMin = "20.0",
			ClampMax = "20000.0",
			UIMin = "20.0",
			UIMax = "20000.0"
		)
	)
	float Frequency;

	// The gain in decibels to apply to the eq band
	UPROPERTY
	(
		EditAnywhere, 
		BlueprintReadWrite, 
		Category = "SourceEffect|Preset", 
		meta = 
		(
			ClampMin = "-48.0",
			ClampMax = "+48.0",
			UIMin = "-48.0",
			UIMax = "+48.0", 
			EditCondition = "Type == EEQBandType::AttBand || Type == EEQBandType::AttLow || Type == EEQBandType::AttHigh"
		)
	)
	float AmountDb;

	// The bandwidth (in octaves) of the band
	UPROPERTY
	(
		EditAnywhere, 
		BlueprintReadWrite, 
		Category = "SourceEffect|Preset", 
		meta = 
		(
			ClampMin = "0.001",
			ClampMax = "1000.0",
			UIMin = "0.001",
			UIMax = "1000.0"
		)
	)
	float Quality;

	// The gain in decibels to apply to the eq band
	UPROPERTY
	(
		EditAnywhere, 
		BlueprintReadWrite, 
		Category = "SourceEffect|Preset", 
		meta = 
		(
			ClampMin = "-48.0",
			ClampMax = "+48.0",
			UIMin = "-48.0",
			UIMax = "+48.0"
		)
	)
	float MakeupDb;

	FSFXSettings_SonaQBand()
		: bEnabled(false)
		, Type(EEQBandType::AttBand)
		, Frequency(500.0f)
		, AmountDb(0.f)
		, Quality(1.0f)
		, MakeupDb(0.f)
	{
	}

	bool IsGainEnabled() const;
};


// ========================================================================
// FSFXSettings_SonaQ
// This is the source effect's setting struct. 
// ========================================================================

USTRUCT(BlueprintType)
struct SONAFRIDGE_API FSFXSettings_SonaQ
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabled = true;

	UPROPERTY
	(
		EditAnywhere, 
		BlueprintReadWrite, 
		Category = "SourceEffect|Preset", 
		meta = 
		(
			ClampMin = "-18", 
			ClampMax = "18.0", 
			UIMin = "-18.0", 
			UIMax = "18.0", 
			EditCondition="bEnabled"
		)
	)
	float OutputDb = 0.f;

	// The EQ bands to use
	UPROPERTY
	(
		EditAnywhere, 
		BlueprintReadWrite, 
		Category = "SourceEffect|Preset", 
		meta = (EditCondition="bEnabled")
	)
	TArray<FSFXSettings_SonaQBand> EQBands;
};

// ========================================================================
// FSFX_SonaQ
// This is the instance of the source effect. Performs DSP calculations.
// ========================================================================

class SONAFRIDGE_API FSFX_SonaQ final : public FSoundEffectSource
{
public:
	// Called on an audio effect at initialization on main thread before audio processing begins.
	virtual void Init(const FSoundEffectSourceInitData& InInitData) override;
	
	// Called when an audio effect preset is changed
	virtual void OnPresetChanged() override;

	// Process the input block of audio. Called on audio thread.
	virtual void ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData) override;

protected:
	bool bEnabled = true;
	float OutputGain = 1.f;

	// Bank of biquad filters
	TArray<Sonafridge::FBiquadFilter> Filters;
	float SampleRate = 0.f;
	int32 NumChannels = 0;
};

// ========================================================================
// USFXPreset_SonaQ
// This code exposes your preset settings and effect class to the editor.
// And allows for a handle to setting/updating effect settings dynamically.
// ========================================================================

UCLASS(ClassGroup = AudioSourceEffect, meta = (BlueprintSpawnableComponent))
class SONAFRIDGE_API USFXPreset_SonaQ : public USoundEffectSourcePreset
{
	GENERATED_BODY()

public:
	// Macro which declares and implements useful functions.
	SONAFX_PRESET_METHODS(SonaQ)

	// Allows you to customize the color of the preset in the editor.
	virtual FColor GetPresetColor() const override { return FColor(196.0f, 185.0f, 121.0f); }

	// Change settings of your effect from blueprints. Will broadcast changes to active instances.
	UFUNCTION(BlueprintCallable, Category = "Audio|Effects")
	void SetSettings(const FSFXSettings_SonaQ& InSettings);
	
	// The copy of the settings struct. Can't be written to in BP, but can be read.
	// Note that the value read in BP is the serialized settings, will not reflect dynamic changes made in BP.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio|Effects")
	FSFXSettings_SonaQ Settings;

	DECLARE_EVENT(UEW_SFX_SonaQ, FChanged);
	FChanged& GetEvent_Changed() { return Changed; }

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	FChanged Changed;
};
