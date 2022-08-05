// Sonafridge 2022

#include "SonafridgeEditor.h"

#include "AssetActions/AssetTypeActions_SonaQ.h"
#include "AssetActions/AssetTypeActions_Clathrispace.h"
#include "Widgets/EW_SonaQ.h"
#include "SonafridgeStyle.h"
#include "SonafridgeCommands.h"
#include "Sonafridge/SonafridgeCommon.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "AssetToolsModule.h"
#include "LevelEditor.h"
#include "ToolMenus.h"
#include "AssetEditors/ClathrispacePreviewScene.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"

#define LOCTEXT_NAMESPACE "Sonafridge"

DEFINE_LOG_CATEGORY(LogSonafridgeEditor);

static const FName SonafridgeTabName("Sonafridge");


void FSonafridgeEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSonafridgeStyle::Initialize();
	FSonafridgeStyle::ReloadTextures();

	FSonafridgeCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSonafridgeCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FSonafridgeEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSonafridgeEditorModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner
	(
		SonafridgeTabName, 
		FOnSpawnTab::CreateRaw(this, &FSonafridgeEditorModule::OnSpawnPluginTab)
	)
	.SetDisplayName(LOCTEXT("FSonafridgeTabTitle", "Sonafridge"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);

	ContentBrowserMenuExtender.Startup();

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	auto SonaQActions = MakeShared<FAssetTypeActions_SonaQ>();
	AssetTools.RegisterAssetTypeActions(SonaQActions);
	AssetActions.Add(SonaQActions);

	auto ClathrispaceActions = MakeShared<FAssetTypeActions_ClathrispaceSettings>();
	AssetTools.RegisterAssetTypeActions(ClathrispaceActions);
	AssetActions.Add(ClathrispaceActions);

	const TSharedPtr<FHelmetVisualizer> Visualizer = MakeShared<FHelmetVisualizer>();
	FName Name = UClathrispaceHelmetComponent::StaticClass()->GetFName();
	GUnrealEd->RegisterComponentVisualizer(Name, Visualizer);
	Visualizer->OnRegister();
}

void FSonafridgeEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FName Name = UClathrispaceHelmetComponent::StaticClass()->GetFName();
	if (!Name.IsNone())
	{
		GUnrealEd->UnregisterComponentVisualizer(Name);
	}

	AssetActions.Reset();

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSonafridgeStyle::Shutdown();

	FSonafridgeCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SonafridgeTabName);

	ContentBrowserMenuExtender.Shutdown();
}

TSharedRef<SDockTab> FSonafridgeEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format
	(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FSonafridgeModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("Sonafridge.cpp"))
	);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FSonafridgeEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(SonafridgeTabName);
}

void FSonafridgeEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FSonafridgeCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSonafridgeCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSonafridgeEditorModule, SonafridgeEditor)