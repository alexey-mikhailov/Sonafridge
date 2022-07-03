// Sonafridge 2022

#include "EW_EQ.h"
#include "EW_EQFrequencyResponse.h"
#include "AudioDevice.h"
#include "EW_EQBandList.h"
#include "EW_EQBandPopup.h"
#include "EW_EQManagementStrip.h"
#include "Components/CanvasPanelSlot.h"
#include "Sonafridge/MathTools.h"

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

void UEW_EQ::RefreshBandPopup()
{
	BandPopup->SetVisibility(SelectedBand ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	BandPopup->Move(GetBandWPos(SelectedBand), LastSize);
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
		Band_BC->SetType(EBandType::BandCut);
		Band_BC->SetFrequency(1000.f);
		Band_BC->SetQuality(10.f);
		Band_BC->SetAmountDb(-6.f);
		Band_BC->SetLoudCompDb(2.f);
		
		Bands.Add(Band_BC);
		
		auto SettingsMock = MakeShared<FEQSettingsMock>();
		SettingsMock->Init(Bands);
		Settings = SettingsMock;

		// Children too

		FrequencyResponse->Init(this, Settings);
		BandPopup->SetVisibility(ESlateVisibility::Hidden);
	}
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

	MutableThis->LastSize = Size;
	return Result;
}

FVector2D UEW_EQ::GetBandWPos(TSharedPtr<FEQBand> InBand)
{
	if (InBand)
	{
		float F = InBand->GetFrequency();
		float NX = MathLogTool::TwentiethsToTribel(F);
		float NY = (DynamicMax - Settings->DtftDb(F))
		         / (DynamicMax - DynamicMin);
		float WX = NX * LastSize.X;
		float WY = NY * LastSize.Y;
		return { WX, WY };
	}

	return {};
}

