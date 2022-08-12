// Sonafridge 2022

#pragma once

#include "ContentBrowserDelegates.h"

class FSonafridgeContentBrowserMenuExtender final
{
	FContentBrowserMenuExtender_SelectedAssets MenuExtenderForSelectedSonaQs;

public:
	void Startup();
	void Shutdown();

private:
	TSharedRef<FExtender> OnExtendAssetMenu(const TArray<FAssetData>& SelectedAssets);
	void                  OnAssetExtended(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets);
	void                  AddMenuEntryForSonaQs(FMenuBuilder& MenuBuilder, const FAssetData& Asset);
	void                  OnAssetEditorRequestedOpen(UObject* InObject);

	FDelegateHandle AssetEditorRequestedOpenDelegateHandle;
};
