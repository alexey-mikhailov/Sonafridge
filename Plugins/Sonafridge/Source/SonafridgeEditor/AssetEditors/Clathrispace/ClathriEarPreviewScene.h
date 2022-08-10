// Sonafridge 2022

#pragma once

#include "SonafridgeEditor/Tools/PrimitiveBakery.h"
#include "ComponentVisualizer.h"
#include "ClathriEarPreviewScene.generated.h"

class FClathriEarViewportClient;
class UClathrispaceSettings;
class UClathriEarHelmetComponent;


enum class EVisibleSide
{
	Left,
	Right
};


class FClathriEarPreviewScene final : public FPreviewScene
{
public:
	explicit FClathriEarPreviewScene(ConstructionValues CVS);
	void     Init(UClathrispaceSettings* InSettings);

	UStaticMesh*                HelmetMesh = nullptr;
	UClathriEarHelmetComponent* HelmetComp = nullptr;

private:
	// No need to pin it. Life time of Settings is longer.
	UClathrispaceSettings* Settings = nullptr;
};


struct HEarPinProxy : HHitProxy
{
	DECLARE_HIT_PROXY();

	HEarPinProxy(int32 InPinIndex) : PinIndex(InPinIndex)
	{
	}

	int32 PinIndex;
};


class FClathriEarVisualizer
{
public:
	FClathriEarVisualizer(const TWeakObjectPtr<UMaterialInstanceDynamic>& InOrdinaryPinMaterial, 
						  const TWeakObjectPtr<UMaterialInstanceDynamic>& InSelectedPinMaterial);

	void Draw(const FClathriEarViewportClient*  InViewportClient,
	          const FSceneView*                 View,
	          FPrimitiveDrawInterface*          PDI);

	bool ProcessClick(FClathriEarViewportClient* InViewportClient,
	                  HHitProxy*             HitProxy,
	                  const FViewportClick&  Click);

	bool HandleInputDelta(const FClathriEarViewportClient* InViewportClient,
	                      FViewport*             InViewport,
	                      FVector&               DeltaTranslate,
	                      FRotator&              DeltaRotate,
	                      FVector&               DeltaScale);

	bool HandleInputKey(const FClathriEarViewportClient* InViewportClient,
						FViewport*  Viewport,
	                    int32       ControllerId,
	                    FKey        Key,
	                    EInputEvent Event,
	                    float       AmountDepressed,
	                    bool        bGamepad);

protected:
	TWeakObjectPtr<UMaterialInstanceDynamic> OrdinaryPinMaterial = nullptr;
	TWeakObjectPtr<UMaterialInstanceDynamic> SelectedPinMaterial = nullptr;

private:
	int32          SelectedPinIndex = INDEX_NONE;
	FPrimitiveCake SphereCake;
};


UCLASS(Blueprintable)
class SONAFRIDGEEDITOR_API UClathriEarHelmetComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UClathriEarHelmetComponent();
	void Init(UClathrispaceSettings* InSettings);

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* OrdinaryPinMaterial = nullptr;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* SelectedPinMaterial = nullptr;
};
