// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"


class FClathrispacePreviewScene;
class SClathriEar;
class UClathrispaceSettings;


class FClathrispaceViewportClient final : public FEditorViewportClient
{
public:
	FClathrispaceViewportClient(FEditorModeTools*                InModeTools,
	                            FPreviewScene*                   InPreviewScene         = nullptr,
	                            const TWeakPtr<SEditorViewport>& InEditorViewportWidget = nullptr);

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

private:
	FClathrispacePreviewScene* ClathriEarScene = nullptr;

	friend SClathriEar;
	UClathrispaceSettings* Settings = nullptr;
};


class SClathriEar : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SClathriEar) {}

		SLATE_ATTRIBUTE(UClathrispaceSettings*, Settings)

	SLATE_END_ARGS()

	SClathriEar();

	void Construct(const FArguments& InArgs);

	// 
	// Toolbar interface
	// 

	virtual TSharedRef<SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;

protected:
	// 
	// SEditorViewport interface
	//

	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;

private:
	TSharedPtr<FClathrispaceViewportClient> ViewportClient;
	TSharedPtr<FClathrispacePreviewScene>   PreviewScene;

	TAttribute<UClathrispaceSettings*> Settings;
};
