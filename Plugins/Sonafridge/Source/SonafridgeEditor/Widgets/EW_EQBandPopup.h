#pragma once

#include "Sonafridge/Settings_EQ/EQBand.h"
#include "EditorUtilityWidget.h"
#include "CoreMinimal.h"
#include "EW_EQBandPopup.generated.h"

class UKnobWidget;
class UNavelWidget;
class UToggleNavelWidget;


UCLASS()
class SONAFRIDGEEDITOR_API UEW_EQBandPopup : public UEditorUtilityWidget
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

	void OnFrequencyChanged(float OldFrequency01, float NewFrequency01);
	void OnAmountChanged(float OldAmount01, float NewAmount01);
	void OnQualityChanged(float OldQuality01, float NewQuality01);
	void OnMakeupGainChanged(float OldMakeupGain01, float NewMakeupGain01);
	void OnListenStarted();
	void OnListenDelta(float FrequencyDelta);
	void OnListenFinished();
	void OnBandTypeChanged(float BandTypeAsFloat);
	void OnToggleNavelRemoveStateChanged(bool bOldValue, bool bNewValue);
	void OnToggleNavelRemoveValueChanged(float QualityDelta01);
	void OnToggleNavelOnOffStateChanged(bool bOldValue, bool bNewValue);
	void OnToggleNavelOnOffValueChanged(float MakeupGainDelta01);

private:
	TSharedPtr<FEQBand> Band;
	EBandType BandTypeBeforeListenTime;
};
