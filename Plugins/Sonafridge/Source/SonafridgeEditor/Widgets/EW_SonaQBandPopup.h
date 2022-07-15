// Sonafridge 2022

#pragma once

#include "Model/VM_SonaQBand.h"
#include "EditorUtilityWidget.h"
#include "CoreMinimal.h"
#include "EW_SonaQBandPopup.generated.h"

class FVM_SonaQ;
class UNaveledKnob;
class UToggleKnob;
class UTextBlock;
class UEditableTextBox;
class UEW_SonaQ;

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
	CutLow,
	CutHigh,
	AttLow,
	AttHigh,
	AttBand,
	Notch,
};

UCLASS()
class SONAFRIDGEEDITOR_API UEW_SonaQBandPopup : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	void Init(UEW_SonaQ* InRootWidget, TSharedPtr<FVM_SonaQ> InViewModel);
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
	
	void OnBandSelectionChanged(TSharedPtr<FVM_SonaQBand> InBand);
	void OnBandChanging(TSharedPtr<FVM_SonaQBand> InBand);
	void OnBandChanged(TSharedPtr<FVM_SonaQBand> InBand);
	void OnFrequencyDelta(float OldFrequency01, float NewFrequency01, bool& bInOutHaveAllHandlersAccepted);
	void OnAmountDelta(float OldAmount01, float NewAmount01, bool& bInOutHaveAllHandlersAccepted);
	void OnQualityDelta(float OldQuality01, float NewQuality01, bool& bInOutHaveAllHandlersAccepted);
	void OnMakeupGainDelta(float OldMakeupGain01, float NewMakeupGain01, bool& bInOutHaveAllHandlersAccepted);
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

	void OnFrequencyClick();
	void OnAmountClick();
	void OnQualityClick();
	void OnMakeupClick();

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
	void                  RefreshVisual();
	static EEQBandType    GetBandTypeByPopupType(EBandPopupType InBandPopupType);
	static EBandPopupType GetPopupTypeByBandType(EEQBandType InBandPopupType);
	UTexture*             GetBandIconByType(EBandPopupType InBandPopup);
	FVector2D             GetBandWPos();

	UPROPERTY()
	UEW_SonaQ* RootWidget;

	TSharedPtr<FVM_SonaQ>     ViewModel;
	TSharedPtr<FVM_SonaQBand> Band;
	float                     BandPopupTypeFloat = 0.f;

	EEQBandType               BandTypeBeforeListenTime = EEQBandType::AttBand;
	float                     BandMakeupBeforeListenTime = 0.f;
	TArray<bool>              BandsActivityBeforeListenTime;

	EBandPopupFocusMode       FocusMode = EBandPopupFocusMode::Frequency;
};
