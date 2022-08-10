// Sonafridge 2022

#pragma once
#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"


class FClathrispacePreviewScene;
class FClathrispaceViewportClient;
class UClathrispaceSettings;


class SClathriEar : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SClathriEar) {}
		SLATE_ATTRIBUTE(UClathrispaceSettings*, Settings)
	SLATE_END_ARGS()

	SClathriEar();

	void Construct(const FArguments& InArgs);

	FORCEINLINE TSharedPtr<FClathrispaceViewportClient> GetOwnViewportClient() const { return ViewportClient; }

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
	virtual TSharedPtr<SWidget>               MakeViewportToolbar() override;

private:
	TSharedPtr<FClathrispaceViewportClient> ViewportClient;
	TSharedPtr<FClathrispacePreviewScene>   PreviewScene;

	TAttribute<UClathrispaceSettings*> Settings;
};

