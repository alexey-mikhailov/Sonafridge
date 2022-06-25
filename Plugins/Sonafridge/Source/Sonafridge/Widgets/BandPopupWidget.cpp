// Fill out your copyright notice in the Description page of Project Settings.


#include "BandPopupWidget.h"

#include "KnobWidget.h"
#include "NavelWidget.h"


void UBandPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	KnobFrequency->GetEvent_ValueChanged().AddUObject(this, &UBandPopupWidget::OnFrequencyChanged);
	KnobAmount->GetEvent_ValueChanged().AddUObject(this, &UBandPopupWidget::OnAmountChanged);
	KnobQuality->GetEvent_ValueChanged().AddUObject(this, &UBandPopupWidget::OnQualityChanged);
	KnobMakeupGain->GetEvent_ValueChanged().AddUObject(this, &UBandPopupWidget::OnMakeupGainChanged);

	NavelListen->GetEvent_MouseCaptureStarted().AddUObject(this, &UBandPopupWidget::OnListenStarted);
	NavelListen->GetEvent_MouseCaptureFinished().AddUObject(this, &UBandPopupWidget::OnListenFinished);
	NavelListen->GetEvent_ValueChanged().AddUObject(this, &UBandPopupWidget::OnListenDelta);
}

void UBandPopupWidget::OnFrequencyChanged(float OldValue, float NewValue)
{
	Frequency = NewValue;
}

void UBandPopupWidget::OnListenStarted()
{
	// TODO: Set band type to Band Pass temporary.
	BandTypeAtListenTime = Model->GetType();
}

void UBandPopupWidget::OnListenDelta(float FrequencyDelta)
{
	Frequency += FrequencyDelta;
	KnobFrequency->Value = Frequency;
}

void UBandPopupWidget::OnListenFinished()
{
	// TODO: Recover band type to Band Pass temporary. 
}

void UBandPopupWidget::OnAmountChanged(float OldValue, float NewValue)
{
	Amount = NewValue;
}

void UBandPopupWidget::OnQualityChanged(float OldValue, float NewValue)
{
	Quality = NewValue;
}

void UBandPopupWidget::OnMakeupGainChanged(float OldValue, float NewValue)
{
	MakeupGain = NewValue;
}
