// Sonafridge 2022

#include "SonafridgeCommands.h"

#define LOCTEXT_NAMESPACE "FSonafridgeModule"

void FSonafridgeCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Sonafridge", "Bring up Sonafridge window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
