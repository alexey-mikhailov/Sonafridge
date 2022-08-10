// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "ClathriEarPreviewScene.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"


class FClathriEarVisualizer;
class FClathriEarPreviewScene;
class SClathriEar;
class UClathrispaceSettings;


class FClathriEarViewportClient final : public FEditorViewportClient
{
public:
	FClathriEarViewportClient(FEditorModeTools*                InModeTools,
	                          FPreviewScene*                   InPreviewScene = nullptr,
	                          const TWeakPtr<SEditorViewport>& InEditorViewportWidget = nullptr);

	void Init(UClathrispaceSettings* InSettings);

	UClathrispaceSettings* GetSettings() const { return Settings; }
	EVisibleSide GetVisibleSide() const { return VisibleSide; }

	//
	// FViewportClient interface
	// 

	virtual void ProcessClick(FSceneView& View,
	                          HHitProxy*  HitProxy,
	                          FKey        Key,
	                          EInputEvent Event,
	                          uint32      HitX,
	                          uint32      HitY) override;

	virtual bool InputKey(FViewport* Viewport,
	                      int32 ControllerId,
	                      FKey Key,
	                      EInputEvent Event,
	                      float AmountDepressed,
	                      bool bGamepad) override;

	virtual bool InputWidgetDelta(FViewport*      InViewport,
	                              EAxisList::Type CurrentAxis,
	                              FVector&        Drag,
	                              FRotator&       Rot,
	                              FVector&        Scale) override;

	virtual void MouseMove(FViewport* Viewport, int32 x, int32 y) override;
	virtual void CapturedMouseMove(FViewport* InViewport, int32 InMouseX, int32 InMouseY) override;
	virtual void Draw(FViewport* InViewport, FCanvas* Canvas) override;

	virtual FVector GetWidgetLocation() const override;

	//
	// FViewElementDrawer interface
	//

	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;

	//
	// FEditorViewportClient interface
	//

	virtual void Tick(float DeltaSeconds) override;
	virtual bool ShouldOrbitCamera() const override;

	DECLARE_EVENT_OneParam(FClathrispaceViewportClient, FPinIndexChanged, int32)


	FPinIndexChanged& GetEvent_PinIndexChanged() { return PinIndexChanged; }

private:
	UClathrispaceSettings*            Settings = nullptr;
	FClathriEarPreviewScene*          ClathriEarScene = nullptr;
	TSharedPtr<FClathriEarVisualizer> Visualizer;

	FPinIndexChanged PinIndexChanged;
	EVisibleSide     VisibleSide = EVisibleSide::Left;
};

