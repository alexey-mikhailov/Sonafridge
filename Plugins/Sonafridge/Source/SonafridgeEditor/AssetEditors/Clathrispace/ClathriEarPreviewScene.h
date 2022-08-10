// Sonafridge 2022

#pragma once

#include "SonafridgeEditor/Tools/PrimitiveBakery.h"
#include "ComponentVisualizer.h"
#include "ClathriEarPreviewScene.generated.h"

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
	void     SetSettings(UClathrispaceSettings* InSettings);

	UStaticMesh*                  HelmetMesh     = nullptr;
	UClathriEarHelmetComponent* HelmetComp     = nullptr;

private:
	// No need to pin it. Life time of Settings is longer.
	UClathrispaceSettings* Settings = nullptr;
};


struct HEarPinProxy : HComponentVisProxy
{
	DECLARE_HIT_PROXY();

	HEarPinProxy(const UActorComponent* InComponent, int32 InTargetIndex)
	    : HComponentVisProxy(InComponent)
	    , Index(InTargetIndex)
	{
	}

	int32 Index;
};


class FClathriEarVisualizer
{
public:
	FClathriEarVisualizer();

	void Draw(const UActorComponent*   Component,
	          const FSceneView*        View,
	          FPrimitiveDrawInterface* PDI);

	bool ProcessClick(FEditorViewportClient* InViewportClient,
	                  HHitProxy*             HitProxy,
	                  const FViewportClick&  Click);

	bool HandleInputDelta(FEditorViewportClient* ViewportClient,
	                      FViewport*             Viewport,
	                      FVector&               DeltaTranslate,
	                      FRotator&              DeltaRotate,
	                      FVector&               DeltaScale);

	bool HandleInputKey(FViewport*  Viewport,
	                    int32       ControllerId,
	                    FKey        Key,
	                    EInputEvent Event,
	                    float       AmountDepressed,
	                    bool        bGamepad);

protected:
	UClathriEarHelmetComponent* HelmetComponent = nullptr;

private:
	int32          SelectedPinIndex = INDEX_NONE;
	FPrimitiveCake SphereCake;
};


UCLASS(Blueprintable)
class SONAFRIDGEEDITOR_API UClathriEarHelmetComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	void Init(UClathrispaceSettings* InSettings);
	UClathrispaceSettings* GetSettings() const { return Settings; }

	UMaterialInstanceDynamic* GetOrdinaryEarPinMaterial() const { return OrdinaryEarPinMaterial; }
	UMaterialInstanceDynamic* GetSelectedEarPinMaterial() const { return SelectedEarPinMaterial; }

	EVisibleSide VisibleSide = EVisibleSide::Left;

private:
	UPROPERTY()
	UClathrispaceSettings* Settings = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* OrdinaryEarPinMaterial = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* SelectedEarPinMaterial = nullptr;
};
