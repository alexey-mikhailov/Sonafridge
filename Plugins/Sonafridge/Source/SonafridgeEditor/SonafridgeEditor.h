// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "ContentBrowserMenuExtender.h"

class FToolBarBuilder;
class FMenuBuilder;
class FUICommandList;
class FAssetTypeActions_Base;

class FSonafridgeEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:
	void                 RegisterMenus();
	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);

	TSharedPtr<FUICommandList>                 PluginCommands;
	FSonafridgeContentBrowserMenuExtender      ContentBrowserMenuExtender;
	TArray<TSharedPtr<FAssetTypeActions_Base>> AssetActions;
};
