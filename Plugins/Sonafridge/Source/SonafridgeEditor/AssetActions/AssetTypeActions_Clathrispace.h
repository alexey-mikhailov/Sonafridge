// Sonafridge 2022

# pragma once

#include "AssetTypeActions_Base.h"

class FAssetTypeActions_ClathrispaceSettings : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual const TArray<FText>& GetSubMenus() const override;

	virtual void OpenAssetEditor(const TArray<UObject*>&  InObjects,
	                             TSharedPtr<IToolkitHost> InToolkitHost) override;
};
