#include "EW_EQManagementStrip.h"
#include "EW_EQ.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "AudioDevice.h"


void UEW_EQManagementStrip::Init(UEW_EQ*                 InRootWidget,
                                 TSharedPtr<IEQSettings> InSettings)
{
	RootWidget = InRootWidget;
	Settings = InSettings;
}

void UEW_EQManagementStrip::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonPower->OnPressed.AddDynamic(this, &UEW_EQManagementStrip::OnToggleOnOff);
	ButtonAdd->OnPressed.AddDynamic(this, &UEW_EQManagementStrip::OnAddBand);
	ButtonRemove->OnPressed.AddDynamic(this, &UEW_EQManagementStrip::OnRemoveBand);
	ButtonReset->OnPressed.AddDynamic(this, &UEW_EQManagementStrip::OnResetBand);
	ButtonToggleFreeze->OnPressed.AddDynamic(this, &UEW_EQManagementStrip::OnToggleFreeze);
}

void UEW_EQManagementStrip::OnToggleOnOff()
{
	if (auto Band = RootWidget->GetSelectedBand())
	{
		Band->SetIsEnabled(!Band->GetIsEnabled());
		RootWidget->GetEvent_BandChanged().Broadcast(Band);
	}
}

void UEW_EQManagementStrip::OnAddBand()
{
	auto Band = MakeShared<FEQBand>();
	Band->Init(Settings->GetSampleRate());
	Band->SetType(EBandType::Bandcut);
	Band->SetFrequency(1000.f);
	Band->SetQuality(10.f);
	Band->SetAmountDb(-6.f);
	Band->SetLoudCompDb(2.f);
	Settings->AddBand(Band);
	RootWidget->SetSelectedBand(Band);
	RootWidget->GetEvent_BandSelectionChanged().Broadcast(Band);
	RootWidget->GetEvent_BandAdded().Broadcast(Band);
	RootWidget->GetEvent_BandChanged().Broadcast(Band);
}

void UEW_EQManagementStrip::OnRemoveBand()
{
	if (auto Band = RootWidget->GetSelectedBand())
	{
		Settings->RemoveBand(Band);
		RootWidget->SetSelectedBandIndex(-1);
		RootWidget->GetEvent_BandSelectionChanged().Broadcast(Band);
		RootWidget->GetEvent_BandRemoved().Broadcast(Band);
		RootWidget->GetEvent_BandChanged().Broadcast(Band);
	}
}

void UEW_EQManagementStrip::OnResetBand()
{
	if (auto Band = RootWidget->GetSelectedBand())
	{
		Settings->ResetBand(Band);
		RootWidget->GetEvent_BandChanged().Broadcast(Band);
	}
}

void UEW_EQManagementStrip::OnToggleFreeze()
{
	if (bIsFrozen)
	{
		Settings->Unfreeze();
	}
	else
	{
		Settings->Freeze();
	}
}

