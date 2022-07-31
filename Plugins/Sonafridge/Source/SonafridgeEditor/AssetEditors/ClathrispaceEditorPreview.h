// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"


class UHeadMeshComponent;


class FClathrispacePreviewScene final : public FPreviewScene
{
public:
	explicit FClathrispacePreviewScene(ConstructionValues CVS);

	UStaticMesh*        HeadMesh;
	UStaticMeshComponent* HeadMeshComp;
};


class FClathrispaceViewportClient final : public FEditorViewportClient
{
public:
	FClathrispaceViewportClient(FEditorModeTools*                InModeTools,
	                            FPreviewScene*                   InPreviewScene         = nullptr,
	                            const TWeakPtr<SEditorViewport>& InEditorViewportWidget = nullptr);

	virtual void Activated(FViewport* InViewport, const FWindowActivateEvent& InActivateEvent) override;
	virtual void ReceivedFocus(FViewport* InViewport) override;
	virtual bool ShouldOrbitCamera() const override;
	virtual void MouseMove(FViewport* Viewport, int32 x, int32 y) override;
	virtual void CapturedMouseMove(FViewport* InViewport, int32 InMouseX, int32 InMouseY) override;

	virtual void Draw(FViewport* InViewport, FCanvas* Canvas) override;

	virtual void Tick(float DeltaSeconds) override;

private:
	FClathrispacePreviewScene* ClathriEarScene;
};


class SClathrispaceViewport : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SClathrispaceViewport) {}
	SLATE_END_ARGS()

	SClathrispaceViewport();

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
	TSharedPtr<FEditorViewportClient> LevelViewportClient;

	/** The scene for this viewport. */
	TSharedPtr<FClathrispacePreviewScene> PreviewScene;
};
