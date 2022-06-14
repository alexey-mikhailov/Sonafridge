#include "EW_EQBand.h"
#include "AudioDevice.h"
#include "EW_EQ.h"
#include "Components/Border.h"
#include "Sonafridge/MathTools.h"
#include "Kismet/KismetTextLibrary.h"
#include "Synthesis/Public/UI/SynthKnob.h"

void UEW_EQBand::NativeConstruct()
{
	Super::NativeConstruct();

	KnobLoudnessCompensation->OnValueChanged.AddDynamic(this, &UEW_EQBand::OnLoudnessCompensationChanged);
	KnobFrequency->OnValueChanged.AddDynamic(this, &UEW_EQBand::OnFrequencyChanged);
	KnobQuality->OnValueChanged.AddDynamic(this, &UEW_EQBand::OnQualityChanged);
	KnobAmountDb->OnValueChanged.AddDynamic(this, &UEW_EQBand::OnAmountDbChanged);
}

void UEW_EQBand::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (auto Proxy = Cast<UProxy_EQBand>(ListItemObject))
	{
		RootWidget = Proxy->RootWidget;
		RootWidget->GetEvent_BandSelectionChanged().AddUObject(this, &UEW_EQBand::OnBandSelectionChanged);
		RootWidget->GetEvent_BandChanged().AddUObject(this, &UEW_EQBand::OnBandChanged);
		Settings = Proxy->Settings;
		Band = Proxy->Band;
		KnobLoudnessCompensation->SetValue((Band->GetLoudCompDb() + 48.f) / 96.f);
		KnobFrequency->SetValue(MathLogTool::TwentiethsToTribel(Band->GetFrequency()));
		KnobQuality->SetValue(MathLogTool::ThousandsToHexabel(Band->GetQuality()));
		KnobAmountDb->SetValue((Band->GetAmountDb() + 48.f) / 96.f);
		TextLoudnessCompensation->SetText(FormatDb(Band->GetLoudCompDb()));
		TextFrequency->SetText(FormatHz(Band->GetFrequency()));
		TextQuality->SetText(FormatQunits(Band->GetQuality()));
		TextAmountDb->SetText(FormatDb(Band->GetAmountDb()));

		BorderSelectionMarker->SetVisibility(Band == RootWidget->GetSelectedBand()
		                                     ? ESlateVisibility::Visible
		                                     : ESlateVisibility::Hidden);
	}
}

int32 UEW_EQBand::NativePaint(const FPaintArgs&        Args,
                              const FGeometry&         AllottedGeometry,
                              const FSlateRect&        MyCullingRect,
                              FSlateWindowElementList& OutDrawElements,
                              int32                    LayerId,
                              const FWidgetStyle&      InWidgetStyle,
                              bool                     bParentEnabled) const
{
	return Super::NativePaint(Args,
	                          AllottedGeometry,
	                          MyCullingRect,
	                          OutDrawElements,
	                          LayerId,
	                          InWidgetStyle,
	                          bParentEnabled);
}

void UEW_EQBand::OnLoudnessCompensationChanged(float NewValue)
{
	if (IsValidDataContext())
	{
		float LoudnessDb = 96.f * NewValue - 48.f;
		Band->SetLoudCompDb(LoudnessDb);
		RootWidget->GetEvent_BandChanged().Broadcast(Band);
		TextLoudnessCompensation->SetText(FormatDb(LoudnessDb));
	}
}

void UEW_EQBand::OnFrequencyChanged(float NewValue)
{
	if (IsValidDataContext())
	{
		float Frequency = MathLogTool::TribelToTwentieths(NewValue);
		Band->SetFrequency(Frequency);
		RootWidget->GetEvent_BandChanged().Broadcast(Band);
		TextFrequency->SetText(FormatHz(Frequency));
	}
}

void UEW_EQBand::OnQualityChanged(float NewValue)
{
	if (IsValidDataContext())
	{
		float Quality = MathLogTool::HexabelToThousands(NewValue);
		Band->SetQuality(Quality);
		RootWidget->GetEvent_BandChanged().Broadcast(Band);
		TextQuality->SetText(FormatQunits(Quality));
	}
}

void UEW_EQBand::OnAmountDbChanged(float NewValue)
{
	if (IsValidDataContext())
	{
		float AmountDb = 96.f * NewValue - 48.f;
		Band->SetAmountDb(AmountDb);
		RootWidget->GetEvent_BandChanged().Broadcast(Band);
		TextAmountDb->SetText(FormatDb(AmountDb));
	}
}

void UEW_EQBand::OnBandSelectionChanged(TSharedPtr<FEQBand> InBand)
{
	BorderSelectionMarker->SetVisibility(InBand == Band 
										 ? ESlateVisibility::Visible
										 : ESlateVisibility::Hidden);
}

void UEW_EQBand::OnBandChanged(TSharedPtr<FEQBand> InBand)
{
	if (Band == InBand)
	{
		KnobLoudnessCompensation->SetValue((Band->GetLoudCompDb() + 48.f) / 96.f);
		KnobFrequency->SetValue(MathLogTool::TwentiethsToTribel(Band->GetFrequency()));
		KnobQuality->SetValue(MathLogTool::ThousandsToHexabel(Band->GetQuality()));
		KnobAmountDb->SetValue((Band->GetAmountDb() + 48.f) / 96.f);
		TextLoudnessCompensation->SetText(FormatDb(Band->GetLoudCompDb()));
		TextFrequency->SetText(FormatHz(Band->GetFrequency()));
		TextQuality->SetText(FormatQunits(Band->GetQuality()));
		TextAmountDb->SetText(FormatDb(Band->GetAmountDb()));
	}
}

