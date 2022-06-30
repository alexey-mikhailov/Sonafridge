// Fill out your copyright notice in the Description page of Project Settings.


#include "EW_EQBandPopup.h"

#include "Sonafridge/Widgets/NaveledKnob.h"
#include "Sonafridge/Widgets/ToggleKnob.h"
#include "Sonafridge/MathTools.h"
#include "AudioDevice.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"
#include "Misc/DefaultValueHelper.h"


void UEW_EQBandPopup::NativeConstruct()
{
	Super::NativeConstruct();

	// TODO: Change to IEQSettings injection. Get SampleRate from there. 
	float SampleRate = 44100.f;
	if (auto AudioDevice = GEngine->GetActiveAudioDevice())
	{
		SampleRate = AudioDevice->GetSampleRate();
	}

	Band = MakeShared<FEQBand>();
	Band->Init(SampleRate);
	Band->SetType(EBandType::BandCut);
	BandTypeFloat = static_cast<float>(EBandType::BandCut);

	const float Frequency01 = MathLogTool::TwentiethsToTribel(Band->GetFrequency());
	const float Amount01 = (Band->GetAmountDb() + 48.f) / 96.f;
	const float Quality01 = MathLogTool::ThousandsToHexabel(Band->GetQuality());
	const float MakeupGain01 = (Band->GetLoudCompDb() + 48.f) / 96.f;

	NaveledKnobFrequency->SetValue01(Frequency01);
	NaveledKnobAmount->SetValue01(Amount01);
	ToggleKnobQuality->SetValue01(Quality01);
	ToggleKnobMakeupGain->SetValue01(MakeupGain01);

	NaveledKnobFrequency->GetEvent_KnobEntrance().AddUObject(this, &UEW_EQBandPopup::OnFrequencyEntrance);
	NaveledKnobAmount->GetEvent_KnobEntrance().AddUObject(this, &UEW_EQBandPopup::OnAmountEntrance);
	ToggleKnobQuality->GetEvent_KnobEntrance().AddUObject(this, &UEW_EQBandPopup::OnQualityEntrance);
	ToggleKnobMakeupGain->GetEvent_KnobEntrance().AddUObject(this, &UEW_EQBandPopup::OnMakeupGainEntrance);

	NaveledKnobFrequency->GetEvent_NavelEntrance().AddUObject(this, &UEW_EQBandPopup::OnListenEntrance);
	NaveledKnobAmount->GetEvent_NavelEntrance().AddUObject(this, &UEW_EQBandPopup::OnBandTypeEntrance);
	ToggleKnobQuality->GetEvent_NavelEntrance().AddUObject(this, &UEW_EQBandPopup::OnButtonRemoveEntrance);
	ToggleKnobMakeupGain->GetEvent_NavelEntrance().AddUObject(this, &UEW_EQBandPopup::OnToggleOnOffEntrance);

	NaveledKnobFrequency->GetEvent_NavelExit().AddUObject(this, &UEW_EQBandPopup::OnListenExit);
	NaveledKnobAmount->GetEvent_NavelExit().AddUObject(this, &UEW_EQBandPopup::OnBandTypeExit);
	ToggleKnobQuality->GetEvent_NavelExit().AddUObject(this, &UEW_EQBandPopup::OnButtonRemoveExit);
	ToggleKnobMakeupGain->GetEvent_NavelExit().AddUObject(this, &UEW_EQBandPopup::OnToggleOnOffExit);

	NaveledKnobFrequency->GetEvent_KnobValueChanged().AddUObject(this, &UEW_EQBandPopup::OnFrequencyChanged);
	NaveledKnobAmount->GetEvent_KnobValueChanged().AddUObject(this, &UEW_EQBandPopup::OnAmountChanged);
	ToggleKnobQuality->GetEvent_KnobValueChanged().AddUObject(this, &UEW_EQBandPopup::OnQualityChanged);
	ToggleKnobMakeupGain->GetEvent_KnobValueChanged().AddUObject(this, &UEW_EQBandPopup::OnMakeupGainChanged);

	NaveledKnobFrequency->GetEvent_NavelCaptureStarted().AddUObject(this, &UEW_EQBandPopup::OnListenStarted);
	NaveledKnobFrequency->GetEvent_NavelCaptureFinished().AddUObject(this, &UEW_EQBandPopup::OnListenFinished);
	NaveledKnobFrequency->GetEvent_NavelValueChanged().AddUObject(this, &UEW_EQBandPopup::OnListenDelta);
	NaveledKnobAmount->GetEvent_NavelValueChanged().AddUObject(this, &UEW_EQBandPopup::OnBandTypeChanged);

	ToggleKnobQuality->GetEvent_ToggleStateChanged().AddUObject(this, &UEW_EQBandPopup::OnToggleNavelRemoveStateChanged);
	ToggleKnobQuality->GetEvent_NavelValueChanged().AddUObject(this, &UEW_EQBandPopup::OnToggleNavelRemoveValueChanged);
	ToggleKnobMakeupGain->GetEvent_ToggleStateChanged().AddUObject(this, &UEW_EQBandPopup::OnToggleNavelOnOffStateChanged);
	ToggleKnobMakeupGain->GetEvent_NavelValueChanged().AddUObject(this, &UEW_EQBandPopup::OnToggleNavelOnOffValueChanged);

	TextBoxValue->OnTextCommitted.AddDynamic(this, &UEW_EQBandPopup::OnTextCommitted);
}

