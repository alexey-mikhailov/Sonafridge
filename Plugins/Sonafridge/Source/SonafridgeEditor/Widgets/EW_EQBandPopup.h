// Sonafridge 2022

#pragma once

#include "Sonafridge/Settings_EQ/EQBand.h"
#include "EditorUtilityWidget.h"
#include "CoreMinimal.h"
#include "EW_EQBandPopup.generated.h"

struct IEQSettings;
class UNaveledKnob;
class UToggleKnob;
class UTextBlock;
class UEditableTextBox;
class UEW_EQ;

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
	void Init(UEW_EQ* InRootWidget, TSharedPtr<IEQSettings> InSettings);
	void FollowBand();

protected:
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
	
	void OnBandSelectionChanged(TSharedPtr<FEQBand> InBand);
	void OnBandChanging(TSharedPtr<FEQBand> InBand);
	void OnBandChanged(TSharedPtr<FEQBand> InBand);
	void OnFrequencyDelta(float OldFrequency01, float NewFrequency01);
	void OnAmountDelta(float OldAmount01, float NewAmount01);
	void OnQualityDelta(float OldQuality01, float NewQuality01);
	void OnMakeupGainDelta(float OldMakeupGain01, float NewMakeupGain01);
	void OnListenDelta(float FrequencyDelta);
	void OnBandTypeChanged(float BandTypeDeltaAsFloat);
	void OnToggleNavelRemoveValueChanged(float QualityDelta01);
	void OnToggleNavelOnOffValueChanged(float MakeupGainDelta01);
	void OnRemoveClick();
	void OnToggleNavelOnOffStateChanged(bool bOldValue, bool bNewValue);
	void OnFrequencyCommit();
	void OnAmountCommit();
	void OnQualityCommit();
	void OnMakeupGainCommit();
	void OnListenStarted();
	void OnListenFinished();

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitType);

	virtual int32 NativePaint(const FPaintArgs&        Args,
	                          const FGeometry&         AllottedGeometry,
	                          const FSlateRect&        MyCullingRect,
	                          FSlateWindowElementList& OutDrawElements,
	                          int32                    LayerId,
	                          const FWidgetStyle&      InWidgetStyle,
	                          bool                     bParentEnabled) const override;
private:
	void RefreshVisual();
	static EBandType GetBandTypeByPopupType(EBandPopupType InBandPopupType);
	UTexture* GetBandIconByType(EBandPopupType InBandPopup);
	FVector2D GetBandWPos();

	UPROPERTY()
	UEW_EQ* RootWidget;

	TSharedPtr<IEQSettings> Settings;
	TSharedPtr<FEQBand>     Band;
	float                   BandTypeFloat = 0.f;
	EBandType               BandTypeBeforeListenTime = EBandType::BandCut;
	EBandPopupFocusMode     FocusMode = EBandPopupFocusMode::Frequency;
};
