// Sonafridge 2022

#include "EW_EQBandPopup.h"

#include "Sonafridge/Widgets/NaveledKnob.h"
#include "Sonafridge/Widgets/ToggleKnob.h"
#include "Sonafridge/MathTools.h"
#include "EW_EQ.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"
#include "Misc/DefaultValueHelper.h"
#include "AudioDevice.h"

enum class ESignMode { NegativeOnly, Always };


FText FloatToText(float     InValue,
                  int32     NumDigits,
                  ESignMode SignMode = ESignMode::NegativeOnly)
{
	return UKismetTextLibrary::Conv_FloatToText(InValue,
	                                            HalfFromZero,
	                                            SignMode == ESignMode::Always,
	                                            false,
	                                            1,
	                                            324,
	                                            NumDigits,
	                                            NumDigits);
}

void UEW_EQBandPopup::Init(UEW_EQ* InRootWidget, TSharedPtr<IEQSettings> InSettings)
{
	RootWidget = InRootWidget;
	Settings = InSettings;
	RootWidget->GetEvent_BandChanging().AddUObject(this, &UEW_EQBandPopup::OnBandChanging);
	RootWidget->GetEvent_BandChanged().AddUObject(this, &UEW_EQBandPopup::OnBandChanged);
	RootWidget->GetEvent_BandSelectionChanged().AddUObject(this, &UEW_EQBandPopup::OnBandSelectionChanged);
}

void UEW_EQBandPopup::FollowBand()
{
	if (!RootWidget)
	{
		return;
	}

	FVector2D BandWPos = GetBandWPos();
	FVector2D ParentSize = RootWidget->GetLastSize();

	FVector2D Size = GetDesiredSize();
	FVector2D Position = BandWPos;
	Position.X -= .5f * Size.X;
	if (BandWPos.Y > Size.Y && BandWPos.Y < .5f * ParentSize.Y)
	{
		Position.Y -= Size.Y;
	}
	if (BandWPos.Y > ParentSize.Y - Size.Y)
	{
		Position.Y -= Size.Y;
	}

	Position.X = FMath::Clamp(Position.X, 0.f, ParentSize.X - Size.X);

	UCanvasPanelSlot* CanvasSlot = static_cast<UCanvasPanelSlot*>(Slot);
	if (CanvasSlot)
	{
		CanvasSlot->SetPosition(Position);
	}
}

