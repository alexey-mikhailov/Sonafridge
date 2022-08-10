// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IAudioExtensionPlugin.h"
#include "Sonafridge/SignalProcessing/DSPCore/BiquadFilter.h"
#include "Clathrispace.generated.h"

class FClathrispace;
class FClathriEarPreviewScene;


USTRUCT(BlueprintType)
struct FEarPin
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction;

	// Low-shelf filter

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bBand1Enabled = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band1Frequency = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band1AmountDb = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band1Quality = .2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band1MakeupDb = 0.f;

	// Band-cut filter

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bBand2Enabled = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band2Frequency = 667.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band2AmountDb = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band2Quality = .2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band2MakeupDb = 0.f;

	// High-shelf filter

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bBand3Enabled = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band3Frequency = 4444.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band3AmountDb = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band3Quality = .2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Band3MakeupDb = 0.f;
};


USTRUCT(BlueprintType)
struct FClathriEarData
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector EarPositionL = { 0.f, -8.f, 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector EarPositionR = { 0.f, +8.f, 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEarPin> EarPinsL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEarPin> EarPinsR;
};


USTRUCT(BlueprintType)
struct FClathriMazeData
{
	GENERATED_BODY();

	// TODO: Add cellular grid data here. 
};


USTRUCT(BlueprintType)
struct FClathriumData
{
	GENERATED_BODY();

	float SendLevel;
};


UCLASS()
class SONAFRIDGE_API UClathrispaceSettings : public USpatializationPluginSourceSettingsBase
{
	GENERATED_BODY()
	
public:
	UClathrispaceSettings();

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Clathri Ear (Spatializer)", DisplayName = "Enabled")
	bool bSpatializationEnabled;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Clathri Maze (Occluder)", DisplayName = "Enabled")
	bool bOcclusionEnabled;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Clathrium (Reverb)", DisplayName="Enabled")
	bool bReverbEnabled;

	FClathriEarData&  GetEarData()  { return EarData; }
	FClathriMazeData& GetMazeData() { return MazeData; }
	FClathriumData&   GetRoomData() { return RoomData; }

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Clathri Ear (Spatializer)")
	void CopyLeftToRight();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Clathri Ear (Spatializer)")
	void CopyRightToLeft();

	DECLARE_EVENT(UClathrispaceSettings, FExternallyChanged);
	FExternallyChanged& GetEvent_ExternallyChanged() { return ExternallyChanged; }

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	friend FClathrispace;
	friend FClathriEarPreviewScene;
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Clathri Ear (Spatializer)")
	FClathriEarData EarData;

	friend FClathrispace;
	friend FClathriEarPreviewScene;
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Clathri Ear (Spatializer)")
	FClathriMazeData MazeData;

	friend FClathrispace;
	friend FClathriEarPreviewScene;
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Clathri Ear (Spatializer)")
	FClathriumData RoomData;

	FExternallyChanged ExternallyChanged;
};


class FClathrispaceFactory : public IAudioSpatializationFactory
{
public:
	virtual FString GetDisplayName() override
	{
		static FString DisplayName = FString(TEXT("Clathrispace Plugin"));
		return DisplayName;
	}

	virtual bool SupportsPlatform(const FString& PlatformName) override
	{
		return PlatformName == FString(TEXT("Windows"));
	}

	virtual UClass* GetCustomSpatializationSettingsClass() const override { return UClathrispaceSettings::StaticClass(); }

	virtual TAudioSpatializationPtr CreateNewSpatializationPlugin(FAudioDevice* OwningDevice) override;
};


class FClathrispace final : public IAudioSpatialization
{
public:
	FClathrispace();
	virtual ~FClathrispace() override {}

	virtual void Initialize(const FAudioPluginInitializationParams Params) override;

	virtual void OnInitSource(const uint32 SourceId,
	                          const FName& AudioComponentUserId,
	                          USpatializationPluginSourceSettingsBase* InSettings) override;

	virtual void OnReleaseSource(const uint32 SourceId) override;

	virtual bool IsSpatializationEffectInitialized() const override;

	virtual void ProcessAudio(const FAudioPluginSourceInputData& InputData,
	                          FAudioPluginSourceOutputData& OutputData) override;

private:
	bool   bInitialized = false;
	uint32 NumberOfOutputs = 2;
	uint32 SampleRate = {};
	uint32 BufferLength = {};

	Sonafridge::FBiquadFilter FilterL;
	Sonafridge::FBiquadFilter FilterR;

	TWeakObjectPtr<UClathrispaceSettings> Settings;
};