void UEW_EQBandPopup::OnFrequencyEntrance()
{
	FocusMode = EBandPopupFocusMode::Frequency;
	TextBlockKey->SetText(FText::FromString(TEXT("Frequency")));
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(Band->GetFrequency(), HalfFromZero, false, false, 1, 324, 1, 1));
	NaveledKnobFrequency->RefreshVisual();
}

void UEW_EQBandPopup::OnAmountEntrance()
{
	FocusMode = EBandPopupFocusMode::Amount;
	TextBlockKey->SetText(FText::FromString(TEXT("Amount")));
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(Band->GetAmountDb(), HalfFromZero, true, false, 1, 324, 2, 2));
	NaveledKnobAmount->RefreshVisual();
}

void UEW_EQBandPopup::OnQualityEntrance()
{
	FocusMode = EBandPopupFocusMode::Quality;
	TextBlockKey->SetText(FText::FromString(TEXT("Quality")));
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(Band->GetQuality(), HalfFromZero, false, false, 1, 324, 3, 3));
	ToggleKnobQuality->RefreshVisual();
}

void UEW_EQBandPopup::OnMakeupGainEntrance()
{
	FocusMode = EBandPopupFocusMode::Makeup;
	TextBlockKey->SetText(FText::FromString(TEXT("Makeup Gain")));
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(Band->GetLoudCompDb(), HalfFromZero, true, false, 1, 324, 2, 2));
	ToggleKnobMakeupGain->RefreshVisual();
}

void UEW_EQBandPopup::OnListenEntrance()
{
	FocusMode = EBandPopupFocusMode::Frequency;
	TextBlockKey->SetText(FText::FromString(TEXT("Frequency")));
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(Band->GetFrequency(), HalfFromZero, false, false, 1, 324, 1, 1));
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
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(Band->GetQuality(), HalfFromZero, false, false, 1, 324, 3, 3));
	ToggleKnobQuality->RefreshVisual();
}

void UEW_EQBandPopup::OnToggleOnOffEntrance()
{
	FocusMode = EBandPopupFocusMode::Makeup;
	TextBlockKey->SetText(FText::FromString(TEXT("Makeup Gain")));
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(Band->GetLoudCompDb(), HalfFromZero, true, false, 1, 324, 2, 2));
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

void UEW_EQBandPopup::OnFrequencyChanged(float OldFrequency01, float NewFrequency01)
{
	float NewFrequency = MathLogTool::TribelToTwentieths(NewFrequency01);
	Band->SetFrequency(NewFrequency);
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(NewFrequency, HalfFromZero, false, false, 1, 324, 1, 1));
}

void UEW_EQBandPopup::OnAmountChanged(float OldAmount01, float NewAmount01)
{
	float NewAmountDb = NewAmount01 * 96.f - 48.f;
	Band->SetAmountDb(NewAmountDb);
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(NewAmountDb, HalfFromZero, true, false, 1, 324, 2, 2));
}

