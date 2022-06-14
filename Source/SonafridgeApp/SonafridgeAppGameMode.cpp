// Copyright Epic Games, Inc. All Rights Reserved.

#include "SonafridgeAppGameMode.h"
#include "SonafridgeAppHUD.h"
#include "SonafridgeAppCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASonafridgeAppGameMode::ASonafridgeAppGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASonafridgeAppHUD::StaticClass();
}
