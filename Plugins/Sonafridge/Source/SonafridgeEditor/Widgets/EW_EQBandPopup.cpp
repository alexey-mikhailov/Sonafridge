// Fill out your copyright notice in the Description page of Project Settings.


#include "EW_EQBandPopup.h"

#include "Sonafridge/Widgets/ToggleNavelWidget.h"
#include "Sonafridge/Widgets/KnobWidget.h"
#include "Sonafridge/Widgets/NavelWidget.h"
#include "Sonafridge/MathTools.h"
#include "AudioDevice.h"


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
	Band->SetType(EBandType::Bandcut);

	const float Frequency01 = MathLogTool::TwentiethsToTribel(Band->GetFrequency());
	const float Amount01 = (Band->GetAmountDb() + 48.f) / 96.f;
	const float Quality01 = MathLogTool::ThousandsToHexabel(Band->GetQuality());
	const float MakeupGain01 = (Band->GetLoudCompDb() + 48.f) / 96.f;

	KnobFrequency->SetValue01(Frequency01);
	KnobAmount->SetValue01(Amount01);
	KnobQuality->SetValue01(Quality01);
	KnobMakeupGain->SetValue01(MakeupGain01);

	KnobFrequency->GetEvent_ValueChanged().AddUObject(this, &UEW_EQBandPopup::OnFrequencyChanged);
	KnobAmount->GetEvent_ValueChanged().AddUObject(this, &UEW_EQBandPopup::OnAmountChanged);
	KnobQuality->GetEvent_ValueChanged().AddUObject(this, &UEW_EQBandPopup::OnQualityChanged);
	KnobMakeupGain->GetEvent_ValueChanged().AddUObject(this, &UEW_EQBandPopup::OnMakeupGainChanged);

	NavelListen->GetEvent_MouseCaptureStarted().AddUObject(this, &UEW_EQBandPopup::OnListenStarted);
	NavelListen->GetEvent_MouseCaptureFinished().AddUObject(this, &UEW_EQBandPopup::OnListenFinished);
	NavelListen->GetEvent_ValueChanged().AddUObject(this, &UEW_EQBandPopup::OnListenDelta);
	NavelBandType->GetEvent_ValueChanged().AddUObject(this, &UEW_EQBandPopup::OnBandTypeChanged);

	ToggleNavelRemove->GetEvent_ToggleStateChanged().AddUObject(this, &UEW_EQBandPopup::OnToggleNavelRemoveStateChanged);
	ToggleNavelRemove->GetEvent_ValueChanged().AddUObject(this, &UEW_EQBandPopup::OnToggleNavelRemoveValueChanged);
	ToggleNavelOnOff->GetEvent_ToggleStateChanged().AddUObject(this, &UEW_EQBandPopup::OnToggleNavelOnOffStateChanged);
	ToggleNavelOnOff->GetEvent_ValueChanged().AddUObject(this, &UEW_EQBandPopup::OnToggleNavelOnOffValueChanged);
}

void UEW_EQBandPopup::OnFrequencyChanged(float OldFrequency01, float NewFrequency01)
{
	float NewFrequency = MathLogTool::TribelToTwentieths(NewFrequency01);
	Band->SetFrequency(NewFrequency);
}

void UEW_EQBandPopup::OnAmountChanged(float OldAmount01, float NewAmount01)
{
	float AmountDb = NewAmount01 * 96.f - 48.f;
	Band->SetAmountDb(AmountDb);
}

void UEW_EQBandPopup::OnQualityChanged(float OldQuality01, float NewQuality01)
{
	float Quality = MathLogTool::HexabelToThousands(NewQuality01);
	Band->SetQuality(Quality);
}

void UEW_EQBandPopup::OnMakeupGainChanged(float OldMakeupGain01, float NewMakeupGain01)
{
	float MakeupGainDb = NewMakeupGain01 * 96.f - 48.f;
	Band->SetLoudCompDb(MakeupGainDb);
}

void UEW_EQBandPopup::OnListenStarted()
{
	BandTypeBeforeListenTime = Band->GetType();
	Band->SetType(EBandType::Bandpass);
}

void UEW_EQBandPopup::OnListenDelta(float FrequencyDelta)
{
	float OldFrequency01 = MathLogTool::TwentiethsToTribel(Band->GetFrequency());
	float NewFrequency01 = OldFrequency01 + FrequencyDelta;
	KnobFrequency->SetValue01(NewFrequency01);

	float NewFrequency = MathLogTool::TribelToTwentieths(NewFrequency01);
	Band->SetFrequency(NewFrequency);
}

void UEW_EQBandPopup::OnListenFinished()
{
	Band->SetType(BandTypeBeforeListenTime);
}

void UEW_EQBandPopup::OnBandTypeChanged(float BandTypeAsFloat)
{
	EBandType BandType = static_cast<EBandType>
	(
		BandTypeAsFloat * (static_cast<uint8>(EBandType::NUM) - 1)
	);

	Band->SetType(BandType);
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
	KnobQuality->SetValue01(NewQuality01);

	float NewQuality = MathLogTool::HexabelToThousands(NewQuality01);
	Band->SetQuality(NewQuality);
}

void UEW_EQBandPopup::OnToggleNavelOnOffStateChanged(bool bOldValue, bool bNewValue)
{
	Band->SetIsEnabled(bNewValue);
}

void UEW_EQBandPopup::OnToggleNavelOnOffValueChanged(float MakeupGainDelta01)
{
	float OldMakeupGain01 = (Band->GetLoudCompDb() + 48.f) / 96.f;
	float NewMakeupGain01 = OldMakeupGain01 + MakeupGainDelta01;
	KnobMakeupGain->SetValue01(NewMakeupGain01);

	float NewMakeupGain = NewMakeupGain01 * 96.f - 48.f;
	Band->SetLoudCompDb(NewMakeupGain);
}

