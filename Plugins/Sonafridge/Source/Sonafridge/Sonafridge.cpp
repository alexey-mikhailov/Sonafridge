// Sonafridge 2022

#include "Sonafridge.h"
#include "SonafridgeCommon.h"

#define LOCTEXT_NAMESPACE "FSonafridgeModule"

DEFINE_LOG_CATEGORY(LogSonafridge);

void FSonafridgeModule::StartupModule()
{
}

void FSonafridgeModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSonafridgeModule, Sonafridge)