void UEW_EQBandPopup::NativeConstruct()
{
	Super::NativeConstruct();

	NaveledKnobFrequency->GetEvent_KnobEntrance().AddUObject(this, &UEW_EQBandPopup::OnFrequencyEntrance);
	NaveledKnobAmount->GetEvent_KnobEntrance().AddUObject(this, &UEW_EQBandPopup::OnAmountEntrance);
	NaveledKnobQuality->GetEvent_KnobEntrance().AddUObject(this, &UEW_EQBandPopup::OnQualityEntrance);
	ToggleKnobMakeupGain->GetEvent_KnobEntrance().AddUObject(this, &UEW_EQBandPopup::OnMakeupGainEntrance);

	NaveledKnobFrequency->GetEvent_NavelEntrance().AddUObject(this, &UEW_EQBandPopup::OnListenEntrance);
	NaveledKnobAmount->GetEvent_NavelEntrance().AddUObject(this, &UEW_EQBandPopup::OnBandTypeEntrance);
	NaveledKnobQuality->GetEvent_NavelEntrance().AddUObject(this, &UEW_EQBandPopup::OnButtonRemoveEntrance);
	ToggleKnobMakeupGain->GetEvent_NavelEntrance().AddUObject(this, &UEW_EQBandPopup::OnToggleOnOffEntrance);

	NaveledKnobFrequency->GetEvent_NavelExit().AddUObject(this, &UEW_EQBandPopup::OnListenExit);
	NaveledKnobAmount->GetEvent_NavelExit().AddUObject(this, &UEW_EQBandPopup::OnBandTypeExit);
	NaveledKnobQuality->GetEvent_NavelExit().AddUObject(this, &UEW_EQBandPopup::OnButtonRemoveExit);
	ToggleKnobMakeupGain->GetEvent_NavelExit().AddUObject(this, &UEW_EQBandPopup::OnToggleOnOffExit);

	NaveledKnobFrequency->GetEvent_KnobValueChanged().AddUObject(this, &UEW_EQBandPopup::OnFrequencyDelta);
	NaveledKnobAmount->GetEvent_KnobValueChanged().AddUObject(this, &UEW_EQBandPopup::OnAmountDelta);
	NaveledKnobQuality->GetEvent_KnobValueChanged().AddUObject(this, &UEW_EQBandPopup::OnQualityDelta);
	ToggleKnobMakeupGain->GetEvent_KnobValueChanged().AddUObject(this, &UEW_EQBandPopup::OnMakeupGainDelta);

	NaveledKnobFrequency->GetEvent_NavelValueChanged().AddUObject(this, &UEW_EQBandPopup::OnListenDelta);
	NaveledKnobAmount->GetEvent_NavelValueChanged().AddUObject(this, &UEW_EQBandPopup::OnBandTypeChanged);
	NaveledKnobQuality->GetEvent_NavelValueChanged().AddUObject(this, &UEW_EQBandPopup::OnToggleNavelRemoveValueChanged);
	ToggleKnobMakeupGain->GetEvent_NavelValueChanged().AddUObject(this, &UEW_EQBandPopup::OnToggleNavelOnOffValueChanged);

	NaveledKnobQuality->GetEvent_NavelClick().AddUObject(this, &UEW_EQBandPopup::OnRemoveClick);
	ToggleKnobMakeupGain->GetEvent_ToggleStateChanged().AddUObject(this, &UEW_EQBandPopup::OnToggleNavelOnOffStateChanged);

	NaveledKnobFrequency->GetEvent_KnobCaptureFinished().AddUObject(this, &UEW_EQBandPopup::OnFrequencyCommit);
	NaveledKnobAmount->GetEvent_KnobCaptureFinished().AddUObject(this, &UEW_EQBandPopup::OnAmountCommit);
	NaveledKnobQuality->GetEvent_KnobCaptureFinished().AddUObject(this, &UEW_EQBandPopup::OnQualityCommit);
	ToggleKnobMakeupGain->GetEvent_KnobCaptureFinished().AddUObject(this, &UEW_EQBandPopup::OnMakeupGainCommit);
	NaveledKnobFrequency->GetEvent_NavelCaptureStarted().AddUObject(this, &UEW_EQBandPopup::OnListenStarted);
	NaveledKnobFrequency->GetEvent_NavelCaptureFinished().AddUObject(this, &UEW_EQBandPopup::OnListenFinished);

	TextBoxValue->OnTextCommitted.AddDynamic(this, &UEW_EQBandPopup::OnTextCommitted);
}

void UEW_EQBandPopup::OnSizeChanged(const FVector2D& OldSize, const FVector2D& NewSize)
{
	FollowBand();
}

void UEW_EQBandPopup::OnFrequencyEntrance()
{
	FocusMode = EBandPopupFocusMode::Frequency;
	TextBlockKey->SetText(FText::FromString(TEXT("Frequency")));
	TextBoxValue->SetText(FloatToText(Band->GetFrequency(), 1));
	NaveledKnobFrequency->RefreshVisual();
}

void UEW_EQBandPopup::OnAmountEntrance()
{
	FocusMode = EBandPopupFocusMode::Amount;
	TextBlockKey->SetText(FText::FromString(TEXT("Amount")));
	TextBoxValue->SetText(FloatToText(Band->GetAmountDb(), 2, ESignMode::Always));
	NaveledKnobAmount->RefreshVisual();
}

