// Sonafridge 2022

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

UENUM()
enum class EBandPopupType : uint8
{
	LowCut,
	HighCut,
	LowShelf,
	HighShelf,
	BandCut,
	Notch,
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
	UNaveledKnob* NaveledKnobQuality;

	UPROPERTY(meta = (BindWidget))
	UToggleKnob* ToggleKnobMakeupGain;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlockKey;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TextBoxValue;

	UPROPERTY(EditDefaultsOnly)
	UTexture* IconLowCut;

	UPROPERTY(EditDefaultsOnly)
	UTexture* IconHighCut;

	UPROPERTY(EditDefaultsOnly)
	UTexture* IconLowShelf;

	UPROPERTY(EditDefaultsOnly)
	UTexture* IconHighShelf;

	UPROPERTY(EditDefaultsOnly)
	UTexture* IconBandCut;

	UPROPERTY(EditDefaultsOnly)
	UTexture* IconNotch;

protected:
	UFUNCTION()
	virtual void NativeConstruct() override;

	void OnFrequencyEntrance();
	void OnAmountEntrance();
	void OnQualityEntrance();
	void OnMakeupGainEntrance();

	void OnListenEntrance();
	void OnBandTypeEntrance();
	void OnButtonRemoveEntrance();
	void OnToggleOnOffEntrance();

	void OnListenExit();
	void OnBandTypeExit();
	void OnButtonRemoveExit();
	void OnToggleOnOffExit();
	
	void OnFrequencyChanged(float OldFrequency01, float NewFrequency01);
	void OnAmountChanged(float OldAmount01, float NewAmount01);
	void OnQualityChanged(float OldQuality01, float NewQuality01);
	void OnMakeupGainChanged(float OldMakeupGain01, float NewMakeupGain01);
	void OnListenStarted();
	void OnListenDelta(float FrequencyDelta);
	void OnListenFinished();
	void OnBandTypeChanged(float BandTypeDeltaAsFloat);
	void OnRemoveClick();
	void OnToggleNavelRemoveValueChanged(float QualityDelta01);
	void OnToggleNavelOnOffStateChanged(bool bOldValue, bool bNewValue);
	void OnToggleNavelOnOffValueChanged(float MakeupGainDelta01);

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitType);

private:
	static EBandType GetBandTypeByPopupType(EBandPopupType InBandPopupType);
	UTexture* GetBandIconByType(EBandPopupType InBandPopup);

	TSharedPtr<FEQBand> Band;
	EBandType           BandTypeBeforeListenTime = EBandType::BandCut;
	EBandPopupFocusMode FocusMode = EBandPopupFocusMode::None;
	float               BandTypeFloat = 0.f;
};
