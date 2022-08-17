// Sonafridge 2022

#include "W_ClathriQBandPopup.h"

#include "Sonafridge/Attenuator/Clathrispace.h"
#include "Sonafridge/Widgets/NaveledKnob.h"
#include "Sonafridge/Widgets/ToggleKnob.h"
#include "Sonafridge/Tools/MathTools.h"
#include "W_ClathriQ.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"
#include "Kismet/KismetInputLibrary.h"
#include "Misc/DefaultValueHelper.h"
#include "AudioDevice.h"

using namespace SonafridgeEditor;


UW_ClathriQBandPopup::UW_ClathriQBandPopup()
{
	BandsActivityBeforeListenTime.SetNum(ClathriEar::BandCount);
}

void UW_ClathriQBandPopup::Init(UW_ClathriQ* InRootWidget, TSharedPtr<FVM_ClathriQ> InViewModel)
{
	RootWidget = InRootWidget;
	ViewModel = InViewModel;
	ViewModel->GetEvent_PinIndexChanged().AddUObject(this, &UW_ClathriQBandPopup::OnPinIndexChanged);
	ViewModel->GetEvent_BandChanging().AddUObject(this, &UW_ClathriQBandPopup::OnBandChanging);
	ViewModel->GetEvent_BandChanged().AddUObject(this, &UW_ClathriQBandPopup::OnBandChanged);
	ViewModel->GetEvent_BandSelectionChanged().AddUObject(this, &UW_ClathriQBandPopup::OnBandSelectionChanged);
}

void UW_ClathriQBandPopup::FollowBand()
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

void UW_ClathriQBandPopup::NativeConstruct()
{
	Super::NativeConstruct();

	NaveledKnobFrequency->GetEvent_KnobEntrance().AddUObject(this, &UW_ClathriQBandPopup::OnFrequencyEntrance);
	NaveledKnobAmount->GetEvent_KnobEntrance().AddUObject(this, &UW_ClathriQBandPopup::OnAmountEntrance);
	NaveledKnobQuality->GetEvent_KnobEntrance().AddUObject(this, &UW_ClathriQBandPopup::OnQualityEntrance);
	ToggleKnobMakeupGain->GetEvent_KnobEntrance().AddUObject(this, &UW_ClathriQBandPopup::OnMakeupGainEntrance);

	ToggleKnobMakeupGain->GetEvent_NavelEntrance().AddUObject(this, &UW_ClathriQBandPopup::OnToggleOnOffEntrance);
	ToggleKnobMakeupGain->GetEvent_NavelExit().AddUObject(this, &UW_ClathriQBandPopup::OnToggleOnOffExit);

	NaveledKnobFrequency->GetEvent_KnobValueChanged().AddUObject(this, &UW_ClathriQBandPopup::OnFrequencyDelta);
	NaveledKnobAmount->GetEvent_KnobValueChanged().AddUObject(this, &UW_ClathriQBandPopup::OnAmountDelta);
	NaveledKnobQuality->GetEvent_KnobValueChanged().AddUObject(this, &UW_ClathriQBandPopup::OnQualityDelta);
	ToggleKnobMakeupGain->GetEvent_KnobValueChanged().AddUObject(this, &UW_ClathriQBandPopup::OnMakeupGainDelta);

	NaveledKnobFrequency->GetEvent_NavelValueChanged().AddUObject(this, &UW_ClathriQBandPopup::OnListenDelta);
	NaveledKnobQuality->GetEvent_NavelValueChanged().AddUObject(this, &UW_ClathriQBandPopup::OnToggleNavelRemoveValueChanged);
	ToggleKnobMakeupGain->GetEvent_NavelValueChanged().AddUObject(this, &UW_ClathriQBandPopup::OnToggleNavelOnOffValueChanged);

	ToggleKnobMakeupGain->GetEvent_ToggleStateChanged().AddUObject(this, &UW_ClathriQBandPopup::OnToggleNavelOnOffStateChanged);

	NaveledKnobFrequency->GetEvent_KnobCaptureStarted().AddUObject(this, &UW_ClathriQBandPopup::OnFrequencyCapture);
	NaveledKnobFrequency->GetEvent_KnobCaptureFinished().AddUObject(this, &UW_ClathriQBandPopup::OnFrequencyCommit);
	NaveledKnobAmount->GetEvent_KnobCaptureStarted().AddUObject(this, &UW_ClathriQBandPopup::OnAmountCapture);
	NaveledKnobAmount->GetEvent_KnobCaptureFinished().AddUObject(this, &UW_ClathriQBandPopup::OnAmountCommit);
	NaveledKnobQuality->GetEvent_KnobCaptureStarted().AddUObject(this, &UW_ClathriQBandPopup::OnQualityCapture);
	NaveledKnobQuality->GetEvent_KnobCaptureFinished().AddUObject(this, &UW_ClathriQBandPopup::OnQualityCommit);
	ToggleKnobMakeupGain->GetEvent_KnobCaptureStarted().AddUObject(this, &UW_ClathriQBandPopup::OnMakeupGainCapture);
	ToggleKnobMakeupGain->GetEvent_KnobCaptureFinished().AddUObject(this, &UW_ClathriQBandPopup::OnMakeupGainCommit);

	NaveledKnobFrequency->GetEvent_KnobClick().AddUObject(this, &UW_ClathriQBandPopup::OnFrequencyClick);
	NaveledKnobAmount->GetEvent_KnobClick().AddUObject(this, &UW_ClathriQBandPopup::OnAmountClick);
	NaveledKnobQuality->GetEvent_KnobClick().AddUObject(this, &UW_ClathriQBandPopup::OnQualityClick);
	ToggleKnobMakeupGain->GetEvent_KnobClick().AddUObject(this, &UW_ClathriQBandPopup::OnMakeupClick);

	TextBoxValue->OnTextCommitted.AddDynamic(this, &UW_ClathriQBandPopup::OnTextCommitted);
}

