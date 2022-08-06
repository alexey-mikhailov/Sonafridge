// Sonafridge 2022

#include "EW_SonaQBandPopup.h"

#include "Sonafridge/Widgets/NaveledKnob.h"
#include "Sonafridge/Widgets/ToggleKnob.h"
#include "Sonafridge/Tools/MathTools.h"
#include "EW_SonaQ.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"
#include "Misc/DefaultValueHelper.h"
#include "AudioDevice.h"
#include "Kismet/KismetInputLibrary.h"


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

void UEW_SonaQBandPopup::Init(UEW_SonaQ* InRootWidget, TSharedPtr<FVM_SonaQ> InViewModel)
{
	RootWidget = InRootWidget;
	ViewModel = InViewModel;
	ViewModel->GetEvent_BandChanging().AddUObject(this, &UEW_SonaQBandPopup::OnBandChanging);
	ViewModel->GetEvent_BandChanged().AddUObject(this, &UEW_SonaQBandPopup::OnBandChanged);
	ViewModel->GetEvent_BandSelectionChanged().AddUObject(this, &UEW_SonaQBandPopup::OnBandSelectionChanged);
}

void UEW_SonaQBandPopup::FollowBand()
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

	// Do not use 0.5 as half size because
	// it may cause more frequently widget flickering. 
	constexpr float HalfSize = .55;

	if (BandWPos.Y > Size.Y && BandWPos.Y < HalfSize * ParentSize.Y)
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

