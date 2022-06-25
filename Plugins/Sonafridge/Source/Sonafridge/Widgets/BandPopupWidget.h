#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sonafridge/Settings_EQ/EQBand.h"
#include "BandPopupWidget.generated.h"

class UKnobWidget;
class UNavelWidget;
class UToggleNavelWidget;


UCLASS()
class SONAFRIDGE_API UBandPopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UKnobWidget* KnobFrequency;

	UPROPERTY(meta = (BindWidget))
	UNavelWidget* NavelListen;

	UPROPERTY(meta = (BindWidget))
	UKnobWidget* KnobAmount;

	UPROPERTY(meta = (BindWidget))
	UNavelWidget* NavelBandType;

	UPROPERTY(meta = (BindWidget))
	UKnobWidget* KnobQuality;

	UPROPERTY(meta = (BindWidget))
	UToggleNavelWidget* ToggleNavelRemove;

	UPROPERTY(meta = (BindWidget))
	UKnobWidget* KnobMakeupGain;

	UPROPERTY(meta = (BindWidget))
	UToggleNavelWidget* ToggleNavelOnOff;

protected:
	virtual void NativeConstruct() override;
	void         OnFrequencyChanged(float OldValue, float NewValue);
	void         OnListenStarted();
	void         OnListenDelta(float FrequencyDelta);
	void         OnListenFinished();
	void         OnAmountChanged(float OldValue, float NewValue);
	void         OnQualityChanged(float OldValue, float NewValue);
	void         OnMakeupGainChanged(float OldValue, float NewValue);

private:
	TSharedPtr<FEQBand> Model;
	EBandType BandTypeAtListenTime;

	// TODO: Move all these fields inside EQSettings;
	float     Frequency;
	float     Amount;
	float     Quality;
	float     MakeupGain;
	EBandType BandType;
	bool      bIsOnOff;
};
