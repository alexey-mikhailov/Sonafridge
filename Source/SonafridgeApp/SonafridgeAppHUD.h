// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SonafridgeAppHUD.generated.h"

UCLASS()
class ASonafridgeAppHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASonafridgeAppHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

