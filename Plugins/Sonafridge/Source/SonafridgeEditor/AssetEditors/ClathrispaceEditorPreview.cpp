// Sonafridge 2022

#include "ClathrispaceEditorPreview.h"

#include "Components/DirectionalLightComponent.h"
#include "GameFramework/WorldSettings.h"

FClathrispacePreviewScene::FClathrispacePreviewScene(ConstructionValues CVS)
{
	// World setting
	GetWorld()->GetWorldSettings()->NotifyBeginPlay();
	GetWorld()->GetWorldSettings()->NotifyMatchStarted();
	GetWorld()->GetWorldSettings()->SetActorHiddenInGame(false);
	GetWorld()->bBegunPlay = true;

	// Set light options 
	DirectionalLight->SetRelativeLocation(FVector(1024.f, 1024.f, 2048.f));
	DirectionalLight->SetRelativeRotation(FRotator{ -45.f, -135.f, 0.f });
	DirectionalLight->SetRelativeScale3D(FVector(15.f));
	DirectionalLight->LightSourceAngle = 180.f;

	SetLightBrightness(4.f);
	DirectionalLight->InvalidateLightingCache();
	DirectionalLight->RecreateRenderState_Concurrent();

	// Test head
	HeadMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Sonafridge/Meshes/TestHead/TestHalfHead.TestHalfHead"), NULL, LOAD_None, NULL);
	HeadMeshComp = NewObject<UStaticMeshComponent>();
	HeadMeshComp->SetStaticMesh(HeadMesh);
	AddComponent(HeadMeshComp, FTransform::Identity);

	// Do not place it before AddComponent because it won't affect on. 
	HeadMeshComp->SetRelativeScale3D({ -1.f , 1.f, 1.f });
}

FClathrispaceViewportClient::FClathrispaceViewportClient(FEditorModeTools* InModeTools,
                                                         FPreviewScene*    InPreviewScene,
                                                         const TWeakPtr<SEditorViewport>& InEditorViewportWidget)
	: FEditorViewportClient(InModeTools,
	                        InPreviewScene,
	                        InEditorViewportWidget)
{
	ClathriEarScene = static_cast<FClathrispacePreviewScene*>(InPreviewScene);

	SetViewLocation(FVector::ZeroVector);
	SetViewRotation(FRotator(-45.0f, -135.0f, 0.0f));
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

	// Don't want to display the widget in this viewport
	Widget->SetDefaultVisibility(false);
}

void FClathrispaceViewportClient::Activated(FViewport*                  InViewport,
                                            const FWindowActivateEvent& InActivateEvent)
{
	FEditorViewportClient::Activated(Viewport, InActivateEvent);
}

void FClathrispaceViewportClient::ReceivedFocus(FViewport* InViewport)
{
	FEditorViewportClient::ReceivedFocus(InViewport);
}

bool FClathrispaceViewportClient::ShouldOrbitCamera() const
{
	return true;
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

	float ViewYaw = GetViewRotation().Yaw;
	ViewYaw = FMath::Frac(ViewYaw / 360.f);

	if (ClathriEarScene && IsValid(ClathriEarScene->HeadMeshComp))
	{
		float Scale = ViewYaw < 0.f || ViewYaw >= .5f ? -1.f : 1.f;
		ClathriEarScene->HeadMeshComp->SetRelativeScale3D({ Scale, 1.f, 1.f });
	}
}

void FClathrispaceViewportClient::Draw(FViewport* InViewport, FCanvas* Canvas)
{
	FEditorViewportClient::Draw(InViewport, Canvas);
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

TSharedRef<class SEditorViewport> SClathrispaceViewport::GetViewportWidget()
{
	return SharedThis(this);
}

SClathrispaceViewport::SClathrispaceViewport()
	: PreviewScene(MakeShared<FClathrispacePreviewScene>(FPreviewScene::ConstructionValues()))
{
}

void SClathrispaceViewport::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct({});
}

TSharedPtr<FExtender> SClathrispaceViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SClathrispaceViewport::OnFloatingButtonClicked()
{
}

TSharedRef<FEditorViewportClient> SClathrispaceViewport::MakeEditorViewportClient()
{
	LevelViewportClient = MakeShareable(new FClathrispaceViewportClient(nullptr, PreviewScene.Get(), SharedThis(this)));
	LevelViewportClient->bSetListenerPosition = false;
	LevelViewportClient->ViewportType = LVT_Perspective;
	LevelViewportClient->SetRealtime(true);

	return LevelViewportClient.ToSharedRef();
}