void UEW_SonaQBandPopup::NativeConstruct()
{
	Super::NativeConstruct();

	NaveledKnobFrequency->GetEvent_KnobEntrance().AddUObject(this, &UEW_SonaQBandPopup::OnFrequencyEntrance);
	NaveledKnobAmount->GetEvent_KnobEntrance().AddUObject(this, &UEW_SonaQBandPopup::OnAmountEntrance);
	NaveledKnobQuality->GetEvent_KnobEntrance().AddUObject(this, &UEW_SonaQBandPopup::OnQualityEntrance);
	ToggleKnobMakeupGain->GetEvent_KnobEntrance().AddUObject(this, &UEW_SonaQBandPopup::OnMakeupGainEntrance);

	NaveledKnobFrequency->GetEvent_NavelEntrance().AddUObject(this, &UEW_SonaQBandPopup::OnListenEntrance);
	NaveledKnobAmount->GetEvent_NavelEntrance().AddUObject(this, &UEW_SonaQBandPopup::OnBandTypeEntrance);
	NaveledKnobQuality->GetEvent_NavelEntrance().AddUObject(this, &UEW_SonaQBandPopup::OnButtonRemoveEntrance);
	ToggleKnobMakeupGain->GetEvent_NavelEntrance().AddUObject(this, &UEW_SonaQBandPopup::OnToggleOnOffEntrance);

	NaveledKnobFrequency->GetEvent_NavelExit().AddUObject(this, &UEW_SonaQBandPopup::OnListenExit);
	NaveledKnobAmount->GetEvent_NavelExit().AddUObject(this, &UEW_SonaQBandPopup::OnBandTypeExit);
	NaveledKnobQuality->GetEvent_NavelExit().AddUObject(this, &UEW_SonaQBandPopup::OnButtonRemoveExit);
	ToggleKnobMakeupGain->GetEvent_NavelExit().AddUObject(this, &UEW_SonaQBandPopup::OnToggleOnOffExit);

	NaveledKnobFrequency->GetEvent_KnobValueChanged().AddUObject(this, &UEW_SonaQBandPopup::OnFrequencyDelta);
	NaveledKnobAmount->GetEvent_KnobValueChanged().AddUObject(this, &UEW_SonaQBandPopup::OnAmountDelta);
	NaveledKnobQuality->GetEvent_KnobValueChanged().AddUObject(this, &UEW_SonaQBandPopup::OnQualityDelta);
	ToggleKnobMakeupGain->GetEvent_KnobValueChanged().AddUObject(this, &UEW_SonaQBandPopup::OnMakeupGainDelta);

	NaveledKnobFrequency->GetEvent_NavelValueChanged().AddUObject(this, &UEW_SonaQBandPopup::OnListenDelta);
	NaveledKnobAmount->GetEvent_NavelValueChanged().AddUObject(this, &UEW_SonaQBandPopup::OnBandTypeChanged);
	NaveledKnobQuality->GetEvent_NavelValueChanged().AddUObject(this, &UEW_SonaQBandPopup::OnToggleNavelRemoveValueChanged);
	ToggleKnobMakeupGain->GetEvent_NavelValueChanged().AddUObject(this, &UEW_SonaQBandPopup::OnToggleNavelOnOffValueChanged);

	NaveledKnobQuality->GetEvent_NavelClick().AddUObject(this, &UEW_SonaQBandPopup::OnRemoveClick);
	ToggleKnobMakeupGain->GetEvent_ToggleStateChanged().AddUObject(this, &UEW_SonaQBandPopup::OnToggleNavelOnOffStateChanged);

	NaveledKnobFrequency->GetEvent_KnobCaptureStarted().AddUObject(this, &UEW_SonaQBandPopup::OnFrequencyCapture);
	NaveledKnobFrequency->GetEvent_KnobCaptureFinished().AddUObject(this, &UEW_SonaQBandPopup::OnFrequencyCommit);
	NaveledKnobAmount->GetEvent_KnobCaptureStarted().AddUObject(this, &UEW_SonaQBandPopup::OnAmountCapture);
	NaveledKnobAmount->GetEvent_KnobCaptureFinished().AddUObject(this, &UEW_SonaQBandPopup::OnAmountCommit);
	NaveledKnobQuality->GetEvent_KnobCaptureStarted().AddUObject(this, &UEW_SonaQBandPopup::OnQualityCapture);
	NaveledKnobQuality->GetEvent_KnobCaptureFinished().AddUObject(this, &UEW_SonaQBandPopup::OnQualityCommit);
	ToggleKnobMakeupGain->GetEvent_KnobCaptureStarted().AddUObject(this, &UEW_SonaQBandPopup::OnMakeupGainCapture);
	ToggleKnobMakeupGain->GetEvent_KnobCaptureFinished().AddUObject(this, &UEW_SonaQBandPopup::OnMakeupGainCommit);
	NaveledKnobFrequency->GetEvent_NavelCaptureStarted().AddUObject(this, &UEW_SonaQBandPopup::OnListenStarted);
	NaveledKnobFrequency->GetEvent_NavelCaptureFinished().AddUObject(this, &UEW_SonaQBandPopup::OnListenFinished);

	NaveledKnobFrequency->GetEvent_KnobClick().AddUObject(this, &UEW_SonaQBandPopup::OnFrequencyClick);
	NaveledKnobAmount->GetEvent_KnobClick().AddUObject(this, &UEW_SonaQBandPopup::OnAmountClick);
	NaveledKnobQuality->GetEvent_KnobClick().AddUObject(this, &UEW_SonaQBandPopup::OnQualityClick);
	ToggleKnobMakeupGain->GetEvent_KnobClick().AddUObject(this, &UEW_SonaQBandPopup::OnMakeupClick);

	TextBoxValue->OnTextCommitted.AddDynamic(this, &UEW_SonaQBandPopup::OnTextCommitted);
}

void UEW_SonaQBandPopup::OnFrequencyEntrance()
{
	FocusMode = EBandPopupFocusMode::Frequency;
	TextBlockKey->SetText(FText::FromString(TEXT("Frequency")));
	TextBoxValue->SetText(FloatToText(Band->GetFrequency(), 1));
	NaveledKnobFrequency->RefreshVisual();
}

void UEW_SonaQBandPopup::OnAmountEntrance()
{
	FocusMode = EBandPopupFocusMode::Amount;
	TextBlockKey->SetText(FText::FromString(TEXT("Amount")));
	TextBoxValue->SetText(FloatToText(Band->GetAmountDb(), 2, ESignMode::Always));
	NaveledKnobAmount->RefreshVisual();
}

