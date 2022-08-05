// Sonafridge 2022

#include "ClathrispacePreviewScene.h"

#include "ClathrispaceEditorPreview.h"
#include "Sonafridge/Attenuator/Clathrispace.h"
#include "Sonafridge/Tools/HSRColor.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/LineBatchComponent.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Tools/PrimitiveBakery.h"

IMPLEMENT_HIT_PROXY(HEarPinProxy, HComponentVisProxy);

FClathrispacePreviewScene::FClathrispacePreviewScene(ConstructionValues CVS)
{
	// World setting
	GetWorld()->GetWorldSettings()->NotifyBeginPlay();
	GetWorld()->GetWorldSettings()->NotifyMatchStarted();
	GetWorld()->GetWorldSettings()->SetActorHiddenInGame(false);
	GetWorld()->bBegunPlay = true;

	// Set light options 
	DirectionalLight->SetRelativeLocation(FVector(1024.f, -1024.f, 2048.f));
	DirectionalLight->SetRelativeRotation(FRotator{ -45.f, 135.f, 0.f });
	DirectionalLight->SetRelativeScale3D(FVector(15.f));
	DirectionalLight->LightSourceAngle = 180.f;

	SetLightBrightness(4.f);
	DirectionalLight->InvalidateLightingCache();
	DirectionalLight->RecreateRenderState_Concurrent();

	// Test head
	HelmetMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Sonafridge/Meshes/TestHead/TestHalfHead.TestHalfHead"), NULL, LOAD_None, NULL);
	HelmetComp = NewObject<UClathrispaceHelmetComponent>();
	HelmetComp->SetStaticMesh(HelmetMesh);
	AddComponent(HelmetComp, FTransform::Identity);

	// Do not place it before AddComponent because it won't affect on. 
	HelmetComp->SetRelativeScale3D({ 1.f, -1.f, 1.f });
}

void FClathrispacePreviewScene::SetSettings(UClathrispaceSettings* InSettings)
{
	Settings = InSettings;
	HelmetComp->Init(InSettings);
}

void UClathrispaceHelmetComponent::Init(UClathrispaceSettings* InSettings)
{
	Settings = InSettings;

	OrdinaryEarPinMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance
	(
		this,
		GEngine->EmissiveMeshMaterial
	);

	OrdinaryEarPinMaterial->SetVectorParameterValue("Color", HSR(.62f, 1.f, .15f));

	SelectedEarPinMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance
	(
		this,
		GEngine->EmissiveMeshMaterial
	);

	SelectedEarPinMaterial->SetVectorParameterValue("Color", HSR(.62f, 1.f, .5));
}

FHelmetVisualizer::FHelmetVisualizer()
{
	SphereCake = PrimitiveBakery::BuildSphere(1.5f, 2);
}

void FHelmetVisualizer::DrawVisualization(const UActorComponent*   Component,
                                          const FSceneView*        View,
                                          FPrimitiveDrawInterface* PDI)
{
	if (const UClathrispaceHelmetComponent* Helmet = Cast<UClathrispaceHelmetComponent>(Component))
	{
		UClathrispaceSettings* Settings = Helmet->GetSettings();
		FVector Origin = Helmet->GetComponentLocation();

		if (IsValid(Settings))
		{
			FClathriEarData& EarData = Settings->GetEarData();

			if (Helmet->VisibleSide == EVisibleSide::Left)
			{
				Origin += EarData.EarPositionL;

				for (int32 Index = 0; Index < EarData.EarPinsL.Num(); ++Index)
				{
					const FEarPin Pin = EarData.EarPinsL[Index];
					const FVector EndPos = Origin + 25.f * Pin.Direction;

					const UMaterialInstanceDynamic* Material = Index == SelectedPinIndex
															   ? Helmet->GetSelectedEarPinMaterial()
															   : Helmet->GetOrdinaryEarPinMaterial();

					PDI->SetHitProxy(new HEarPinProxy(Helmet, Index));
					PDI->DrawLine(Origin, EndPos, HSR(.62f, .667f, .5f), 0);
					PrimitiveBakery::DrawSphere(PDI, SphereCake, FTranslationMatrix(EndPos), Material);
					PDI->SetHitProxy(nullptr);
				}
			}
			else
			{
				Origin += EarData.EarPositionR;

				for (int32 Index = 0; Index < EarData.EarPinsR.Num(); ++Index)
				{
					const FEarPin Pin = EarData.EarPinsR[Index];
					const FVector EndPos = Origin  + 25.f * Pin.Direction;

					const UMaterialInstanceDynamic* Material = Index == SelectedPinIndex
						? Helmet->GetSelectedEarPinMaterial()
						: Helmet->GetOrdinaryEarPinMaterial();

					PDI->SetHitProxy(new HEarPinProxy(Helmet, Index));
					PDI->DrawLine(Origin, EndPos, HSR(.62f, .667f, .5f), 0);
					PrimitiveBakery::DrawSphere(PDI, SphereCake, FTranslationMatrix(EndPos), Material);
					PDI->SetHitProxy(nullptr);
				}
			}
		}

		HelmetComponent = const_cast<UClathrispaceHelmetComponent*>(Helmet);
	}
}

