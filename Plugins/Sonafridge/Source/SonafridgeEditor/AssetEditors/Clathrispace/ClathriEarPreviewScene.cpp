// Sonafridge 2022

#include "ClathriEarPreviewScene.h"

#include "ClathriEarEditorPreview.h"
#include "Sonafridge/Attenuator/Clathrispace.h"
#include "SonafridgeEditor/Tools/PrimitiveBakery.h"
#include "Sonafridge/Tools/HSRColor.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/LineBatchComponent.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Sonafridge/Tools/MathTools.h"

IMPLEMENT_HIT_PROXY(HEarPinProxy, HComponentVisProxy);

FClathriEarPreviewScene::FClathriEarPreviewScene(ConstructionValues CVS)
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
	HelmetComp = NewObject<UClathriEarHelmetComponent>();
	HelmetComp->SetStaticMesh(HelmetMesh);
	AddComponent(HelmetComp, FTransform::Identity);

	// Do not place it before AddComponent because it won't affect on. 
	HelmetComp->SetRelativeScale3D({ 1.f, -1.f, 1.f });
}

void FClathriEarPreviewScene::Init(UClathrispaceSettings* InSettings)
{
	Settings = InSettings;
	HelmetComp->Init(InSettings);
}

UClathriEarHelmetComponent::UClathriEarHelmetComponent()
{
	OrdinaryPinMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance
	(
		this,
		GEngine->EmissiveMeshMaterial
	);

	SelectedPinMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance
	(
		this,
		GEngine->EmissiveMeshMaterial
	);

	OrdinaryPinMaterial->SetVectorParameterValue("Color", HSR(.62f, 1.f, .15f));
	SelectedPinMaterial->SetVectorParameterValue("Color", HSR(.62f, 1.f, .5));
}

void UClathriEarHelmetComponent::Init(UClathrispaceSettings* InSettings)
{
	FVector Location = InSettings->GetEarData().EarPositionL;
	MathTool::ReflectVectorY(Location);
	SetRelativeLocation(Location);
}

FClathriEarVisualizer::FClathriEarVisualizer(const TWeakObjectPtr<UMaterialInstanceDynamic>& InOrdinaryPinMaterial,
                                             const TWeakObjectPtr<UMaterialInstanceDynamic>& InSelectedPinMaterial)
{
	OrdinaryPinMaterial = InOrdinaryPinMaterial;
	SelectedPinMaterial = InSelectedPinMaterial;
	SphereCake = PrimitiveBakery::BuildSphere(1.5f, 2);
}

void FClathriEarVisualizer::Draw(const FClathriEarViewportClient*  InViewportClient,
                                 const FSceneView*                 View,
                                 FPrimitiveDrawInterface*          PDI)
{
	UClathrispaceSettings* Settings = InViewportClient->GetSettings();

	if (IsValid(Settings))
	{
		FClathriEarData& EarData = Settings->GetEarData();

		if (InViewportClient->GetVisibleSide() == EVisibleSide::Left)
		{
			for (int32 Index = 0; Index < EarData.EarPinsL.Num(); ++Index)
			{
				const FEarPin Pin = EarData.EarPinsL[Index];
				const FVector EndPos = 25.f * Pin.Direction;

				const UMaterialInstanceDynamic* Material = Index == SelectedPinIndex
				                                           ? SelectedPinMaterial.Get()
				                                           : OrdinaryPinMaterial.Get();

				HEarPinProxy* DanglingProxy = new HEarPinProxy(Index);
				PDI->SetHitProxy(DanglingProxy);
				PDI->DrawLine(FVector::ZeroVector, EndPos, HSR(.62f, .667f, .5f), 0);
				PrimitiveBakery::DrawSphere(PDI,
											SphereCake,
											FTranslationMatrix(EndPos),
											Material,
											DanglingProxy->Id);
				PDI->SetHitProxy(nullptr);
			}
		}
		else
		{
			for (int32 Index = 0; Index < EarData.EarPinsR.Num(); ++Index)
			{
				const FEarPin Pin = EarData.EarPinsR[Index];
				const FVector EndPos = 25.f * Pin.Direction;

				const UMaterialInstanceDynamic* Material = Index == SelectedPinIndex
				                                           ? SelectedPinMaterial.Get()
				                                           : OrdinaryPinMaterial.Get();

				HEarPinProxy* DanglingProxy = new HEarPinProxy(Index);
				PDI->SetHitProxy(DanglingProxy);
				PDI->DrawLine(FVector::ZeroVector, EndPos, HSR(.62f, .667f, .5f), 0);
				PrimitiveBakery::DrawSphere(PDI,
				                            SphereCake,
				                            FTranslationMatrix(EndPos),
				                            Material,
				                            DanglingProxy->Id);
				PDI->SetHitProxy(nullptr);
			}
		}
	}
}

