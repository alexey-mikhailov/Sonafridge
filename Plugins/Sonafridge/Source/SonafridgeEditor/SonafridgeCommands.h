// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SonafridgeStyle.h"

class FSonafridgeCommands : public TCommands<FSonafridgeCommands>
{
public:

	FSonafridgeCommands()
		: TCommands<FSonafridgeCommands>(TEXT("Sonafridge"), NSLOCTEXT("Contexts", "Sonafridge", "Sonafridge Plugin"), NAME_None, FSonafridgeStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};