// Sonafridge 2022

#include "AssetTypeActions_Clathrispace.h"
#include "Sonafridge/Attenuator/Clathrispace.h"
#include "SonafridgeEditor/AssetEditors/Clathrispace/AssetEditor_Clathrispace.h"

#define LOCTEXT_NAMESPACE "Sonafridge"

FText FAssetTypeActions_ClathrispaceSettings::GetName() const
{
	return LOCTEXT("AssetTypeActions_Clathrispace",
	               "Clathrispace Spatialization Settings");
}

FColor FAssetTypeActions_ClathrispaceSettings::GetTypeColor() const
{
	return FColor(0xf5, 0xc3, 0x65);
}

UClass* FAssetTypeActions_ClathrispaceSettings::GetSupportedClass() const
{
	return UClathrispaceSettings::StaticClass();
}

uint32 FAssetTypeActions_ClathrispaceSettings::GetCategories()
{
	return EAssetTypeCategories::Sounds;
}

const TArray<FText>& FAssetTypeActions_ClathrispaceSettings::GetSubMenus() const
{
	static const TArray<FText> SubMenus { LOCTEXT("ClathrispaceSubMenus", "Clathrispace") };
	return SubMenus;
}

void FAssetTypeActions_ClathrispaceSettings::OpenAssetEditor(const TArray<UObject*>&  InObjects,
                                                             TSharedPtr<IToolkitHost> InToolkitHost)
{
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UClathrispaceSettings* const Preset = Cast<UClathrispaceSettings>(*ObjIt))
		{
			const TSharedRef<FAssetEditor_Clathrispace> Editor = MakeShared<FAssetEditor_Clathrispace>();
			Editor->Init(InToolkitHost, Preset);
		}
	}
}

#undef LOCTEXT_NAMESPACE