bool FHelmetVisualizer::VisProxyHandleClick(FEditorViewportClient* InViewportClient,
                                            HComponentVisProxy*    VisProxy,
                                            const FViewportClick&  Click)
{
	if (VisProxy && VisProxy->Component.IsValid())
	{
		if (const HEarPinProxy* EarPinProxy = static_cast<HEarPinProxy*>(VisProxy))
		{
			SelectedPinIndex = EarPinProxy->Index;
		}
		else
		{
			SelectedPinIndex = INDEX_NONE;
		}

		return true;
	}

	return false;
}

void FHelmetVisualizer::EndEditing()
{
	// It does not work when user clicked in empty space. 
	SelectedPinIndex = INDEX_NONE;
}

bool FHelmetVisualizer::HandleInputDelta(FEditorViewportClient* ViewportClient,
                                         FViewport*             Viewport,
                                         FVector&               DeltaTranslate,
                                         FRotator&              DeltaRotate,
                                         FVector&               DeltaScale)
{
	bool bHandled = false;

	if (IsValid(HelmetComponent) && SelectedPinIndex != INDEX_NONE)
	{
		if (UClathrispaceSettings* Settings = HelmetComponent->GetSettings())
		{
			FClathriEarData& EarData = Settings->GetEarData();

			if (HelmetComponent->VisibleSide == EVisibleSide::Left)
			{
				if (EarData.EarPinsL.Num() > SelectedPinIndex)
				{
					FRotator DeltaRotator(DeltaRotate.Pitch, DeltaRotate.Yaw, DeltaRotate.Roll);
					FVector NewDirection = DeltaRotator.RotateVector(EarData.EarPinsL[SelectedPinIndex].Direction);
					EarData.EarPinsL[SelectedPinIndex].Direction = NewDirection;
				}
				if (EarData.EarPinsR.Num() > SelectedPinIndex)
				{
					FRotator DeltaRotator(DeltaRotate.Pitch, -DeltaRotate.Yaw, -DeltaRotate.Roll);
					FVector NewDirection = DeltaRotator.RotateVector(EarData.EarPinsR[SelectedPinIndex].Direction);
					EarData.EarPinsR[SelectedPinIndex].Direction = NewDirection;
				}
			}
			else
			{
				if (EarData.EarPinsR.Num() > SelectedPinIndex)
				{
					FRotator DeltaRotator(DeltaRotate.Pitch, DeltaRotate.Yaw, DeltaRotate.Roll);
					FVector NewDirection = DeltaRotator.RotateVector(EarData.EarPinsR[SelectedPinIndex].Direction);
					EarData.EarPinsR[SelectedPinIndex].Direction = NewDirection;
				}
				if (EarData.EarPinsL.Num() > SelectedPinIndex)
				{
					FRotator DeltaRotator(DeltaRotate.Pitch, -DeltaRotate.Yaw, -DeltaRotate.Roll);
					FVector NewDirection = DeltaRotator.RotateVector(EarData.EarPinsL[SelectedPinIndex].Direction);
					EarData.EarPinsL[SelectedPinIndex].Direction = NewDirection;
				}
			}
		}

		bHandled = true;
	}

	return bHandled;
}

bool FHelmetVisualizer::GetWidgetLocation(const FEditorViewportClient* ViewportClient,
                                          FVector&                     OutLocation) const
{
	UActorComponent* EditedComponent = GetEditedComponent();

	if (IsValid(HelmetComponent) && 
		EditedComponent == HelmetComponent && 
		SelectedPinIndex != INDEX_NONE)
	{
		OutLocation = FVector::ZeroVector;
		return true;
	}

	return false;
}