void UW_ClathriQBandPopup::OnFrequencyEntrance()
{
	FocusMode = EBandPopupFocusMode::Frequency;
	TextBlockKey->SetText(FText::FromString(TEXT("Frequency")));
	TextBoxValue->SetText(FloatToText(ViewModel->GetFrequency(BandIndex), 1));
	NaveledKnobFrequency->RefreshVisual();
}

void UW_ClathriQBandPopup::OnAmountEntrance()
{
	FocusMode = EBandPopupFocusMode::Amount;
	TextBlockKey->SetText(FText::FromString(TEXT("Amount")));
	TextBoxValue->SetText(FloatToText(ViewModel->GetAmountDb(BandIndex), 2, ESignMode::Always));
	NaveledKnobAmount->RefreshVisual();
}

void UW_ClathriQBandPopup::OnQualityEntrance()
{
	FocusMode = EBandPopupFocusMode::Quality;
	TextBlockKey->SetText(FText::FromString(TEXT("Quality")));
	TextBoxValue->SetText(FloatToText(ViewModel->GetQuality(BandIndex), 3));
	NaveledKnobQuality->RefreshVisual();
}

void UW_ClathriQBandPopup::OnMakeupGainEntrance()
{
	FocusMode = EBandPopupFocusMode::Makeup;
	TextBlockKey->SetText(FText::FromString(TEXT("Makeup Gain")));
	TextBoxValue->SetText(FloatToText(ViewModel->GetMakeupDb(BandIndex), 2, ESignMode::Always));
	ToggleKnobMakeupGain->RefreshVisual();
}

void UW_ClathriQBandPopup::OnToggleOnOffEntrance()
{
	FocusMode = EBandPopupFocusMode::Makeup;
	TextBlockKey->SetText(FText::FromString(TEXT("On/off band")));
	TextBoxValue->SetText(FText::FromString(TEXT("On/off band")));
	ToggleKnobMakeupGain->RefreshVisual();
}

void UW_ClathriQBandPopup::OnToggleOnOffExit()
{
}

