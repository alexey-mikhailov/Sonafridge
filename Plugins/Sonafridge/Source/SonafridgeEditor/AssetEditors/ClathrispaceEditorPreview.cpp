// Sonafridge 2022

#include "ClathrispaceEditorPreview.h"

#include "EditorModeManager.h"
#include "SonafridgeEditor/AssetEditors/ClathrispacePreviewScene.h"
#include "Sonafridge/Attenuator/Clathrispace.h"
#include "Sonafridge/Tools/MathTools.h"
#include "Components/DirectionalLightComponent.h"
#include "Editor/UnrealEdEngine.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealEdGlobals.h"
#include "Engine/Selection.h"


FClathrispaceViewportClient::FClathrispaceViewportClient(FEditorModeTools* InModeTools,
                                                         FPreviewScene*    InPreviewScene,
                                                         const TWeakPtr<SEditorViewport>& InEditorViewportWidget)
	: FEditorViewportClient(InModeTools,
	                        InPreviewScene,
	                        InEditorViewportWidget)
{
	ClathriEarScene = static_cast<FClathrispacePreviewScene*>(InPreviewScene);

	SetViewLocation(FVector::ZeroVector);
	SetViewRotation(FRotator(-45.0f, 135.0f, 0.0f));
	SetViewLocationForOrbiting(FVector::ZeroVector, 45.f);
	bSetListenerPosition = false;
	EngineShowFlags.DisableAdvancedFeatures();
	EngineShowFlags.SetLighting(true);
	EngineShowFlags.SetIndirectLightingCache(true);
	EngineShowFlags.SetPostProcessing(false);

	// Setup defaults for the common draw helper.
	DrawHelper.bDrawPivot = false;
	DrawHelper.bDrawWorldBox = false;
	DrawHelper.bDrawKillZ = false;
	DrawHelper.bDrawGrid = false;
	DrawHelper.GridColorAxis = FColor(80, 80, 80);
	DrawHelper.GridColorMajor = FColor(72, 72, 72);
	DrawHelper.GridColorMinor = FColor(64, 64, 64);
	DrawHelper.PerspectiveGridSize = HALF_WORLD_MAX1;
}

void FClathrispaceViewportClient::ProcessClick(FSceneView& View,
                                               HHitProxy*  HitProxy,
                                               FKey        Key,
                                               EInputEvent Event,
                                               uint32      HitX,
                                               uint32      HitY)
{
	SetWidgetMode(FWidget::WM_Rotate);

	if (ClathriEarScene && IsValid(ClathriEarScene->HelmetComp))
	{
		TSharedPtr<FHelmetVisualizer> Visualizer = ClathriEarScene->HelmetComp->GetVisualizer();
		const FViewportClick Click(&View, this, Key, Event, HitX, HitY);
		Visualizer->ProcessClick(this, HitProxy, Click);
	}
}

bool FClathrispaceViewportClient::InputKey(FViewport*  InViewport,
                                           int32       ControllerId,
                                           FKey        Key,
                                           EInputEvent Event,
                                           float       AmountDepressed,
                                           bool        bGamepad)
{
	bool bResult = FEditorViewportClient::InputKey(InViewport,
	                                               ControllerId,
	                                               Key,
	                                               Event,
	                                               AmountDepressed,
	                                               bGamepad);

	if (ClathriEarScene && IsValid(ClathriEarScene->HelmetComp))
	{
		TSharedPtr<FHelmetVisualizer> Visualizer = ClathriEarScene->HelmetComp->GetVisualizer();
		bResult |= Visualizer->HandleInputKey(InViewport, ControllerId, Key, Event, AmountDepressed, bGamepad);
	}

	return bResult;
}

bool FClathrispaceViewportClient::InputWidgetDelta(FViewport*      InViewport,
                                                   EAxisList::Type CurrentAxis,
                                                   FVector&        Drag,
                                                   FRotator&       Rot,
                                                   FVector&        Scale)
{
	if (ClathriEarScene && IsValid(ClathriEarScene->HelmetComp))
	{
		if (Widget->IsDragging())
		{
			TSharedPtr<FHelmetVisualizer> Visualizer = ClathriEarScene->HelmetComp->GetVisualizer();
			return Visualizer->HandleInputDelta(this, InViewport, Drag, Rot, Scale);
		}
	}

	return false;
}

void FClathrispaceViewportClient::MouseMove(FViewport* InViewport, int32 x, int32 y)
{
	FEditorViewportClient::MouseMove(InViewport, x, y);
}

void FClathrispaceViewportClient::CapturedMouseMove(FViewport* InViewport,
                                                    int32      InMouseX,
                                                    int32      InMouseY)
{
	FEditorViewportClient::CapturedMouseMove(InViewport, InMouseX, InMouseY);

	FVector ViewLocation = GetViewLocation();

	if (ClathriEarScene && IsValid(ClathriEarScene->HelmetComp))
	{
		EVisibleSide VisibleSide = ViewLocation.Y < 0.f
			                           ? EVisibleSide::Left
			                           : EVisibleSide::Right;

		FVector Location = VisibleSide == EVisibleSide::Left
			                   ? Settings->GetEarData().EarPositionL
			                   : Settings->GetEarData().EarPositionR;

		MathTool::ReflectVectorY(Location);

		float Scale = VisibleSide == EVisibleSide::Left ? -1.f : 1.f;
		ClathriEarScene->HelmetComp->SetRelativeLocation(Location);
		ClathriEarScene->HelmetComp->SetRelativeScale3D({ 1.f, Scale, 1.f });
		ClathriEarScene->HelmetComp->VisibleSide = VisibleSide;
	}
}

void FClathrispaceViewportClient::Draw(FViewport* InViewport, FCanvas* Canvas)
{
	FEditorViewportClient::Draw(InViewport, Canvas);
}

FVector FClathrispaceViewportClient::GetWidgetLocation() const
{
	return FVector::ZeroVector;
}

void FClathrispaceViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

	if (ClathriEarScene && IsValid(ClathriEarScene->HelmetComp))
	{
		const FName Name = UClathrispaceHelmetComponent::StaticClass()->GetFName();

		TSharedPtr<FHelmetVisualizer> Visualizer = ClathriEarScene->HelmetComp->GetVisualizer();
		Visualizer->Draw(ClathriEarScene->HelmetComp, View, PDI);
	}
}

void FClathrispaceViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	const float WorldDelta = GetWorld()->DeltaTimeSeconds;
	if (WorldDelta > 0.f)
	{
		PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

bool FClathrispaceViewportClient::ShouldOrbitCamera() const
{
	return true;
}

TSharedRef<SEditorViewport> SClathriEar::GetViewportWidget()
{
	return SharedThis(this);
}

SClathriEar::SClathriEar()
	: PreviewScene(MakeShared<FClathrispacePreviewScene>(FPreviewScene::ConstructionValues()))
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
	ViewportClient = MakeShared<FClathrispaceViewportClient>
	(
		nullptr,
		PreviewScene.Get(),
		SharedThis(this)
	);

	ViewportClient->bSetListenerPosition = false;
	ViewportClient->ViewportType         = LVT_Perspective;
	ViewportClient->SetRealtime(true);

	return ViewportClient.ToSharedRef();
}