void UEW_EQBandPopup::OnQualityEntrance()
{
	FocusMode = EBandPopupFocusMode::Quality;
	TextBlockKey->SetText(FText::FromString(TEXT("Quality")));
	TextBoxValue->SetText(FloatToText(Band->GetQuality(), 3));
	NaveledKnobQuality->RefreshVisual();
}

void UEW_EQBandPopup::OnMakeupGainEntrance()
{
	FocusMode = EBandPopupFocusMode::Makeup;
	TextBlockKey->SetText(FText::FromString(TEXT("Makeup Gain")));
	TextBoxValue->SetText(FloatToText(Band->GetLoudCompDb(), 2, ESignMode::Always));
	ToggleKnobMakeupGain->RefreshVisual();
}

void UEW_EQBandPopup::OnListenEntrance()
{
	FocusMode = EBandPopupFocusMode::Frequency;
	TextBlockKey->SetText(FText::FromString(TEXT("Frequency")));
	TextBoxValue->SetText(FloatToText(Band->GetFrequency(), 1));
	NaveledKnobFrequency->RefreshVisual();
}

void UEW_EQBandPopup::OnBandTypeEntrance()
{
	UEnum* BandTypeEnum = StaticEnum<EBandType>();
	if (BandTypeEnum)
	{
		FocusMode = EBandPopupFocusMode::None;
		TextBlockKey->SetText(FText::FromString(TEXT("Band Type")));

		FText EnumText = BandTypeEnum->GetDisplayNameTextByValue(static_cast<int64>(Band->GetType()));
		if (!EnumText.IsEmpty())
		{
			TextBoxValue->SetText(EnumText);
		}			
	}

	NaveledKnobAmount->RefreshVisual();
}

void UEW_EQBandPopup::OnButtonRemoveEntrance()
{
	FocusMode = EBandPopupFocusMode::Quality;
	TextBlockKey->SetText(FText::FromString(TEXT("Quality")));
	TextBoxValue->SetText(FloatToText(Band->GetQuality(), 3));
	NaveledKnobQuality->RefreshVisual();
}

void UEW_EQBandPopup::OnToggleOnOffEntrance()
{
	FocusMode = EBandPopupFocusMode::Makeup;
	TextBlockKey->SetText(FText::FromString(TEXT("Makeup Gain")));
	TextBoxValue->SetText(FloatToText(Band->GetLoudCompDb(), 2, ESignMode::Always));
	ToggleKnobMakeupGain->RefreshVisual();
}

void UEW_EQBandPopup::OnListenExit()
{
}

void UEW_EQBandPopup::OnBandTypeExit()
{
}

void UEW_EQBandPopup::OnButtonRemoveExit()
{
}

void UEW_EQBandPopup::OnToggleOnOffExit()
{
}

