// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"

class FClathrispaceFactory;
class FToolBarBuilder;
class FMenuBuilder;

class FSonafridgeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FClathrispaceFactory> ClathrispaceFactory;
};