void UEW_EQBandPopup::OnQualityChanged(float OldQuality01, float NewQuality01)
{
	float NewQuality = MathLogTool::HexabelToThousands(NewQuality01);
	Band->SetQuality(NewQuality);
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(NewQuality, HalfFromZero, false, false, 1, 324, 3, 3));
}

void UEW_EQBandPopup::OnMakeupGainChanged(float OldMakeupGain01, float NewMakeupGain01)
{
	float NewMakeupGainDb = NewMakeupGain01 * 96.f - 48.f;
	Band->SetLoudCompDb(NewMakeupGainDb);
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(NewMakeupGainDb, HalfFromZero, true, false, 1, 324, 2, 2));
}

void UEW_EQBandPopup::OnListenStarted()
{
	BandTypeBeforeListenTime = Band->GetType();
	Band->SetType(EBandType::BandPass);
}

void UEW_EQBandPopup::OnListenDelta(float FrequencyDelta)
{
	float OldFrequency01 = MathLogTool::TwentiethsToTribel(Band->GetFrequency());
	float NewFrequency01 = OldFrequency01 + FrequencyDelta;
	NaveledKnobFrequency->SetValue01(NewFrequency01);

	float NewFrequency = MathLogTool::TribelToTwentieths(NewFrequency01);
	Band->SetFrequency(NewFrequency);
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(NewFrequency, HalfFromZero, false, false, 1, 324, 1, 1));
}

void UEW_EQBandPopup::OnListenFinished()
{
	Band->SetType(BandTypeBeforeListenTime);
}

void UEW_EQBandPopup::OnBandTypeChanged(float BandTypeDeltaAsFloat)
{
	UEnum* BandTypeEnum = StaticEnum<EBandType>();
	if (BandTypeEnum)
	{
		BandTypeFloat += 10.f * BandTypeDeltaAsFloat;

		BandTypeFloat = FMath::Clamp(BandTypeFloat,
		                             static_cast<float>(EBandType::None) + 1.f,
		                             static_cast<float>(BandTypeEnum->GetMaxEnumValue()) - 1.f);

		EBandType BandType = static_cast<EBandType>(BandTypeFloat);

		Band->SetType(BandType);

		FText EnumText = BandTypeEnum->GetDisplayNameTextByValue(static_cast<int64>(BandType));
		if (!EnumText.IsEmpty())
		{
			TextBoxValue->SetText(EnumText);
		}			
	}
}

void UEW_EQBandPopup::OnToggleNavelRemoveStateChanged(bool bOldValue, bool bNewValue)
{
	// TODO: Inject IEQSettings, remove band in there.
	UE_LOG(LogTemp, Log, TEXT("Remove band stub. "));
}

void UEW_EQBandPopup::OnToggleNavelRemoveValueChanged(float QualityDelta01)
{
	float OldQuality01 = MathLogTool::ThousandsToHexabel(Band->GetQuality());
	float NewQuality01 = OldQuality01 + QualityDelta01;
	ToggleKnobQuality->SetValue01(NewQuality01);

	float NewQuality = MathLogTool::HexabelToThousands(NewQuality01);
	Band->SetQuality(NewQuality);
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(NewQuality, HalfFromZero, false, false, 1, 324, 3, 3));
}

void UEW_EQBandPopup::OnToggleNavelOnOffStateChanged(bool bOldValue, bool bNewValue)
{
	Band->SetIsEnabled(bNewValue);
}

void UEW_EQBandPopup::OnToggleNavelOnOffValueChanged(float MakeupGainDelta01)
{
	float OldMakeupGain01 = (Band->GetLoudCompDb() + 48.f) / 96.f;
	float NewMakeupGain01 = OldMakeupGain01 + MakeupGainDelta01;
	ToggleKnobMakeupGain->SetValue01(NewMakeupGain01);

	float NewMakeupGainDb = NewMakeupGain01 * 96.f - 48.f;
	Band->SetLoudCompDb(NewMakeupGainDb);
	TextBoxValue->SetText(UKismetTextLibrary::Conv_FloatToText(NewMakeupGainDb, HalfFromZero, true, false, 1, 324, 2, 2));
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
			ToggleKnobQuality->SetValue01(NewQuality01);
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
}