void UEW_SonaQBandPopup::OnQualityEntrance()
{
	FocusMode = EBandPopupFocusMode::Quality;
	TextBlockKey->SetText(FText::FromString(TEXT("Quality")));
	TextBoxValue->SetText(FloatToText(Band->GetQuality(), 3));
	NaveledKnobQuality->RefreshVisual();
}

void UEW_SonaQBandPopup::OnMakeupGainEntrance()
{
	FocusMode = EBandPopupFocusMode::Makeup;
	TextBlockKey->SetText(FText::FromString(TEXT("Makeup Gain")));
	TextBoxValue->SetText(FloatToText(Band->GetMakeupDb(), 2, ESignMode::Always));
	ToggleKnobMakeupGain->RefreshVisual();
}

void UEW_SonaQBandPopup::OnListenEntrance()
{
	FocusMode = EBandPopupFocusMode::Frequency;
	TextBlockKey->SetText(FText::FromString(TEXT("Listen band")));
	TextBoxValue->SetText(FloatToText(Band->GetFrequency(), 1));
	NaveledKnobFrequency->RefreshVisual();
}

void UEW_SonaQBandPopup::OnBandTypeEntrance()
{
	UEnum* BandTypeEnum = StaticEnum<EEQBandType>();
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

void UEW_SonaQBandPopup::OnButtonRemoveEntrance()
{
	FocusMode = EBandPopupFocusMode::Quality;
	TextBlockKey->SetText(FText::FromString(TEXT("Remove band")));
	TextBoxValue->SetText(FText::FromString(TEXT("Remove band")));
	NaveledKnobQuality->RefreshVisual();
}

void UEW_SonaQBandPopup::OnToggleOnOffEntrance()
{
	FocusMode = EBandPopupFocusMode::Makeup;
	TextBlockKey->SetText(FText::FromString(TEXT("On/off band")));
	TextBoxValue->SetText(FText::FromString(TEXT("On/off band")));
	ToggleKnobMakeupGain->RefreshVisual();
}

void UEW_SonaQBandPopup::OnListenExit()
{
}

void UEW_SonaQBandPopup::OnBandTypeExit()
{
}

void UEW_SonaQBandPopup::OnButtonRemoveExit()
{
}

void UEW_SonaQBandPopup::OnToggleOnOffExit()
{
}

void UEW_SonaQBandPopup::OnBandSelectionChanged(TSharedPtr<FVM_SonaQBand> InBand)
{
	Band = InBand;
	SetVisibility(Band ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	if (Band)
	{
		BandPopupTypeFloat = static_cast<float>(GetPopupTypeByBandType(Band->GetType()));
		TextBoxValue->SetIsEnabled(Band->GetIsEnabled());
	}

	FollowBand();
	RefreshVisual();
}

void UEW_SonaQBandPopup::OnBandChanging(TSharedPtr<FVM_SonaQBand> InBand)
{
	RefreshVisual();
	FollowBand();
}

void UEW_SonaQBandPopup::OnBandChanged(TSharedPtr<FVM_SonaQBand> InBand)
{
	RefreshVisual();
	FollowBand();
}

void UEW_SonaQBandPopup::OnFrequencyDelta(float OldFrequency01, 
                                          float NewFrequency01,
                                          const FPointerEvent& InMouseEvent,
                                          bool& bInOutHaveAllHandlersAccepted)
{
	if (Band->GetIsEnabled())
	{
		float NewFrequency = MathLogTool::TribelToTwentieths(NewFrequency01);
		Band->SetFrequency(NewFrequency);
		TextBoxValue->SetText(FloatToText(NewFrequency, 1));
		ViewModel->GetEvent_BandChanging().Broadcast(Band);

		TryAutoMakeup(InMouseEvent);
	}
	else
	{
		bInOutHaveAllHandlersAccepted = false;
	}
}

void UEW_SonaQBandPopup::OnAmountDelta(float OldAmount01, 
									   float NewAmount01,
									   const FPointerEvent& InMouseEvent, 
									   bool& bInOutHaveAllHandlersAccepted)
{
	if (Band->GetIsEnabled())
	{
		float NewAmountDb = NewAmount01 * 96.f - 48.f;
		Band->SetAmountDb(NewAmountDb);
		TextBoxValue->SetText(FloatToText(NewAmountDb, 2, ESignMode::Always));
		ViewModel->GetEvent_BandChanging().Broadcast(Band);

		TryAutoMakeup(InMouseEvent);
	}
	else
	{
		bInOutHaveAllHandlersAccepted = false;
	}
}

void UEW_SonaQBandPopup::OnQualityDelta(float OldQuality01, 
										float NewQuality01,
										const FPointerEvent& InMouseEvent, 
										bool& bInOutHaveAllHandlersAccepted)
{
	if (Band->GetIsEnabled())
	{
		float NewQuality = MathLogTool::HexabelToThousands(NewQuality01);
		Band->SetQuality(NewQuality);
		TextBoxValue->SetText(FloatToText(NewQuality, 3));
		ViewModel->GetEvent_BandChanging().Broadcast(Band);

		TryAutoMakeup(InMouseEvent);
	}
	else
	{
		bInOutHaveAllHandlersAccepted = false;
	}
}

void UEW_SonaQBandPopup::OnMakeupGainDelta(float OldMakeupGain01,
                                           float NewMakeupGain01,
										   const FPointerEvent& InMouseEvent,
                                           bool& bInOutHaveAllHandlersAccepted)
{
	if (Band->GetIsEnabled())
	{
		float NewMakeupGainDb = NewMakeupGain01 * 96.f - 48.f;
		Band->SetMakeupDb(NewMakeupGainDb);
		TextBoxValue->SetText(FloatToText(NewMakeupGainDb, 2, ESignMode::Always));
		ViewModel->GetEvent_BandChanging().Broadcast(Band);
	}
	else
	{
		bInOutHaveAllHandlersAccepted = false;
	}
}

void UEW_SonaQBandPopup::OnListenDelta(float FrequencyDelta, const FPointerEvent& InMouseEvent)
{
	if (Band->GetIsEnabled())
	{
		float OldFrequency01 = MathLogTool::TwentiethsToTribel(Band->GetFrequency());
		float NewFrequency01 = OldFrequency01 + FrequencyDelta;
		NewFrequency01 = FMath::Clamp(NewFrequency01, 0.f, 1.f);
		NaveledKnobFrequency->SetValue01(NewFrequency01);

		float NewFrequency = MathLogTool::TribelToTwentieths(NewFrequency01);
		Band->SetFrequency(NewFrequency);
		TextBoxValue->SetText(FloatToText(NewFrequency, 1));
		ViewModel->GetEvent_BandChanging().Broadcast(Band);

		TryAutoMakeup(InMouseEvent);
	}
}

void UEW_SonaQBandPopup::OnBandTypeChanged(float BandTypeDeltaAsFloat, const FPointerEvent& InMouseEvent)
{
	if (Band->GetIsEnabled())
	{
		UEnum* BandPopupTypeEnum = StaticEnum<EBandPopupType>();
		if (BandPopupTypeEnum)
		{
			BandPopupTypeFloat += 10.f * BandTypeDeltaAsFloat;

			BandPopupTypeFloat = FMath::Clamp(BandPopupTypeFloat,
										 static_cast<float>(EBandPopupType::CutLow),
										 static_cast<float>(BandPopupTypeEnum->GetMaxEnumValue()) - 1.f);

			EBandPopupType BandPopupType = static_cast<EBandPopupType>(BandPopupTypeFloat);
			Band->SetType(GetBandTypeByPopupType(BandPopupType));
			NaveledKnobAmount->HoverIcon = GetBandIconByType(BandPopupType);
			NaveledKnobAmount->RefreshVisual();

			FText EnumText = BandPopupTypeEnum->GetDisplayNameTextByValue(static_cast<int64>(BandPopupType));
			TextBoxValue->SetText(EnumText);

			ViewModel->GetEvent_BandChanging().Broadcast(Band);

			TryAutoMakeup(InMouseEvent);
		}
	}
}

void UEW_SonaQBandPopup::OnToggleNavelRemoveValueChanged(float QualityDelta01, const FPointerEvent& InMouseEvent)
{
	if (Band->GetIsEnabled())
	{
		float OldQuality01 = MathLogTool::ThousandsToHexabel(Band->GetQuality());
		float NewQuality01 = OldQuality01 + QualityDelta01;
		NewQuality01 = FMath::Clamp(NewQuality01, 0.f, 1.f);
		NaveledKnobQuality->SetValue01(NewQuality01);

		float NewQuality = MathLogTool::HexabelToThousands(NewQuality01);
		Band->SetQuality(NewQuality);
		TextBoxValue->SetText(FloatToText(NewQuality, 3));
		ViewModel->GetEvent_BandChanging().Broadcast(Band);

		TryAutoMakeup(InMouseEvent);
	}
}

void UEW_SonaQBandPopup::OnToggleNavelOnOffValueChanged(float MakeupGainDelta01, const FPointerEvent& InMouseEvent)
{
	if (Band->GetIsEnabled())
	{
		float OldMakeupGain01 = (Band->GetMakeupDb() + 48.f) / 96.f;
		float NewMakeupGain01 = OldMakeupGain01 + MakeupGainDelta01;
		NewMakeupGain01 = FMath::Clamp(NewMakeupGain01, 0.f, 1.f);
		ToggleKnobMakeupGain->SetValue01(NewMakeupGain01);

		float NewMakeupGainDb = NewMakeupGain01 * 96.f - 48.f;
		Band->SetMakeupDb(NewMakeupGainDb);
		TextBoxValue->SetText(FloatToText(NewMakeupGainDb, 2, ESignMode::Always));
		ViewModel->GetEvent_BandChanging().Broadcast(Band);
	}
}

void UEW_SonaQBandPopup::OnRemoveClick()
{
	ViewModel->RemoveBand(Band);
	ViewModel->SetSelectedBandIndex(-1);
	ViewModel->GetEvent_BandSelectionChanged().Broadcast(Band);
	ViewModel->GetEvent_BandRemoved().Broadcast(Band);
	ViewModel->GetEvent_BandChanged().Broadcast(Band);
}

void UEW_SonaQBandPopup::OnToggleNavelOnOffStateChanged(bool bOldValue, bool bNewValue)
{
	Band->SetIsEnabled(bNewValue);
	ViewModel->GetEvent_BandChanged().Broadcast(Band);
	TextBoxValue->SetIsEnabled(Band->GetIsEnabled());
}

void UEW_SonaQBandPopup::OnFrequencyCapture()
{
	PresstimeMakeupDb = Band->GetMakeupDb();
	PresstimeAvgDb = Band->GetResponseAvgDb();
}

void UEW_SonaQBandPopup::OnFrequencyCommit()
{
	if (Band->GetIsEnabled())
	{
		ViewModel->GetEvent_BandChanged().Broadcast(Band);
	}
}

void UEW_SonaQBandPopup::OnAmountCapture()
{
	PresstimeMakeupDb = Band->GetMakeupDb();
	PresstimeAvgDb = Band->GetResponseAvgDb();
}

void UEW_SonaQBandPopup::OnAmountCommit()
{
	if (Band->GetIsEnabled())
	{
		ViewModel->GetEvent_BandChanged().Broadcast(Band);
	}
}

void UEW_SonaQBandPopup::OnQualityCapture()
{
	PresstimeMakeupDb = Band->GetMakeupDb();
	PresstimeAvgDb = Band->GetResponseAvgDb();
}

void UEW_SonaQBandPopup::OnQualityCommit()
{
	if (Band->GetIsEnabled())
	{
		ViewModel->GetEvent_BandChanged().Broadcast(Band);
	}
}

void UEW_SonaQBandPopup::OnMakeupGainCapture()
{
	PresstimeMakeupDb = Band->GetMakeupDb();
	PresstimeAvgDb = Band->GetResponseAvgDb();
}

void UEW_SonaQBandPopup::OnMakeupGainCommit()
{
	if (Band->GetIsEnabled())
	{
		ViewModel->GetEvent_BandChanged().Broadcast(Band);
	}
}

void UEW_SonaQBandPopup::OnListenStarted()
{
	if (Band->GetIsEnabled())
	{
		const TArray<TSharedPtr<FVM_SonaQBand>>& Bands = ViewModel->GetBands();

		BandTypeBeforeListenTime = Band->GetType();
		BandMakeupBeforeListenTime = Band->GetMakeupDb();
		BandsActivityBeforeListenTime.SetNum(Bands.Num());

		for (int32 Index = 0; Index < Bands.Num(); ++Index)
		{
			TSharedPtr<FVM_SonaQBand> Band_ = Bands[Index];
			if (Band != Band_)
			{
				BandsActivityBeforeListenTime[Index] = Band_->GetIsEnabled();
				Band_->SetIsEnabled(false);
			}
		}

		Band->SetType(EEQBandType::PassBand);
		Band->SetMakeupCoeff(4.f * FMath::Pow(Band->GetQuality(), .25f));
		Band->SetQuality(2.f * Band->GetQuality());
	}
}

void UEW_SonaQBandPopup::OnListenFinished()
{
	if (Band->GetIsEnabled())
	{
		const TArray<TSharedPtr<FVM_SonaQBand>>& Bands = ViewModel->GetBands();

		for (int32 Index = 0; Index < Bands.Num(); ++Index)
		{
			TSharedPtr<FVM_SonaQBand> Band_ = Bands[Index];
			if (Band != Band_)
			{
				Band_->SetIsEnabled(BandsActivityBeforeListenTime[Index]);
			}
		}

		Band->SetType(BandTypeBeforeListenTime);
		Band->SetMakeupDb(BandMakeupBeforeListenTime);
		Band->SetQuality(.5f * Band->GetQuality());

		ViewModel->GetEvent_BandChanged().Broadcast(Band);
	}
}

void UEW_SonaQBandPopup::OnFrequencyClick()
{
	TextBoxValue->SetKeyboardFocus();
}

void UEW_SonaQBandPopup::OnAmountClick()
{
	TextBoxValue->SetKeyboardFocus();
}

void UEW_SonaQBandPopup::OnQualityClick()
{
	TextBoxValue->SetKeyboardFocus();
}

void UEW_SonaQBandPopup::OnMakeupClick()
{
	TextBoxValue->SetKeyboardFocus();
}

void UEW_SonaQBandPopup::OnTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (!Band || !Band->GetIsEnabled())
	{
		return;
	}

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
			Band->SetMakeupDb(NewMakeupGainDb);
			const float NewMakeupGain01 = (NewMakeupGainDb + 48.f) / 96.f;
			ToggleKnobMakeupGain->SetValue01(NewMakeupGain01);
		}
	}

	ViewModel->GetEvent_BandChanged().Broadcast(Band);
}