void UEW_EQBandPopup::OnBandSelectionChanged(TSharedPtr<FEQBand> InBand)
{
	Band = InBand;
	SetVisibility(Band ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	FollowBand();
	RefreshVisual();
}

void UEW_EQBandPopup::OnBandChanging(TSharedPtr<FEQBand> InBand)
{
	RefreshVisual();
}

void UEW_EQBandPopup::OnBandChanged(TSharedPtr<FEQBand> InBand)
{
	RefreshVisual();
	FollowBand();
}

void UEW_EQBandPopup::OnFrequencyDelta(float OldFrequency01, float NewFrequency01)
{
	float NewFrequency = MathLogTool::TribelToTwentieths(NewFrequency01);
	Band->SetFrequency(NewFrequency);
	TextBoxValue->SetText(FloatToText(NewFrequency, 1));
	RootWidget->GetEvent_BandChanging().Broadcast(Band);
}

void UEW_EQBandPopup::OnAmountDelta(float OldAmount01, float NewAmount01)
{
	float NewAmountDb = NewAmount01 * 96.f - 48.f;
	Band->SetAmountDb(NewAmountDb);
	TextBoxValue->SetText(FloatToText(NewAmountDb, 2, ESignMode::Always));
	RootWidget->GetEvent_BandChanging().Broadcast(Band);
}

void UEW_EQBandPopup::OnQualityDelta(float OldQuality01, float NewQuality01)
{
	float NewQuality = MathLogTool::HexabelToThousands(NewQuality01);
	Band->SetQuality(NewQuality);
	TextBoxValue->SetText(FloatToText(NewQuality, 3));
	RootWidget->GetEvent_BandChanging().Broadcast(Band);
}

void UEW_EQBandPopup::OnMakeupGainDelta(float OldMakeupGain01, float NewMakeupGain01)
{
	float NewMakeupGainDb = NewMakeupGain01 * 96.f - 48.f;
	Band->SetLoudCompDb(NewMakeupGainDb);
	TextBoxValue->SetText(FloatToText(NewMakeupGainDb, 2, ESignMode::Always));
	RootWidget->GetEvent_BandChanging().Broadcast(Band);
}

void UEW_EQBandPopup::OnListenDelta(float FrequencyDelta)
{
	float OldFrequency01 = MathLogTool::TwentiethsToTribel(Band->GetFrequency());
	float NewFrequency01 = OldFrequency01 + FrequencyDelta;
	NewFrequency01 = FMath::Clamp(NewFrequency01, 0.f, 1.f);
	NaveledKnobFrequency->SetValue01(NewFrequency01);

	float NewFrequency = MathLogTool::TribelToTwentieths(NewFrequency01);
	Band->SetFrequency(NewFrequency);
	TextBoxValue->SetText(FloatToText(NewFrequency, 1));
	RootWidget->GetEvent_BandChanging().Broadcast(Band);
}

void UEW_EQBandPopup::OnBandTypeChanged(float BandTypeDeltaAsFloat)
{
	UEnum* BandPopupTypeEnum = StaticEnum<EBandPopupType>();
	if (BandPopupTypeEnum)
	{
		BandTypeFloat += 10.f * BandTypeDeltaAsFloat;

		BandTypeFloat = FMath::Clamp(BandTypeFloat,
		                             static_cast<float>(EBandPopupType::LowCut),
		                             static_cast<float>(BandPopupTypeEnum->GetMaxEnumValue()) - 1.f);

		EBandPopupType BandPopupType = static_cast<EBandPopupType>(BandTypeFloat);
		Band->SetType(GetBandTypeByPopupType(BandPopupType));
		NaveledKnobAmount->HoverIcon = GetBandIconByType(BandPopupType);
		NaveledKnobAmount->RefreshVisual();

		FText EnumText = BandPopupTypeEnum->GetDisplayNameTextByValue(static_cast<int64>(BandPopupType));
		if (!EnumText.IsEmpty())
		{
			TextBoxValue->SetText(EnumText);
		}			
	}
}

void UEW_EQBandPopup::OnRemoveClick()
{
	// TODO: Inject IEQSettings, remove band in there.
	UE_LOG(LogTemp, Log, TEXT("Remove band stub. "));
}

void UEW_EQBandPopup::OnToggleNavelRemoveValueChanged(float QualityDelta01)
{
	float OldQuality01 = MathLogTool::ThousandsToHexabel(Band->GetQuality());
	float NewQuality01 = OldQuality01 + QualityDelta01;
	NewQuality01 = FMath::Clamp(NewQuality01, 0.f, 1.f);
	NaveledKnobQuality->SetValue01(NewQuality01);

	float NewQuality = MathLogTool::HexabelToThousands(NewQuality01);
	Band->SetQuality(NewQuality);
	TextBoxValue->SetText(FloatToText(NewQuality, 3));
	RootWidget->GetEvent_BandChanging().Broadcast(Band);
}

void UEW_EQBandPopup::OnToggleNavelOnOffValueChanged(float MakeupGainDelta01)
{
	float OldMakeupGain01 = (Band->GetLoudCompDb() + 48.f) / 96.f;
	float NewMakeupGain01 = OldMakeupGain01 + MakeupGainDelta01;
	NewMakeupGain01 = FMath::Clamp(NewMakeupGain01, 0.f, 1.f);
	ToggleKnobMakeupGain->SetValue01(NewMakeupGain01);

	float NewMakeupGainDb = NewMakeupGain01 * 96.f - 48.f;
	Band->SetLoudCompDb(NewMakeupGainDb);
	TextBoxValue->SetText(FloatToText(NewMakeupGainDb, 2, ESignMode::Always));
	RootWidget->GetEvent_BandChanging().Broadcast(Band);
}

void UEW_EQBandPopup::OnToggleNavelOnOffStateChanged(bool bOldValue, bool bNewValue)
{
	Band->SetIsEnabled(bNewValue);
}

void UEW_EQBandPopup::OnFrequencyCommit()
{
	RootWidget->GetEvent_BandChanged().Broadcast(Band);
}

void UEW_EQBandPopup::OnAmountCommit()
{
	RootWidget->GetEvent_BandChanged().Broadcast(Band);
}

void UEW_EQBandPopup::OnQualityCommit()
{
	RootWidget->GetEvent_BandChanged().Broadcast(Band);
}

void UEW_EQBandPopup::OnMakeupGainCommit()
{
	RootWidget->GetEvent_BandChanged().Broadcast(Band);
}

void UEW_EQBandPopup::OnListenStarted()
{
	BandTypeBeforeListenTime = Band->GetType();
	Band->SetType(EBandType::BandPass);
}

void UEW_EQBandPopup::OnListenFinished()
{
	Band->SetType(BandTypeBeforeListenTime);
	RootWidget->GetEvent_BandChanged().Broadcast(Band);
}

void UEW_EQBandPopup::OnTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (FocusMode == EBandPopupFocusMode::Frequency)
	{
		float NewFrequency;
		if (FDefaultValueHelper::ParseFloat(Text.ToString(), NewFrequency))
		{
			Band->SetFrequency(NewFrequency);
			const float NewFrequency01 = MathLogTool::TwentiethsToTribel(NewFrequency);
			NaveledKnobFrequency->SetValue01(NewFrequency01);
		}
	}
	else if (FocusMode == EBandPopupFocusMode::Amount)
	{
		float NewAmountDb;
		if (FDefaultValueHelper::ParseFloat(Text.ToString(), NewAmountDb))
		{
			Band->SetAmountDb(NewAmountDb);
			const float NewAmount01 = (NewAmountDb + 48.f) / 96.f;
			NaveledKnobAmount->SetValue01(NewAmount01);
		}
	}
	else if (FocusMode == EBandPopupFocusMode::Quality)
	{
		float NewQuality;
		if (FDefaultValueHelper::ParseFloat(Text.ToString(), NewQuality))
		{
			Band->SetQuality(NewQuality);
			const float NewQuality01 = MathLogTool::ThousandsToHexabel(NewQuality);
			NaveledKnobQuality->SetValue01(NewQuality01);
		}
	}
	else if (FocusMode == EBandPopupFocusMode::Makeup)
	{
		float NewMakeupGainDb;
		if (FDefaultValueHelper::ParseFloat(Text.ToString(), NewMakeupGainDb))
		{
			Band->SetLoudCompDb(NewMakeupGainDb);
			const float NewMakeupGain01 = (NewMakeupGainDb + 48.f) / 96.f;
			ToggleKnobMakeupGain->SetValue01(NewMakeupGain01);
		}
	}

	RootWidget->GetEvent_BandChanged().Broadcast(Band);
}