bool FClathriEarVisualizer::ProcessClick(FClathriEarViewportClient* InViewportClient,
                                         HHitProxy*                 HitProxy,
                                         const FViewportClick&      Click)
{
	if (HitProxy)
	{
		if (const HEarPinProxy* EarPinProxy = HitProxyCast<HEarPinProxy>(HitProxy))
		{
			SelectedPinIndex = EarPinProxy->PinIndex;
		}
		else
		{
			SelectedPinIndex = INDEX_NONE;
		}

		InViewportClient->GetEvent_PinIndexChanged().Broadcast(SelectedPinIndex);

		return true;
	}

	SelectedPinIndex = INDEX_NONE;
	InViewportClient->GetEvent_PinIndexChanged().Broadcast(SelectedPinIndex);

	return false;
}

bool FClathriEarVisualizer::HandleInputDelta(const FClathriEarViewportClient* InViewportClient,
                                             FViewport*                 InViewport,
                                             FVector&                   DeltaTranslate,
                                             FRotator&                  DeltaRotate,
                                             FVector&                   DeltaScale)
{
	bool bHandled = false;

	if (SelectedPinIndex != INDEX_NONE)
	{
		if (UClathrispaceSettings* Settings = InViewportClient->GetSettings())
		{
			FClathriEarData& EarData = Settings->GetEarData();

			if (InViewportClient->GetVisibleSide() == EVisibleSide::Left)
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

			if (!DeltaRotate.IsNearlyZero())
			{
				if (Settings->MarkPackageDirty())
				{
					UE_LOG(LogSonafridgeEditor, Error, TEXT("FClathriEarVisualizer::HandleInputDelta: Could not mark package dirty. "));
				}
			}
		}

		bHandled = true;
	}

	return bHandled;
}

bool FClathriEarVisualizer::HandleInputKey(const FClathriEarViewportClient* InViewportClient,
										   FViewport*  Viewport,
                                           int32       ControllerId,
                                           FKey        Key,
                                           EInputEvent Event,
                                           float       AmountDepressed,
                                           bool        bGamepad)
{
	if (Key == EKeys::Delete)
	{
		if (SelectedPinIndex != INDEX_NONE)
		{
			if (UClathrispaceSettings* Settings = InViewportClient->GetSettings())
			{
				TArray<FEarPin>& LeftPins = Settings->GetEarData().EarPinsL;
				if (LeftPins.Num() > SelectedPinIndex)
				{
					LeftPins.RemoveAt(SelectedPinIndex);
				}

				TArray<FEarPin>& RightPins = Settings->GetEarData().EarPinsR;
				if (RightPins.Num() > SelectedPinIndex)
				{
					RightPins.RemoveAt(SelectedPinIndex);
				}

				SelectedPinIndex = INDEX_NONE;

				if (Settings->MarkPackageDirty())
				{
					UE_LOG(LogSonafridgeEditor, Error, TEXT("FClathriEarVisualizer::HandleInputKey: Could not mark package dirty. "));
				}

				return true;
			}
		}
	}

	return false;
}

