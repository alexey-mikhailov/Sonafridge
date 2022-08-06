// Sonafridge 2022

#include "ContentBrowserMenuExtender.h"
#include "SonafridgeEditor/AssetEditors/SonaQ/View/EW_SonaQ.h"
#include "Sonafridge/SignalProcessing/SFX_SonaQ.h"
#include "WidgetBlueprint.h"
#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "Sonafridge"

void FSonafridgeContentBrowserMenuExtender::Startup()
{
	FContentBrowserModule& ContentBrowserModule =
	{
		FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"))
	};

	TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuAssetExtenderDelegates = 
	{
		ContentBrowserModule.GetAllAssetViewContextMenuExtenders()
	};

	MenuExtenderForSelectedSonaQs = FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FSonafridgeContentBrowserMenuExtender::OnExtendAssetMenu);
	CBMenuAssetExtenderDelegates.Add(MenuExtenderForSelectedSonaQs);

	if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
	{
		auto EditorRequestedOpen = AssetEditorSubsystem->OnAssetEditorRequestedOpen();
		EditorRequestedOpen.AddRaw(this, &FSonafridgeContentBrowserMenuExtender::OnAssetEditorRequestedOpen);
	}
}

void FSonafridgeContentBrowserMenuExtender::Shutdown()
{
	FContentBrowserModule& ContentBrowserModule =
	{
		FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"))
	};

	TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuAssetExtenderDelegates = 
	{
		ContentBrowserModule.GetAllAssetViewContextMenuExtenders()
	};

	CBMenuAssetExtenderDelegates.RemoveAll([this](const FContentBrowserMenuExtender_SelectedAssets& Delegate)
	{
		return Delegate.GetHandle() == MenuExtenderForSelectedSonaQs.GetHandle();
	});

	MenuExtenderForSelectedSonaQs = {};
}

TSharedRef<FExtender> FSonafridgeContentBrowserMenuExtender::OnExtendAssetMenu(const TArray<FAssetData>& SelectedAssets)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();

	Extender->AddMenuExtension
	(
		"CommonAssetActions",
		EExtensionHook::After,
		nullptr,
		FMenuExtensionDelegate::CreateRaw(this, &FSonafridgeContentBrowserMenuExtender::OnAssetExtended, SelectedAssets)
	);

	return Extender;
}

void FSonafridgeContentBrowserMenuExtender::OnAssetExtended(FMenuBuilder&      MenuBuilder,
                                                            TArray<FAssetData> SelectedAssets)
{
    MenuBuilder.BeginSection("Sonafridge", LOCTEXT("Sonafridge", "Sonafridge"));

	{
        for (auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt)
		{
			const FAssetData& Asset = *AssetIt;
			
			if (!Asset.IsRedirector() && Asset.AssetClass != NAME_Class && !(Asset.PackageFlags & PKG_FilterEditorOnly))
			{
				if (Asset.GetClass()->IsChildOf(USFXPreset_SonaQ::StaticClass()))
				{
					AddMenuEntryForSonaQs(MenuBuilder, Asset);
				}
			}
		}
    }

    MenuBuilder.EndSection();
}

void FSonafridgeContentBrowserMenuExtender::AddMenuEntryForSonaQs(FMenuBuilder& MenuBuilder,
                                                                        const FAssetData&   Asset)
{
	USFXPreset_SonaQ* SonaQPreset = Cast<USFXPreset_SonaQ>(Asset.GetAsset());

	MenuBuilder.AddMenuEntry
	(
		LOCTEXT("Extended Editor", "Extended Editor"),
		LOCTEXT("Will open extended editor. ", "Will open extended editor"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([SonaQPreset]
		{
			UWidgetBlueprint* Blueprint = LoadObject<UWidgetBlueprint>
			(
				nullptr,
				TEXT("/Sonafridge/UI/EW_SonaQ.EW_SonaQ")
			);

			UWorld* World = GEditor->GetEditorWorldContext().World();
			check(World);

			UEW_SonaQ* EW_SonaQ = CreateWidget<UEW_SonaQ>
			(
				World,
				static_cast<UClass*>(Blueprint->GeneratedClass)
			);

			EW_SonaQ->Init(SonaQPreset);

			TSharedRef<SWindow> MyWindow = 
			{
				SNew(SWindow).ClientSize({ 600, 400 }).MaxWidth(600).MaxHeight(400)
				[
					EW_SonaQ->TakeWidget()
				]
			};

			FSlateApplication::Get().AddWindow(MyWindow, true);
		})),
		NAME_None,
		EUserInterfaceActionType::Button
	);
}

void FSonafridgeContentBrowserMenuExtender::OnAssetEditorRequestedOpen(UObject* InObject)
{

}

#undef LOCTEXT_NAMESPACE
