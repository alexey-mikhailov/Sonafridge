// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"

class UEW_SonaQ;
class USFXPreset_SonaQ;

/**
 * 
 */
class SONAFRIDGEEDITOR_API FAssetEditor_SonaQ final : public FAssetEditorToolkit, public FGCObject
{
public:
	FAssetEditor_SonaQ();
	virtual ~FAssetEditor_SonaQ() override;
	void    Init(TSharedPtr<IToolkitHost> InToolkitHost, USFXPreset_SonaQ* InSonaQPreset);

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
	TSharedRef<SDockTab> SpawnTab_SonaQWidget(const FSpawnTabArgs& Args);

	// UObjects that referenced by this asset editor. 
	USFXPreset_SonaQ* SonaQPreset = nullptr;
	UEW_SonaQ*        SonaQWidget = nullptr;

	TSharedPtr<IDetailsView> DetailsView;

	static const FName Identifier;
	static const FName ToolkitFName;
	static const FName DetailsTabId;
	static const FName SonaQTabId;
};
