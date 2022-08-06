// Sonafridge 2022

#pragma once

#include "SonafridgeEditor/Tools/PrimitiveBakery.h"
#include "ComponentVisualizer.h"
#include "ClathrispacePreviewScene.generated.h"

class UClathrispaceSettings;
class UClathrispaceHelmetComponent;


enum class EVisibleSide
{
	Left,
	Right
};


class FClathrispacePreviewScene final : public FPreviewScene
{
public:
	explicit FClathrispacePreviewScene(ConstructionValues CVS);
	void     SetSettings(UClathrispaceSettings* InSettings);

	UStaticMesh*                  HelmetMesh     = nullptr;
	UClathrispaceHelmetComponent* HelmetComp     = nullptr;

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


class FHelmetVisualizer
{
public:
	FHelmetVisualizer();

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
	UClathrispaceHelmetComponent* HelmetComponent = nullptr;

private:
	int32          SelectedPinIndex = INDEX_NONE;
	FPrimitiveCake SphereCake;
};


UCLASS(Blueprintable)
class SONAFRIDGEEDITOR_API UClathrispaceHelmetComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	void Init(UClathrispaceSettings* InSettings);
	UClathrispaceSettings* GetSettings() const { return Settings; }
	TSharedPtr<FHelmetVisualizer> GetVisualizer() const { return Visualizer;  }

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

	TSharedPtr<FHelmetVisualizer> Visualizer = MakeShared<FHelmetVisualizer>();
};
