// Sonafridge 2022


#include "AssetTypeActions_SonaQ.h"
#include "Sonafridge/SignalProcessing/SFX_SonaQ.h"
#include "SonafridgeEditor/AssetEditors/AssetEditor_SonaQ.h"

FAssetTypeActions_SonaQ::FAssetTypeActions_SonaQ()
{
}

FAssetTypeActions_SonaQ::~FAssetTypeActions_SonaQ()
{
}

UClass* FAssetTypeActions_SonaQ::GetSupportedClass() const
{
	return USFXPreset_SonaQ::StaticClass();
}

void FAssetTypeActions_SonaQ::OpenAssetEditor(const TArray<UObject*>&  InObjects,
                                              TSharedPtr<IToolkitHost> InToolkitHost)
{
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (auto SonaQPreset = Cast<USFXPreset_SonaQ>(*ObjIt))
		{
			TSharedRef<FAssetEditor_SonaQ> Editor = MakeShared<FAssetEditor_SonaQ>();
			Editor->Init(InToolkitHost, SonaQPreset);
		}
	}
}