int32 UEW_SonaQBandPopup::NativePaint(const FPaintArgs&        Args,
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

	return Result;
}

FReply UEW_SonaQBandPopup::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	bIsAltKeyDown = InKeyEvent.IsAltDown();
	return Reply;
}

FReply UEW_SonaQBandPopup::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyUp(InGeometry, InKeyEvent);
	bIsAltKeyDown = InKeyEvent.IsAltDown();
	return Reply;
}

void UEW_SonaQBandPopup::RefreshVisual()
{
	if (Band)
	{
		const float Frequency01 = MathLogTool::TwentiethsToTribel(Band->GetFrequency());
		const float Amount01 = (Band->GetAmountDb() + 48.f) / 96.f;
		const float Quality01 = MathLogTool::ThousandsToHexabel(Band->GetQuality());
		const float MakeupGain01 = (Band->GetMakeupDb() + 48.f) / 96.f;

		NaveledKnobFrequency->SetValue01(Frequency01);
		NaveledKnobAmount->SetValue01(Amount01);
		NaveledKnobQuality->SetValue01(Quality01);
		ToggleKnobMakeupGain->SetValue01(MakeupGain01);
		ToggleKnobMakeupGain->SetIsOn(Band->GetIsEnabled());

		NaveledKnobAmount->HoverIcon = GetBandIconByType(GetPopupTypeByBandType(Band->GetType()));

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
			TextBoxValue->SetText(FloatToText(Band->GetMakeupDb(), 2, ESignMode::Always));
		}
	}
}

