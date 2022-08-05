// Sonafridge 2022

#pragma once

#include "ComponentVisualizer.h"
#include "Tools/PrimitiveBakery.h"
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


UCLASS(Blueprintable)
class SONAFRIDGEEDITOR_API UClathrispaceHelmetComponent : public UStaticMeshComponent
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


class FHelmetVisualizer : public FComponentVisualizer
{
public:
	FHelmetVisualizer();

	virtual void DrawVisualization(const UActorComponent*   Component,
	                               const FSceneView*        View,
	                               FPrimitiveDrawInterface* PDI) override;

	virtual bool VisProxyHandleClick(FEditorViewportClient* InViewportClient,
	                                 HComponentVisProxy*    VisProxy,
	                                 const FViewportClick&  Click) override;

	virtual void EndEditing() override;

	virtual bool HandleInputDelta(FEditorViewportClient* ViewportClient,
	                              FViewport*             Viewport,
	                              FVector&               DeltaTranslate,
	                              FRotator&              DeltaRotate,
	                              FVector&               DeltaScale) override;

	virtual bool GetWidgetLocation(const FEditorViewportClient* ViewportClient,
	                               FVector&                     OutLocation) const override;

	virtual UActorComponent* GetEditedComponent() const override { return HelmetComponent; }
	
protected:
	UClathrispaceHelmetComponent* HelmetComponent = nullptr;

private:
	int32          SelectedPinIndex = INDEX_NONE;
	FPrimitiveCake SphereCake;
};
