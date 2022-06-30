#pragma once

#include "Sonafridge/Settings_EQ/EQBand.h"
#include "EditorUtilityWidget.h"
#include "CoreMinimal.h"
#include "EW_EQBandPopup.generated.h"

class UNaveledKnob;
class UToggleKnob;
class UTextBlock;
class UEditableTextBox;

UENUM()
enum class EBandPopupFocusMode : uint8
{
	None,
	Frequency,
	Amount,
	Quality,
	Makeup
};


UCLASS()
class SONAFRIDGEEDITOR_API UEW_EQBandPopup : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UNaveledKnob* NaveledKnobFrequency;

	UPROPERTY(meta = (BindWidget))
	UNaveledKnob* NaveledKnobAmount;

	UPROPERTY(meta = (BindWidget))
	UToggleKnob* ToggleKnobQuality;

	UPROPERTY(meta = (BindWidget))
	UToggleKnob* ToggleKnobMakeupGain;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlockKey;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TextBoxValue;

protected:
	UFUNCTION()
	virtual void NativeConstruct() override;

	void OnFrequencyChanged(float OldFrequency01, float NewFrequency01);
	void OnAmountChanged(float OldAmount01, float NewAmount01);
	void OnQualityChanged(float OldQuality01, float NewQuality01);
	void OnMakeupGainChanged(float OldMakeupGain01, float NewMakeupGain01);
	void OnListenStarted();
	void OnListenDelta(float FrequencyDelta);
	void OnListenFinished();
	void OnBandTypeChanged(float BandTypeDeltaAsFloat);
	void OnToggleNavelRemoveStateChanged(bool bOldValue, bool bNewValue);
	void OnToggleNavelRemoveValueChanged(float QualityDelta01);
	void OnToggleNavelOnOffStateChanged(bool bOldValue, bool bNewValue);
	void OnToggleNavelOnOffValueChanged(float MakeupGainDelta01);

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitType);

private:
	TSharedPtr<FEQBand> Band;
	EBandType           BandTypeBeforeListenTime = EBandType::BandCut;
	EBandPopupFocusMode FocusMode = EBandPopupFocusMode::None;
	float               BandTypeFloat = 0.f;
};