EEQBandType UEW_SonaQBandPopup::GetBandTypeByPopupType(EBandPopupType InBandPopupType)
{
	UEnum* BandPopupEnum = StaticEnum<EBandPopupType>();
	UEnum* BandEnum = StaticEnum<EEQBandType>();

	if (BandEnum && BandPopupEnum)
	{
		if (InBandPopupType == EBandPopupType::CutLow)
		{
			return EEQBandType::CutLowButterworth;
		}
		if (InBandPopupType == EBandPopupType::CutHigh)
		{
			return EEQBandType::CutHighButterworth;
		}

		FName Name = BandPopupEnum->GetNameByValue(static_cast<int64>(InBandPopupType));
		return static_cast<EEQBandType>(BandEnum->GetValueByName(Name));
	}

	return EEQBandType::None;
}

EBandPopupType UEW_SonaQBandPopup::GetPopupTypeByBandType(EEQBandType InBandType)
{
	UEnum* BandPopupEnum = StaticEnum<EBandPopupType>();
	UEnum* BandEnum = StaticEnum<EEQBandType>();

	if (BandEnum && BandPopupEnum)
	{
		if (InBandType == EEQBandType::CutLowFast || 
			InBandType == EEQBandType::CutLowButterworth)
		{
			return EBandPopupType::CutLow;
		}
		if (InBandType == EEQBandType::CutHighFast || 
			InBandType == EEQBandType::CutHighButterworth)
		{
			return EBandPopupType::CutHigh;
		}

		FName Name = BandEnum->GetNameByValue(static_cast<int64>(InBandType));
		return static_cast<EBandPopupType>(BandPopupEnum->GetValueByName(Name));
	}

	UE_LOG(LogSonafridgeEditor, Error, TEXT("Could not get EBandPopupType or EEQBandType metadata. "))
	return EBandPopupType::AttBand;
}