int32 UEW_EQBandPopup::NativePaint(const FPaintArgs&        Args,
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

	UEW_EQBandPopup* MutableThis = const_cast<UEW_EQBandPopup*>(this);
	FVector2D Size = AllottedGeometry.GetLocalSize();

	if (Size != LastSize)
	{
		MutableThis->OnSizeChanged(LastSize, Size);
	}

	MutableThis->LastSize = Size;


	return Result;
}

void UEW_EQBandPopup::RefreshVisual()
{
	if (Band)
	{
		const float Frequency01 = MathLogTool::TwentiethsToTribel(Band->GetFrequency());
		const float Amount01 = (Band->GetAmountDb() + 48.f) / 96.f;
		const float Quality01 = MathLogTool::ThousandsToHexabel(Band->GetQuality());
		const float MakeupGain01 = (Band->GetLoudCompDb() + 48.f) / 96.f;

		NaveledKnobFrequency->SetValue01(Frequency01);
		NaveledKnobAmount->SetValue01(Amount01);
		NaveledKnobQuality->SetValue01(Quality01);
		ToggleKnobMakeupGain->SetValue01(MakeupGain01);
		ToggleKnobMakeupGain->SetIsOn(Band->GetIsEnabled());

		if (FocusMode == EBandPopupFocusMode::Frequency)
		{
			TextBlockKey->SetText(FText::FromString(TEXT("Frequency")));
			TextBoxValue->SetText(FloatToText(Band->GetFrequency(), 1));
		}
		else if (FocusMode == EBandPopupFocusMode::Amount)
		{
			TextBlockKey->SetText(FText::FromString(TEXT("Amount")));
			TextBoxValue->SetText(FloatToText(Band->GetAmountDb(), 2, ESignMode::Always));
		}
		else if (FocusMode == EBandPopupFocusMode::Quality)
		{
			TextBlockKey->SetText(FText::FromString(TEXT("Quality")));
			TextBoxValue->SetText(FloatToText(Band->GetQuality(), 3));
		}
		else if (FocusMode == EBandPopupFocusMode::Makeup)
		{
			TextBlockKey->SetText(FText::FromString(TEXT("Makeup Gain")));
			TextBoxValue->SetText(FloatToText(Band->GetLoudCompDb(), 2, ESignMode::Always));
		}
	}
}

