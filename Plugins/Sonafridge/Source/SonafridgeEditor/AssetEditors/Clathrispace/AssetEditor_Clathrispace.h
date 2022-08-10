// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"

class UW_ClathriQ;
class UClathrispaceSettings;

/**
 * 
 */
class SONAFRIDGEEDITOR_API FAssetEditor_Clathrispace final : public FAssetEditorToolkit, public FGCObject
{
public:
	FAssetEditor_Clathrispace();
	virtual ~FAssetEditor_Clathrispace() override;
	void    Init(TSharedPtr<IToolkitHost> InToolkitHost, UClathrispaceSettings* InSettings);

	// FAssetEditorToolkit interface

	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FName        GetToolkitFName() const override;
	virtual FText        GetBaseToolkitName() const override;
	virtual FString      GetWorldCentricTabPrefix() const override;
	virtual void         RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void         UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	// FGCObject interface

	virtual void    AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;

private:
	TSharedRef<SDockTab> SpawnTab_Properties(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_ClathriQ(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_ClathriEar(const FSpawnTabArgs& Args);

	void OnPinIndexChanged(int32 NewPinIndex);

	// UObjects that referenced by this asset editor. 
	UClathrispaceSettings* ClathrispaceSettings = nullptr;
	UW_ClathriQ*           ClathrispaceWidget = nullptr;

	TSharedPtr<IDetailsView> DetailsView;

	TDelegate<void(int32)> PinIndexChanged;

	static const FName Identifier;
	static const FName ToolkitFName;
	static const FName ClathriEarTabId;
	static const FName ClathriQTabId;
	static const FName DetailsTabId;
};
