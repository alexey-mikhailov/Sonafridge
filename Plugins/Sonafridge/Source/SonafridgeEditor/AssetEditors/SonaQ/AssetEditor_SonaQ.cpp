// Sonafridge 2022


#include "AssetEditor_SonaQ.h"

#include "Sonafridge/SignalProcessing/SFX_SonaQ.h"
#include "SonafridgeEditor/AssetEditors/SonaQ/View/EW_SonaQ.h"
#include "WidgetBlueprint.h"

#define LOCTEXT_NAMESPACE "FAssetEditor_SonaQ"

const FName FAssetEditor_SonaQ::Identifier(TEXT( "AssetEditor_SonaQ"));
const FName FAssetEditor_SonaQ::ToolkitFName(TEXT("AssetEditorToolkit_SonaQ"));
const FName FAssetEditor_SonaQ::DetailsTabId(TEXT("AssetEditor_SonaQ_PropertiesTab"));
const FName FAssetEditor_SonaQ::SonaQTabId(TEXT("AssetEditor_SonaQ_SonaQTab"));

FAssetEditor_SonaQ::FAssetEditor_SonaQ()
{
}

FAssetEditor_SonaQ::~FAssetEditor_SonaQ()
{
}

void FAssetEditor_SonaQ::Init(TSharedPtr<IToolkitHost> InToolkitHost, USFXPreset_SonaQ* InSonaQPreset)
{
	EToolkitMode::Type Mode = InToolkitHost.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	SonaQPreset = InSonaQPreset;

	FPropertyEditorModule& PropertyEditorModule = 
		FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bAllowSearch = true;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::ObjectsUseNameArea;
	DetailsViewArgs.bHideSelectionTip = true;

	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = 
		FTabManager::NewLayout("Standalone_AssetEditor_SonaQ_Layout_v1")
		->AddArea
	(
		FTabManager::NewPrimaryArea() ->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewStack()
			->SetSizeCoefficient(0.1f)
			->SetHideTabWell(true)
			->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
		)
		->Split
		(
			FTabManager::NewSplitter()
			->Split
			(
				FTabManager::NewSplitter()
				->SetSizeCoefficient(0.775f)
				->SetOrientation(Orient_Vertical)
				->Split
				(
					FTabManager::NewStack()
					->SetHideTabWell(true)
					->SetSizeCoefficient(0.33f)
					->AddTab(SonaQTabId, ETabState::OpenedTab)
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.225f)
				->AddTab(DetailsTabId, ETabState::ClosedTab)
			)
		)
	);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;

	FAssetEditorToolkit::InitAssetEditor(Mode,
	                                     InToolkitHost,
	                                     Identifier,
	                                     StandaloneDefaultLayout,
	                                     bCreateDefaultStandaloneMenu,
	                                     bCreateDefaultToolbar,
	                                     { InSonaQPreset });

	RegenerateMenusAndToolbars();

	// Ensure all objects are transactable for undo/redo in the details panel
	InSonaQPreset->SetFlags(RF_Transactional);

	if (DetailsView.IsValid())
	{
		// Make sure details window is pointing to our object
		DetailsView->SetObject(InSonaQPreset);
	}
}

FLinearColor FAssetEditor_SonaQ::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.0f, 0.0f, 0.2f, 0.5f);
}

FName FAssetEditor_SonaQ::GetToolkitFName() const
{
	return FName("SonaQPresetEditor");
}

FText FAssetEditor_SonaQ::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "SonaQ Editor");
}

FString FAssetEditor_SonaQ::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "SonaQ ").ToString();
}

void FAssetEditor_SonaQ::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_SonaQEditor", "SonaQ Editor"));

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	const FString    ClassName = SonaQPreset->GetClass()->GetName();
	const FSlateIcon BPIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.CreateClassBlueprint");

	InTabManager->RegisterTabSpawner(SonaQTabId,
	                                 FOnSpawnTab::CreateSP(this, &FAssetEditor_SonaQ::SpawnTab_SonaQWidget))
	            .SetDisplayName(
		            FText::Format(LOCTEXT("UserEditorTabFormat", "{0} Editor"), FText::FromString(ClassName)))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef())
	            .SetIcon(BPIcon);

	InTabManager->RegisterTabSpawner(DetailsTabId,
	                                 FOnSpawnTab::CreateSP(this, &FAssetEditor_SonaQ::SpawnTab_Properties))
	            .SetDisplayName(LOCTEXT("DetailsTab", "Details"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef())
	            .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FAssetEditor_SonaQ::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(SonaQTabId);
	InTabManager->UnregisterTabSpawner(DetailsTabId);

	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

void FAssetEditor_SonaQ::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(SonaQPreset);
}

FString FAssetEditor_SonaQ::GetReferencerName() const
{
	return "SonaQ AssetEditor";
}

TSharedRef<SDockTab> FAssetEditor_SonaQ::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == DetailsTabId);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DetailsTabTitle", "Legacy Details"))
		[
			DetailsView.ToSharedRef()
		];
}

TSharedRef<SDockTab> FAssetEditor_SonaQ::SpawnTab_SonaQWidget(const FSpawnTabArgs& Args)
{
	const FText Label = FText::FromString(GetEditingObject()->GetName());

	UWidgetBlueprint* Blueprint = LoadObject<UWidgetBlueprint>
	(
		nullptr,
		TEXT("/Sonafridge/UI/EW_SonaQ.EW_SonaQ")
	);

	UWorld* World = GEditor->GetEditorWorldContext().World();
	check(World);

	if (IsValid(Blueprint))
	{
		// Transient UWidget, which SWidget will be held by shared pointer. 
		SonaQWidget = CreateWidget<UEW_SonaQ>
		(
			World, static_cast<UClass*>(Blueprint->GeneratedClass)
		);

		SonaQWidget->Init(SonaQPreset);

		if (SonaQWidget)
		{
			return SNew(SDockTab)
				.Label(Label)
				.TabColorScale(GetTabColorScale())
				[
					SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					.Padding(0.0f)
					[
						SonaQWidget->TakeWidget()
					]
				];
		}
	}

	return SNew(SDockTab)
		.Label(Label)
		.TabColorScale(GetTabColorScale())
		[
			SNew(STextBlock)
			.Text(LOCTEXT("InvalidPresetEditor",
			              "No editor available for SonaQ Preset. Widget Blueprint not found."))
		];
}

#undef LOCTEXT_NAMESPACE
