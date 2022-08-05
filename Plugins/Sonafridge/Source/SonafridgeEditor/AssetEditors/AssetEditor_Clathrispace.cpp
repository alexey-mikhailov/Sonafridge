// Sonafridge 2022


#include "AssetEditor_Clathrispace.h"

#include "ClathrispaceEditorPreview.h"
#include "SonafridgeEditor/Widgets/W_Clathrispace.h"
#include "Sonafridge/Attenuator/Clathrispace.h"
#include "WidgetBlueprint.h"

#define LOCTEXT_NAMESPACE "FAssetEditor_Clathrispace"

const FName FAssetEditor_Clathrispace::Identifier(TEXT("AssetEditor_Clathrispace"));
const FName FAssetEditor_Clathrispace::ToolkitFName(TEXT("AssetEditorToolkit_Clathrispace"));

const FName FAssetEditor_Clathrispace::DetailsTabId(TEXT("AssetEditor_Clathrispace_PropertiesTab"));
const FName FAssetEditor_Clathrispace::ClathriEarTabId(TEXT("AssetEditor_Clathrispace_ClathriEarTab"));

FAssetEditor_Clathrispace::FAssetEditor_Clathrispace()
{
}

FAssetEditor_Clathrispace::~FAssetEditor_Clathrispace()
{
}

void FAssetEditor_Clathrispace::Init(TSharedPtr<IToolkitHost> InToolkitHost, UClathrispaceSettings* InPreset)
{
	EToolkitMode::Type Mode = InToolkitHost.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	ClathrispacePreset = InPreset;

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
		FTabManager::NewLayout("Standalone_AssetEditor_Clathrispace_Layout_v1")
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
				FTabManager::NewStack()
				->SetHideTabWell(true)
				->SetSizeCoefficient(0.775f)
				->AddTab(ClathriEarTabId, ETabState::OpenedTab)
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
	                                     { InPreset });

	RegenerateMenusAndToolbars();

	// Ensure all objects are transactable for undo/redo in the details panel
	InPreset->SetFlags(RF_Transactional);

	if (DetailsView.IsValid())
	{
		// Make sure details window is pointing to our object
		DetailsView->SetObject(InPreset);
	}
}

FLinearColor FAssetEditor_Clathrispace::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.0f, 0.0f, 0.2f, 0.5f);
}

FName FAssetEditor_Clathrispace::GetToolkitFName() const
{
	return FName("ClathrispacePresetEditor");
}

FText FAssetEditor_Clathrispace::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Clathrispace Editor");
}

FString FAssetEditor_Clathrispace::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Clathrispace ").ToString();
}

void FAssetEditor_Clathrispace::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_ClathrispaceEditor", "Clathrispace Editor"));

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	const FString    ClassName = ClathrispacePreset->GetClass()->GetName();
	const FSlateIcon BPIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.CreateClassBlueprint");

	InTabManager->RegisterTabSpawner(ClathriEarTabId,
	                                 FOnSpawnTab::CreateSP(this, &FAssetEditor_Clathrispace::SpawnTab_ClathriEar))
	            .SetDisplayName(LOCTEXT("ClathriEarTabName", "ClathriEar"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef())
	            .SetIcon(BPIcon);

	InTabManager->RegisterTabSpawner(DetailsTabId,
	                                 FOnSpawnTab::CreateSP(this, &FAssetEditor_Clathrispace::SpawnTab_Properties))
	            .SetDisplayName(LOCTEXT("DetailsTabName", "Details"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef())
	            .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FAssetEditor_Clathrispace::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(ClathriEarTabId);
	InTabManager->UnregisterTabSpawner(DetailsTabId);

	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

void FAssetEditor_Clathrispace::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(ClathrispaceWidget);
}

FString FAssetEditor_Clathrispace::GetReferencerName() const
{
	return "Clathrispace AssetEditor";
}

TSharedRef<SDockTab> FAssetEditor_Clathrispace::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == DetailsTabId);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DetailsTabTitle", "Legacy Details"))
		[
			DetailsView.ToSharedRef()
		];
}

TSharedRef<SDockTab> FAssetEditor_Clathrispace::SpawnTab_ClathrispaceWidget(const FSpawnTabArgs& Args)
{
	const FText Label = FText::FromString(GetEditingObject()->GetName());

	UWidgetBlueprint* Blueprint = LoadObject<UWidgetBlueprint>
	(
		nullptr,
		TEXT("/Sonafridge/UI/WBP_Clathrispace.WBP_Clathrispace")
	);

	UWorld* World = GEditor->GetEditorWorldContext().World();
	check(World);

	if (IsValid(Blueprint))
	{
		// Transient UWidget, which SWidget will be held by shared pointer. 
		ClathrispaceWidget = CreateWidget<UW_Clathrispace>
		(
			World, static_cast<UClass*>(Blueprint->GeneratedClass)
		);

		ClathrispaceWidget->Init(ClathrispacePreset);

		if (ClathrispaceWidget)
		{
			return SNew(SDockTab)
				.Label(Label)
				.TabColorScale(GetTabColorScale())
				[
					SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					.Padding(0.0f)
					[
						ClathrispaceWidget->TakeWidget()
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
			              "No editor available for Clathrispace Preset. Widget Blueprint not found."))
		];
}

TSharedRef<SDockTab> FAssetEditor_Clathrispace::SpawnTab_ClathriEar(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == ClathriEarTabId);

	return SNew(SDockTab)
		.Label(LOCTEXT("ClathriEarTabTitle", "ClathriEar"))
		[
			SNew(SClathrispaceViewport).Settings(ClathrispacePreset)
		];
}

#undef LOCTEXT_NAMESPACE