void UW_ClathriQBandPopup::OnPinIndexChanged(int32 InPinIndex)
{
	PinIndex = InPinIndex;
	bool bIsPinFailed = PinIndex == INDEX_NONE;
	bool bIsBandFailed = BandIndex == INDEX_NONE;
	SetVisibility(bIsPinFailed || bIsBandFailed ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	FollowBand();
}

void UW_ClathriQBandPopup::OnBandSelectionChanged(int32 InBandIndex)
{
	BandIndex = InBandIndex;
	bool bIsPinFailed = PinIndex == INDEX_NONE;
	bool bIsBandFailed = BandIndex == INDEX_NONE;
	SetVisibility(bIsPinFailed || bIsBandFailed ? ESlateVisibility::Hidden : ESlateVisibility::Visible);

	if (BandIndex != INDEX_NONE)
	{
		TextBoxValue->SetIsEnabled(ViewModel->GetIsEnabled(BandIndex));
	}

	FollowBand();
	RefreshVisual();
}

void UW_ClathriQBandPopup::OnBandChanging(int32 InBandIndex)
{
	RefreshVisual();
	FollowBand();
}

void UW_ClathriQBandPopup::OnBandChanged(int32 InBandIndex)
{
	RefreshVisual();
	FollowBand();
}

void UW_ClathriQBandPopup::OnFrequencyDelta(float                OldFrequency01,
                                            float                NewFrequency01,
                                            const FPointerEvent& InMouseEvent,
                                            bool&                bInOutHaveAllHandlersAccepted)
{
	if (ViewModel->GetIsEnabled(BandIndex))
	{
		float NewFrequency = MathLogTool::TribelToTwentieths(NewFrequency01);
		ViewModel->SetFrequency(BandIndex, NewFrequency);
		TextBoxValue->SetText(FloatToText(NewFrequency, 1));
		ViewModel->GetEvent_BandChanging().Broadcast(BandIndex);

		TryAutoMakeup(InMouseEvent);
	}
	else
	{
		bInOutHaveAllHandlersAccepted = false;
	}
}

void UW_ClathriQBandPopup::OnAmountDelta(float                OldAmount01,
                                         float                NewAmount01,
                                         const FPointerEvent& InMouseEvent,
                                         bool&                bInOutHaveAllHandlersAccepted)
{
	if (ViewModel->GetIsEnabled(BandIndex))
	{
		float NewAmountDb = NewAmount01 * 96.f - 48.f;
		ViewModel->SetAmountDb(BandIndex, NewAmountDb);
		TextBoxValue->SetText(FloatToText(NewAmountDb, 2, ESignMode::Always));
		ViewModel->GetEvent_BandChanging().Broadcast(BandIndex);

		TryAutoMakeup(InMouseEvent);
	}
	else
	{
		bInOutHaveAllHandlersAccepted = false;
	}
}

void UW_ClathriQBandPopup::OnQualityDelta(float                OldQuality01,
                                          float                NewQuality01,
                                          const FPointerEvent& InMouseEvent,
                                          bool&                bInOutHaveAllHandlersAccepted)
{
	if (ViewModel->GetIsEnabled(BandIndex))
	{
		float NewQuality = MathLogTool::HexabelToThousands(NewQuality01);
		ViewModel->SetQuality(BandIndex, NewQuality);
		TextBoxValue->SetText(FloatToText(NewQuality, 3));
		ViewModel->GetEvent_BandChanging().Broadcast(BandIndex);

		TryAutoMakeup(InMouseEvent);
	}
	else
	{
		bInOutHaveAllHandlersAccepted = false;
	}
}

void UW_ClathriQBandPopup::OnMakeupGainDelta(float                OldMakeupGain01,
                                             float                NewMakeupGain01,
                                             const FPointerEvent& InMouseEvent,
                                             bool&                bInOutHaveAllHandlersAccepted)
{
	if (ViewModel->GetIsEnabled(BandIndex))
	{
		float NewMakeupGainDb = NewMakeupGain01 * 96.f - 48.f;
		ViewModel->SetMakeupDb(BandIndex, NewMakeupGainDb);
		TextBoxValue->SetText(FloatToText(NewMakeupGainDb, 2, ESignMode::Always));
		ViewModel->GetEvent_BandChanging().Broadcast(BandIndex);
	}
	else
	{
		bInOutHaveAllHandlersAccepted = false;
	}
}

void UW_ClathriQBandPopup::OnListenDelta(float FrequencyDelta, const FPointerEvent& InMouseEvent)
{
	if (ViewModel->GetIsEnabled(BandIndex))
	{
		float OldFrequency01 = MathLogTool::TwentiethsToTribel(ViewModel->GetFrequency(BandIndex));
		float NewFrequency01 = OldFrequency01 + FrequencyDelta;
		NewFrequency01 = FMath::Clamp(NewFrequency01, 0.f, 1.f);
		NaveledKnobFrequency->SetValue01(NewFrequency01);

		float NewFrequency = MathLogTool::TribelToTwentieths(NewFrequency01);
		ViewModel->SetFrequency(BandIndex, NewFrequency);
		TextBoxValue->SetText(FloatToText(NewFrequency, 1));
		ViewModel->GetEvent_BandChanging().Broadcast(BandIndex);

		TryAutoMakeup(InMouseEvent);
	}
}

void UW_ClathriQBandPopup::OnToggleNavelRemoveValueChanged(float QualityDelta01, const FPointerEvent& InMouseEvent)
{
	if (ViewModel->GetIsEnabled(BandIndex))
	{
		float OldQuality01 = MathLogTool::ThousandsToHexabel(ViewModel->GetQuality(BandIndex));
		float NewQuality01 = OldQuality01 + QualityDelta01;
		NewQuality01 = FMath::Clamp(NewQuality01, 0.f, 1.f);
		NaveledKnobQuality->SetValue01(NewQuality01);

		float NewQuality = MathLogTool::HexabelToThousands(NewQuality01);
		ViewModel->SetQuality(BandIndex, NewQuality);
		TextBoxValue->SetText(FloatToText(NewQuality, 3));
		ViewModel->GetEvent_BandChanging().Broadcast(BandIndex);

		TryAutoMakeup(InMouseEvent);
	}
}

void UW_ClathriQBandPopup::OnToggleNavelOnOffValueChanged(float MakeupGainDelta01, const FPointerEvent& InMouseEvent)
{
	if (ViewModel->GetIsEnabled(BandIndex))
	{
		float OldMakeupGain01 = (ViewModel->GetMakeupDb(BandIndex) + 48.f) / 96.f;
		float NewMakeupGain01 = OldMakeupGain01 + MakeupGainDelta01;
		NewMakeupGain01 = FMath::Clamp(NewMakeupGain01, 0.f, 1.f);
		ToggleKnobMakeupGain->SetValue01(NewMakeupGain01);

		float NewMakeupGainDb = NewMakeupGain01 * 96.f - 48.f;
		ViewModel->SetMakeupDb(BandIndex, NewMakeupGainDb);
		TextBoxValue->SetText(FloatToText(NewMakeupGainDb, 2, ESignMode::Always));
		ViewModel->GetEvent_BandChanging().Broadcast(BandIndex);
	}
}

void UW_ClathriQBandPopup::OnToggleNavelOnOffStateChanged(bool bOldValue, bool bNewValue)
{
	ViewModel->SetIsEnabled(BandIndex, bNewValue);
	ViewModel->GetEvent_BandChanged().Broadcast(BandIndex);
	TextBoxValue->SetIsEnabled(ViewModel->GetIsEnabled(BandIndex));
}

void UW_ClathriQBandPopup::OnFrequencyCapture()
{
	PresstimeMakeupDb = ViewModel->GetMakeupDb(BandIndex);
	PresstimeAvgDb = ViewModel->GetResponseAvgDb(BandIndex);
}

void UW_ClathriQBandPopup::OnFrequencyCommit()
{
	if (ViewModel->GetIsEnabled())
	{
		ViewModel->GetEvent_BandChanged().Broadcast(BandIndex);
	}
}

void UW_ClathriQBandPopup::OnAmountCapture()
{
	PresstimeMakeupDb = ViewModel->GetMakeupDb(BandIndex);
	PresstimeAvgDb = ViewModel->GetResponseAvgDb(BandIndex);
}

void UW_ClathriQBandPopup::OnAmountCommit()
{
	if (ViewModel->GetIsEnabled(BandIndex))
	{
		ViewModel->GetEvent_BandChanged().Broadcast(BandIndex);
	}
}

void UW_ClathriQBandPopup::OnQualityCapture()
{
	PresstimeMakeupDb = ViewModel->GetMakeupDb(BandIndex);
	PresstimeAvgDb = ViewModel->GetResponseAvgDb(BandIndex);
}

void UW_ClathriQBandPopup::OnQualityCommit()
{
	if (ViewModel->GetIsEnabled())
	{
		ViewModel->GetEvent_BandChanged().Broadcast(BandIndex);
	}
}

void UW_ClathriQBandPopup::OnMakeupGainCapture()
{
	PresstimeMakeupDb = ViewModel->GetMakeupDb(BandIndex);
	PresstimeAvgDb = ViewModel->GetResponseAvgDb(BandIndex);
}

void UW_ClathriQBandPopup::OnMakeupGainCommit()
{
	if (ViewModel->GetIsEnabled())
	{
		ViewModel->GetEvent_BandChanged().Broadcast(BandIndex);
	}
}

void UW_ClathriQBandPopup::OnFrequencyClick()
{
	TextBoxValue->SetKeyboardFocus();
}

void UW_ClathriQBandPopup::OnAmountClick()
{
	TextBoxValue->SetKeyboardFocus();
}

void UW_ClathriQBandPopup::OnQualityClick()
{
	TextBoxValue->SetKeyboardFocus();
}

void UW_ClathriQBandPopup::OnMakeupClick()
{
	TextBoxValue->SetKeyboardFocus();
}

void UW_ClathriQBandPopup::OnTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (BandIndex == INDEX_NONE || !ViewModel->GetIsEnabled(BandIndex))
	{
		return;
	}

	if (FocusMode == EBandPopupFocusMode::Frequency)
	{
		float NewFrequency;
		if (FDefaultValueHelper::ParseFloat(Text.ToString(), NewFrequency))
		{
			ViewModel->SetFrequency(BandIndex, NewFrequency);
			const float NewFrequency01 = MathLogTool::TwentiethsToTribel(NewFrequency);
			NaveledKnobFrequency->SetValue01(NewFrequency01);
		}
	}
	else if (FocusMode == EBandPopupFocusMode::Amount)
	{
		float NewAmountDb;
		if (FDefaultValueHelper::ParseFloat(Text.ToString(), NewAmountDb))
		{
			ViewModel->SetAmountDb(BandIndex, NewAmountDb);
			const float NewAmount01 = (NewAmountDb + 48.f) / 96.f;
			NaveledKnobAmount->SetValue01(NewAmount01);
		}
	}
	else if (FocusMode == EBandPopupFocusMode::Quality)
	{
		float NewQuality;
		if (FDefaultValueHelper::ParseFloat(Text.ToString(), NewQuality))
		{
			ViewModel->SetQuality(BandIndex, NewQuality);
			const float NewQuality01 = MathLogTool::ThousandsToHexabel(NewQuality);
			NaveledKnobQuality->SetValue01(NewQuality01);
		}
	}
	else if (FocusMode == EBandPopupFocusMode::Makeup)
	{
		float NewMakeupGainDb;
		if (FDefaultValueHelper::ParseFloat(Text.ToString(), NewMakeupGainDb))
		{
			ViewModel->SetMakeupDb(BandIndex, NewMakeupGainDb);
			const float NewMakeupGain01 = (NewMakeupGainDb + 48.f) / 96.f;
			ToggleKnobMakeupGain->SetValue01(NewMakeupGain01);
		}
	}

	ViewModel->GetEvent_BandChanged().Broadcast(BandIndex);
}