EBandType UEW_EQBandPopup::GetBandTypeByPopupType(EBandPopupType InBandPopupType)
{
	UEnum* BandPopupEnum = StaticEnum<EBandPopupType>();
	UEnum* BandEnum = StaticEnum<EBandType>();

	if (BandEnum && BandPopupEnum)
	{
		FName Name = BandPopupEnum->GetNameByValue(static_cast<int64>(InBandPopupType));
		return static_cast<EBandType>(BandEnum->GetValueByName(Name));
	}
	else
	{
		return EBandType::None;
	}
}

UTexture* UEW_EQBandPopup::GetBandIconByType(EBandPopupType InBandPopup)
{
	switch (InBandPopup)
	{
		case EBandPopupType::LowCut: return IconLowCut;
		case EBandPopupType::HighCut: return IconHighCut;
		case EBandPopupType::LowShelf: return IconLowShelf;
		case EBandPopupType::HighShelf: return IconHighShelf;
		case EBandPopupType::BandCut: return IconBandCut;
		case EBandPopupType::Notch: return IconNotch;
		default: return nullptr;
	}
}

FVector2D UEW_EQBandPopup::GetBandWPos()
{
	if (Band)
	{
		float F = Band->GetFrequency();
		float NX = MathLogTool::TwentiethsToTribel(F);
		float NY = (UEW_EQ::DynamicMax - Settings->DtftDb(F))
		         / (UEW_EQ::DynamicMax - UEW_EQ::DynamicMin);
		float WX = NX * RootWidget->GetLastSize().X;
		float WY = NY * RootWidget->GetLastSize().Y;

		return { WX, WY };
	}

	return {};
}

