// Sonafridge 2022

#include "EW_EQBandList.h"
#include "EW_EQBand.h"
#include "EW_EQ.h"
#include "Components/ListView.h"
#include "AudioDevice.h"

void UEW_EQBandList::Init(UEW_EQ*                 InRootWidget,
                          TSharedPtr<IEQSettings> InSettings)
{
	RootWidget = InRootWidget;
	RootWidget->GetEvent_BandAdded().AddUObject(this, &UEW_EQBandList::OnBandAdded);
	RootWidget->GetEvent_BandRemoved().AddUObject(this, &UEW_EQBandList::OnBandRemoved);
	Settings = InSettings;

	ConcurrentInit();
}

void UEW_EQBandList::NativeConstruct()
{
	Super::NativeConstruct();
	ListViewBands->OnItemSelectionChanged().AddUObject(this, &UEW_EQBandList::OnSelectedBandChanged);

	ConcurrentInit();
}

void UEW_EQBandList::OnSelectedBandChanged(UObject* Object)
{
	if (auto Proxy = Cast<UProxy_EQBand>(Object))
	{
		RootWidget->SetSelectedBand(Proxy->Band);
	}
}

void UEW_EQBandList::OnBandAdded(TSharedPtr<FEQBand> InBand)
{
	UProxy_EQBand* BandProxy = NewObject<UProxy_EQBand>();
	BandProxy->Settings = Settings;
	BandProxy->Band = InBand;
	BandProxy->RootWidget = RootWidget;

	ListViewBands->AddItem(BandProxy);
	BandProxies.Add(InBand, BandProxy);
}

void UEW_EQBandList::OnBandRemoved(TSharedPtr<FEQBand> InBand)
{
	if (UProxy_EQBand** FoundItem = BandProxies.Find(InBand))
	{
		ListViewBands->RemoveItem(*FoundItem);
		BandProxies.Remove(InBand);
	}
}

void UEW_EQBandList::ConcurrentInit()
{
	if (Settings && IsValid(ListViewBands))
	{
		for (auto Band : Settings->GetBands())
		{
			OnBandAdded(Band);
		}
	}
}
