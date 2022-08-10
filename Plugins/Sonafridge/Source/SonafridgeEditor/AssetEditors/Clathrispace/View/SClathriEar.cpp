// Sonafridge 2022

#include "SClathriEar.h"

#include "Sonafridge/Attenuator/Clathrispace.h"
#include "Sonafridge/Tools/MathTools.h"
#include "SonafridgeEditor/AssetEditors/Clathrispace/ClathriEarEditorPreview.h"
#include "SonafridgeEditor/AssetEditors/Clathrispace/ClathriEarPreviewScene.h"


TSharedRef<SEditorViewport> SClathriEar::GetViewportWidget()
{
	return SharedThis(this);
}

SClathriEar::SClathriEar()
	: PreviewScene(MakeShared<FClathriEarPreviewScene>(FPreviewScene::ConstructionValues()))
{
}

void SClathriEar::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct({});
	Settings = InArgs._Settings;

	if (UClathrispaceSettings* SettingsValue = Settings.Get())
	{
		PreviewScene->SetSettings(SettingsValue);
		ViewportClient->Settings = SettingsValue;

		FVector Location = SettingsValue->GetEarData().EarPositionL;
		MathTool::ReflectVectorY(Location);
		PreviewScene->HelmetComp->SetRelativeLocation(Location);
	}
}

TSharedPtr<FExtender> SClathriEar::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShared<FExtender>());
	return Result;
}

void SClathriEar::OnFloatingButtonClicked()
{
}

TSharedRef<FEditorViewportClient> SClathriEar::MakeEditorViewportClient()
{
	ViewportClient = MakeShared<FClathriEarViewportClient>
	(
		nullptr,
		PreviewScene.Get(),
		SharedThis(this)
	);

	ViewportClient->bSetListenerPosition = false;
	ViewportClient->ViewportType = LVT_Perspective;
	ViewportClient->SetRealtime(true);

	return ViewportClient.ToSharedRef();
}

TSharedPtr<SWidget> SClathriEar::MakeViewportToolbar()
{
	return SNew(SCommonEditorViewportToolbarBase, SharedThis(this)).Cursor(EMouseCursor::Default);
}