UTexture* UEW_SonaQBandPopup::GetBandIconByType(EBandPopupType InBandPopup)
{
	switch (InBandPopup)
	{
		case EBandPopupType::CutLow: return IconLowCut;
		case EBandPopupType::CutHigh: return IconHighCut;
		case EBandPopupType::AttLow: return IconLowShelf;
		case EBandPopupType::AttHigh: return IconHighShelf;
		case EBandPopupType::AttBand: return IconBandCut;
		case EBandPopupType::Notch: return IconNotch;
		default: return nullptr;
	}
}

FVector2D UEW_SonaQBandPopup::GetBandWPos()
{
	if (Band)
	{
		float F = Band->GetFrequency();
		float NX = MathLogTool::TwentiethsToTribel(F);
		float NY = Band->GetType() == EEQBandType::Notch
		           ? 0.f
		           : (FVM_SonaQ::DynamicMax - ViewModel->DtftDb(F)) / 
		             (FVM_SonaQ::DynamicMax - FVM_SonaQ::DynamicMin);

		float WX = NX * RootWidget->GetLastSize().X;
		float WY = NY * RootWidget->GetLastSize().Y;

		return { WX, WY };
	}

	return {};
}

void UEW_SonaQBandPopup::TryAutoMakeup(const FPointerEvent& InMouseEvent)
{
	if (!InMouseEvent.IsAltDown())
	{
		if (Band->GetType() == EEQBandType::AttBand)
		{
			Band->SetMakeupDb(PresstimeMakeupDb + 1.5f * (PresstimeAvgDb - Band->GetResponseAvgDb()));
		}
		else if (Band->GetType() == EEQBandType::AttLow ||
				 Band->GetType() == EEQBandType::AttHigh)
		{
			Band->SetMakeupDb(PresstimeMakeupDb + 1.0f * (PresstimeAvgDb - Band->GetResponseAvgDb()));
		}
	}
}
