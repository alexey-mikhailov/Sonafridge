// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "IAudioExtensionPlugin.h"
#include "Sonafridge/SignalProcessing/DSPCore/BiquadBufferMixer.h"
#include "Clathrispace.generated.h"

using namespace Sonafridge;

class FClathrispace;
class FClathriEarPreviewScene;
class FClathriEarStatDrawer;


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
	static constexpr int32 BandCount = 3;

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

	DECLARE_EVENT(UClathrispaceSettings, FInternallyChanged);
	FInternallyChanged& GetEvent_InternallyChanged() { return InternallyChanged; }

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
	FInternallyChanged InternallyChanged;
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

	virtual UClass* GetCustomSpatializationSettingsClass() const override
	{
		return UClathrispaceSettings::StaticClass();
	}

	virtual int32                   GetMaxSupportedChannels() override { return 2; }
	virtual TAudioSpatializationPtr CreateNewSpatializationPlugin(FAudioDevice* OwningDevice) override;
};


class FClathrispace final : public IAudioSpatialization
{
public:
	FClathrispace();
	virtual ~FClathrispace() override;

	virtual void Initialize(const FAudioPluginInitializationParams Params) override;
	virtual void Shutdown() override;

	virtual void OnInitSource(const uint32                             SourceId,
	                          const FName&                             AudioComponentUserId,
	                          USpatializationPluginSourceSettingsBase* InSettings) override;

	virtual void OnReleaseSource(const uint32 SourceId) override;

	virtual bool IsSpatializationEffectInitialized() const override;

	virtual void ProcessAudio(const FAudioPluginSourceInputData& Src,
	                          FAudioPluginSourceOutputData& Dst) override;

protected:
	void OnAssetExternallyChanged();
	void OnAssetInternallyChanged();

private:
	void RecalculateEar(const FVector& InEmitterNormal);

	bool    bInitialized = false;
	uint32  NumDstChannels = 2;
	uint32  SampleRate = {};
	uint32  NumSamples = {};
	FVector PreviousEmitterDirection = FVector::ZeroVector;

	FBiquadBufferMixer FilterL[UClathrispaceSettings::BandCount] {};
	FBiquadBufferMixer FilterR[UClathrispaceSettings::BandCount] {};

	TWeakObjectPtr<UClathrispaceSettings> Settings;

	FDelegateHandle ExternallyChangedDelegateHandle;
	FDelegateHandle InternallyChangedDelegateHandle;

	TSharedPtr<FClathriEarStatDrawer> StatDrawer;

	friend FClathriEarStatDrawer;
	int32 ClathriEarStatIdxL1{INDEX_NONE},
	      ClathriEarStatIdxL2{INDEX_NONE},
	      ClathriEarStatIdxL3{INDEX_NONE};

	friend FClathriEarStatDrawer;
	int32 ClathriEarStatIdxR1{INDEX_NONE},
	      ClathriEarStatIdxR2{INDEX_NONE},
	      ClathriEarStatIdxR3{INDEX_NONE};

	friend FClathriEarStatDrawer;
	FVector ClathriEarStatWeightsL;

	friend FClathriEarStatDrawer;
	FVector ClathriEarStatWeightsR;
};

