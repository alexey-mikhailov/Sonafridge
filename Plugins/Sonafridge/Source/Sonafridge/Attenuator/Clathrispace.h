// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "ClathrispaceSettingsProxy.h"
#include "IAudioExtensionPlugin.h"
#include "Sonafridge/SignalProcessing/DSPCore/DelayBufferMixer.h"
#include "Sonafridge/SignalProcessing/DSPCore/BiquadBufferMixer.h"

using namespace Sonafridge;

class FClathrispace;
class FClathriEarPreviewScene;
class FClathriEarStatDrawer;


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


struct FClathrispaceSource
{
	int32 SourceId;
	FDelayBufferMixer  Delay;
	FBiquadBufferMixer FilterL[ClathriEar::BandCount]{};
	FBiquadBufferMixer FilterR[ClathriEar::BandCount]{};
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
	void RecalculateEar(FClathrispaceSource* InSource, const FVector& InEmitterNormal);

	bool    bInitialized = false;
	uint32  NumDstChannels = 2;
	uint32  SampleRate = {};
	uint32  NumSamples = {};
	FVector PreviousEmitterDirection = FVector::ZeroVector;

	TMap<uint32, FClathrispaceSource> Sources;

	TSharedPtr<FClathrispaceSettingsProxy> Settings;

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

