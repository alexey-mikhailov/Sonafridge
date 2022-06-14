// Copyright Epic Games, Inc. All Rights Reserved.

#include "SonafridgeEditor.h"
#include "SonafridgeStyle.h"
#include "SonafridgeCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName SonafridgeTabName("Sonafridge");

#define LOCTEXT_NAMESPACE "FSonafridgeModule"

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
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SonafridgeTabName, FOnSpawnTab::CreateRaw(this, &FSonafridgeEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FSonafridgeTabTitle", "Sonafridge"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FSonafridgeEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSonafridgeStyle::Shutdown();

	FSonafridgeCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SonafridgeTabName);
}

TSharedRef<SDockTab> FSonafridgeEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
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