// Sonafridge 2022

#include "Sonafridge.h"
#include "SonafridgeCommon.h"
#include "Attenuator/Clathrispace.h"

#define LOCTEXT_NAMESPACE "FSonafridgeModule"

DEFINE_LOG_CATEGORY(LogSonafridge);

void FSonafridgeModule::StartupModule()
{
	ClathrispaceFactory = MakeShared<FClathrispaceFactory>();
	IModularFeatures::Get().RegisterModularFeature(FClathrispaceFactory::GetModularFeatureName(), ClathrispaceFactory.Get());
}

void FSonafridgeModule::ShutdownModule()
{
	IModularFeatures::Get().UnregisterModularFeature(FClathrispaceFactory::GetModularFeatureName(), ClathrispaceFactory.Get());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSonafridgeModule, Sonafridge)