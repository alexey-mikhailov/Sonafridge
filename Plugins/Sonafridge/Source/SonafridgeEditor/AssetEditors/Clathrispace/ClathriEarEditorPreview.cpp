// Sonafridge 2022

#include "ClathriEarEditorPreview.h"

#include "EditorModeManager.h"
#include "SonafridgeEditor/AssetEditors/Clathrispace/ClathriEarPreviewScene.h"
#include "Sonafridge/Attenuator/Clathrispace.h"
#include "Sonafridge/Tools/MathTools.h"
#include "Components/DirectionalLightComponent.h"
#include "Editor/UnrealEdEngine.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Selection.h"


FClathriEarViewportClient::FClathriEarViewportClient(FEditorModeTools*                InModeTools,
                                                     FPreviewScene*                   InPreviewScene,
                                                     const TWeakPtr<SEditorViewport>& InEditorViewportWidget)
	: FEditorViewportClient(InModeTools,
	                        InPreviewScene,
	                        InEditorViewportWidget)
{
	ClathriEarScene = static_cast<FClathriEarPreviewScene*>(InPreviewScene);

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

	Visualizer = MakeShared<FClathriEarVisualizer>();
}

void FClathriEarViewportClient::ProcessClick(FSceneView& View,
                                             HHitProxy*  HitProxy,
                                             FKey        Key,
                                             EInputEvent Event,
                                             uint32      HitX,
                                             uint32      HitY)
{
	SetWidgetMode(FWidget::WM_Rotate);

	if (ClathriEarScene && IsValid(ClathriEarScene->HelmetComp))
	{
		const FViewportClick Click(&View, this, Key, Event, HitX, HitY);
		Visualizer->ProcessClick(this, HitProxy, Click);
	}
}

bool FClathriEarViewportClient::InputKey(FViewport*  InViewport,
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
		bResult |= Visualizer->HandleInputKey(InViewport, ControllerId, Key, Event, AmountDepressed, bGamepad);
	}

	return bResult;
}

bool FClathriEarViewportClient::InputWidgetDelta(FViewport*      InViewport,
                                                 EAxisList::Type CurrentAxis,
                                                 FVector&        Drag,
                                                 FRotator&       Rot,
                                                 FVector&        Scale)
{
	if (ClathriEarScene && IsValid(ClathriEarScene->HelmetComp))
	{
		if (Widget->IsDragging())
		{
			return Visualizer->HandleInputDelta(this, InViewport, Drag, Rot, Scale);
		}
	}

	return false;
}

void FClathriEarViewportClient::MouseMove(FViewport* InViewport, int32 x, int32 y)
{
	FEditorViewportClient::MouseMove(InViewport, x, y);
}

void FClathriEarViewportClient::CapturedMouseMove(FViewport* InViewport,
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

void FClathriEarViewportClient::Draw(FViewport* InViewport, FCanvas* Canvas)
{
	FEditorViewportClient::Draw(InViewport, Canvas);
}

FVector FClathriEarViewportClient::GetWidgetLocation() const
{
	return FVector::ZeroVector;
}

void FClathriEarViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

	if (ClathriEarScene && IsValid(ClathriEarScene->HelmetComp))
	{
		Visualizer->Draw(ClathriEarScene->HelmetComp, View, PDI);
	}
}

void FClathriEarViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	const float WorldDelta = GetWorld()->DeltaTimeSeconds;
	if (WorldDelta > 0.f)
	{
		PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

bool FClathriEarViewportClient::ShouldOrbitCamera() const
{
	return true;
}