int32 UW_ClathriQBandPopup::NativePaint(const FPaintArgs&        Args,
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

FReply UW_ClathriQBandPopup::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	bIsAltKeyDown = InKeyEvent.IsAltDown();
	return Reply;
}

FReply UW_ClathriQBandPopup::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyUp(InGeometry, InKeyEvent);
	bIsAltKeyDown = InKeyEvent.IsAltDown();
	return Reply;
}

void UW_ClathriQBandPopup::RefreshVisual()
{
	if (BandIndex != INDEX_NONE)
	{
		const float Frequency01 = MathLogTool::TwentiethsToTribel(ViewModel->GetFrequency(BandIndex));
		const float Amount01 = (ViewModel->GetAmountDb(BandIndex) + 48.f) / 96.f;
		const float Quality01 = MathLogTool::ThousandsToHexabel(ViewModel->GetQuality(BandIndex));
		const float MakeupGain01 = (ViewModel->GetMakeupDb(BandIndex) + 48.f) / 96.f;

		NaveledKnobFrequency->SetValue01(Frequency01);
		NaveledKnobAmount->SetValue01(Amount01);
		NaveledKnobQuality->SetValue01(Quality01);
		ToggleKnobMakeupGain->SetValue01(MakeupGain01);
		ToggleKnobMakeupGain->SetIsOn(ViewModel->GetIsEnabled(BandIndex));

		if (FocusMode == EBandPopupFocusMode::Frequency)
		{
			TextBlockKey->SetText(FText::FromString(TEXT("Frequency")));
			TextBoxValue->SetText(FloatToText(ViewModel->GetFrequency(BandIndex), 1));
		}
		else if (FocusMode == EBandPopupFocusMode::Amount)
		{
			TextBlockKey->SetText(FText::FromString(TEXT("Amount")));
			TextBoxValue->SetText(FloatToText(ViewModel->GetAmountDb(BandIndex), 2, ESignMode::Always));
		}
		else if (FocusMode == EBandPopupFocusMode::Quality)
		{
			TextBlockKey->SetText(FText::FromString(TEXT("Quality")));
			TextBoxValue->SetText(FloatToText(ViewModel->GetQuality(BandIndex), 3));
		}
		else if (FocusMode == EBandPopupFocusMode::Makeup)
		{
			TextBlockKey->SetText(FText::FromString(TEXT("Makeup Gain")));
			TextBoxValue->SetText(FloatToText(ViewModel->GetMakeupDb(BandIndex), 2, ESignMode::Always));
		}
	}
}

