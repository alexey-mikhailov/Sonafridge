// Sonafridge 2022

#include "EW_SonaQ.h"
#include "EW_SonaQFrequencyResponse.h"
#include "EW_SonaQBandPopup.h"
#include "Sonafridge/SonafridgeCommon.h"
#include "Components/CanvasPanelSlot.h"
#include "AudioDevice.h"

UEW_SonaQ::UEW_SonaQ()
{
}

void UEW_SonaQ::Init(TSharedPtr<FVM_SonaQ> InViewModel)
{
	if (InViewModel)
	{
		SampleRate = InViewModel->GetSampleRate();
		ViewModel = InViewModel;
		FrequencyResponse->Init(this, ViewModel);
		bWasInitialized = true;
	}
	else
	{
		UE_LOG(LogSonafridgeEditor, Error, TEXT("UEW_SonaQ::Init: InViewModel argument is not valid. "));
	}
}

void UEW_SonaQ::SetSelectedBand(TSharedPtr<FVM_SonaQBand> InBand)
{
	SelectedBand = InBand;
	SelectedBandIndex = -1;

	auto& Bands = ViewModel->GetBands();
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

void UEW_SonaQ::SetSelectedBandIndex(int32 InBandIndex)
{
	SelectedBand = {};
	SelectedBandIndex = InBandIndex;

	auto& Bands = ViewModel->GetBands();

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

void UEW_SonaQ::NativeConstruct()
{
	Super::NativeConstruct();

	if (!bWasInitialized)
	{
		// Deferred initialization in case of settings injection (Init call) is impossible. 

		if (auto AudioDevice = GEngine->GetActiveAudioDevice())
		{
			SampleRate = AudioDevice->GetSampleRate();
		}

		TArray<TSharedPtr<FVM_SonaQBand>> Bands;
		auto Band = MakeShared<FVM_SonaQBand>();
		Band->Init(SampleRate);
		Band->SetType(EBandType::LowShelf);
		Band->SetFrequency(100.f);
		Band->SetQuality(.5f);
		Band->SetAmountDb(0.f);
		Band->SetLoudCompDb(0.f);
		Bands.Add(Band);

		Band = MakeShared<FVM_SonaQBand>();
		Band->Init(SampleRate);
		Band->SetType(EBandType::BandCut);
		Band->SetFrequency(1000.f);
		Band->SetQuality(.667f);
		Band->SetAmountDb(0.f);
		Band->SetLoudCompDb(0.f);
		Bands.Add(Band);
		
		Band = MakeShared<FVM_SonaQBand>();
		Band->Init(SampleRate);
		Band->SetType(EBandType::HighShelf);
		Band->SetFrequency(10000.f);
		Band->SetQuality(.5f);
		Band->SetAmountDb(0.f);
		Band->SetLoudCompDb(0.f);
		Bands.Add(Band);

		ViewModel = MakeShared<FVM_SonaQ>();
		ViewModel->Init(Bands);

		// Children too

		FrequencyResponse->Init(this, ViewModel);
		BandPopup->Init(this, ViewModel);
		BandPopup->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UEW_SonaQ::OnSizeChanged(const FVector2D& OldSize, const FVector2D& NewSize)
{
	BandPopup->FollowBand();
}

int32 UEW_SonaQ::NativePaint(const FPaintArgs&        Args,
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

	UEW_SonaQ* MutableThis = const_cast<UEW_SonaQ*>(this);
	FVector2D  Size = AllottedGeometry.GetLocalSize();

	if (Size != LastSize)
	{
		MutableThis->OnSizeChanged(LastSize, Size);
	}

	MutableThis->LastSize = Size;
	return Result;
}

