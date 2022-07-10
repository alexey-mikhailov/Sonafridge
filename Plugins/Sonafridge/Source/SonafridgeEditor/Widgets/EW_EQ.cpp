// Sonafridge 2022

#include "EW_EQ.h"
#include "EW_EQFrequencyResponse.h"
#include "AudioDevice.h"
#include "EW_EQBandPopup.h"
#include "Components/CanvasPanelSlot.h"

UEW_EQ::UEW_EQ()
{
}

void UEW_EQ::Init(TSharedPtr<IEQSettings> InSettings)
{
	SampleRate = InSettings->GetSampleRate();
	Settings = InSettings;
	FrequencyResponse->Init(this, Settings);
	bWasInitialized = true;
}

void UEW_EQ::SetSelectedBand(TSharedPtr<FEQBand> InBand)
{
	SelectedBand = InBand;
	SelectedBandIndex = -1;

	auto& Bands = Settings->GetBands();
	for (int32 Index = 0; Index < Bands.Num(); ++Index)
	{
		auto Band = Bands[Index];

		if (Band == InBand)
		{
			SelectedBandIndex = Index;
			break;
		}
	}

	BandSelectionChanged.Broadcast(SelectedBand);
}

void UEW_EQ::SetSelectedBandIndex(int32 InBandIndex)
{
	SelectedBand = {};
	SelectedBandIndex = InBandIndex;

	auto& Bands = Settings->GetBands();

	for (int32 Index = 0; Index < Bands.Num(); ++Index)
	{
		auto& Band = Bands[Index];

		if (Index == InBandIndex)
		{
			SelectedBand = Band;
			break;
		}
	}

	BandSelectionChanged.Broadcast(SelectedBand);
}

void UEW_EQ::NativeConstruct()
{
	Super::NativeConstruct();

	if (!bWasInitialized)
	{
		// Deferred initialization in case of settings injection (Init call) is impossible. 

		if (auto AudioDevice = GEngine->GetActiveAudioDevice())
		{
			SampleRate = AudioDevice->GetSampleRate();
		}

		TArray<TSharedPtr<FEQBand>> Bands;
		auto Band_BC = MakeShared<FEQBand>();
		Band_BC->Init(SampleRate);
		Band_BC->SetType(EBandType::LowShelf);
		Band_BC->SetFrequency(100.f);
		Band_BC->SetQuality(.5f);
		Band_BC->SetAmountDb(0.f);
		Band_BC->SetLoudCompDb(0.f);
		Bands.Add(Band_BC);

		Band_BC = MakeShared<FEQBand>();
		Band_BC->Init(SampleRate);
		Band_BC->SetType(EBandType::BandCut);
		Band_BC->SetFrequency(1000.f);
		Band_BC->SetQuality(.667f);
		Band_BC->SetAmountDb(0.f);
		Band_BC->SetLoudCompDb(0.f);
		Bands.Add(Band_BC);
		
		Band_BC = MakeShared<FEQBand>();
		Band_BC->Init(SampleRate);
		Band_BC->SetType(EBandType::HighShelf);
		Band_BC->SetFrequency(10000.f);
		Band_BC->SetQuality(.5f);
		Band_BC->SetAmountDb(0.f);
		Band_BC->SetLoudCompDb(0.f);
		Bands.Add(Band_BC);
		
		auto SettingsMock = MakeShared<FEQSettingsMock>();
		SettingsMock->Init(Bands);
		Settings = SettingsMock;

		// Children too

		FrequencyResponse->Init(this, Settings);
		BandPopup->Init(this, Settings);
		BandPopup->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UEW_EQ::OnSizeChanged(const FVector2D& OldSize, const FVector2D& NewSize)
{
	BandPopup->FollowBand();
}

int32 UEW_EQ::NativePaint(const FPaintArgs&        Args,
                          const FGeometry&         AllottedGeometry,
                          const FSlateRect&        MyCullingRect,
                          FSlateWindowElementList& OutDrawElements,
                          int32                    LayerId,
                          const FWidgetStyle&      InWidgetStyle,
                          bool                     bParentEnabled) const
{
	int32 Result = Super::NativePaint(Args,
	                                  AllottedGeometry,
	                                  MyCullingRect,
	                                  OutDrawElements,
	                                  LayerId,
	                                  InWidgetStyle,
	                                  bParentEnabled);

	UEW_EQ*   MutableThis = const_cast<UEW_EQ*>(this);
	FVector2D Size = AllottedGeometry.GetLocalSize();

	if (Size != LastSize)
	{
		MutableThis->OnSizeChanged(LastSize, Size);
	}

	MutableThis->LastSize = Size;
	return Result;
}