EEQBandType UW_ClathriQBandPopup::GetBandTypeByPopupType(EBandPopupType InBandPopupType)
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

EBandPopupType UW_ClathriQBandPopup::GetPopupTypeByBandType(EEQBandType InBandType)
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

FVector2D UW_ClathriQBandPopup::GetBandWPos()
{
	if (BandIndex != INDEX_NONE)
	{
		float F = ViewModel->GetFrequency(BandIndex);
		float NX = MathLogTool::TwentiethsToTribel(F);
		float NY = (FVM_ClathriQ::DynamicMax - ViewModel->DtftDb(F))
		         / (FVM_ClathriQ::DynamicMax - FVM_ClathriQ::DynamicMin);

		float WX = NX * RootWidget->GetLastSize().X;
		float WY = NY * RootWidget->GetLastSize().Y;

		return { WX, WY };
	}

	return FVector2D::ZeroVector;
}

void UW_ClathriQBandPopup::TryAutoMakeup(const FPointerEvent& InMouseEvent)
{
	if (!InMouseEvent.IsAltDown())
	{
		bool bIsBell = BandIndex == 1;
		bool bIsShelf = BandIndex == 0 || BandIndex == 2;

		if (bIsBell)
		{
			float ResponseAvgDb = ViewModel->GetResponseAvgDb(BandIndex);
			ViewModel->SetMakeupDb
			(
				BandIndex, 
				PresstimeMakeupDb + 1.5f * (PresstimeAvgDb - ResponseAvgDb)
			);
		}
		else if (bIsShelf)
		{
			float ResponseAvgDb = ViewModel->GetResponseAvgDb(BandIndex);
			ViewModel->SetMakeupDb
			(
				BandIndex, 
				PresstimeMakeupDb + 1.0f * (PresstimeAvgDb - ResponseAvgDb)
			);
		}
	}
}

