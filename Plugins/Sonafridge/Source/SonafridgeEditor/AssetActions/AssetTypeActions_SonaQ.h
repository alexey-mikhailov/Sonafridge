// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/**
 * 
 */
class SONAFRIDGEEDITOR_API FAssetTypeActions_SonaQ final : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_SonaQ();
	virtual ~FAssetTypeActions_SonaQ() override;

	virtual FText   GetName() const override { return NSLOCTEXT("Sonafridge", "AssetTypeActions_SonaQ", "SonaQ"); }
	virtual UClass* GetSupportedClass() const override;
	virtual FColor  GetTypeColor() const override { return FColor(97, 85, 212); }
	virtual uint32  GetCategories() override { return EAssetTypeCategories::Sounds; }

	virtual void OpenAssetEditor(const TArray<UObject*>&  InObjects,
	                             TSharedPtr<IToolkitHost> InToolkitHost) override;
};
