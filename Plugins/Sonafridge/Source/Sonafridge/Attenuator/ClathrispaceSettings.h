// Sonafridge 2022

#pragma once
#include "ClathrispaceSettings.generated.h"

class FClathrispace;
class FClathriEarPreviewScene;

namespace ClathriEar
{
	static constexpr int32 BandCount = 3;
}


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
	FVector EarPosition = { 0.f, +8.f, 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEarPin> EarPins;
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

	DECLARE_EVENT(UClathrispaceSettings, FExternallyChanged);
	FExternallyChanged& GetEvent_ExternallyChanged() { return ExternallyChanged; }

	DECLARE_EVENT(UClathrispaceSettings, FInternallyChanged);
	FInternallyChanged& GetEvent_InternallyChanged() { return InternallyChanged; }

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Clathri Ear (Spatializer)")
	FClathriEarData EarData;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Clathri Ear (Spatializer)")
	FClathriMazeData MazeData;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Clathri Ear (Spatializer)")
	FClathriumData RoomData;

	FExternallyChanged ExternallyChanged;
	FInternallyChanged InternallyChanged;
};


