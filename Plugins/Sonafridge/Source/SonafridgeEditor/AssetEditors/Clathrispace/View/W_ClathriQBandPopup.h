// Sonafridge 2022

#pragma once

#include "SonafridgeEditor/AssetEditors/Clathrispace/ViewModel/VM_ClathriQ.h"
#include "SonafridgeEditor/SonafridgeEditorCommon.h"
#include "Sonafridge/SonafridgeCommon.h"
#include "EditorUtilityWidget.h"
#include "CoreMinimal.h"
#include "W_ClathriQBandPopup.generated.h"

class FVM_ClathriQ;
class UNaveledKnob;
class UToggleKnob;
class UTextBlock;
class UEditableTextBox;
class UW_ClathriQ;

UCLASS()
class SONAFRIDGEEDITOR_API UW_ClathriQBandPopup : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UW_ClathriQBandPopup();
	void Init(UW_ClathriQ* InRootWidget, TSharedPtr<FVM_ClathriQ> InViewModel);
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

	void OnToggleOnOffEntrance();
	void OnToggleOnOffExit();

	void OnPinIndexChanged(int32 InPinIndex);
	void OnBandSelectionChanged(int32 InBandIndex);
	void OnBandChanging(int32 BandChanging);
	void OnBandChanged(int32 BandChanging);
	void OnFrequencyDelta(float OldFrequency01, float NewFrequency01, const FPointerEvent& InMouseEvent, bool& bInOutHaveAllHandlersAccepted);
	void OnAmountDelta(float OldAmount01, float NewAmount01, const FPointerEvent& InMouseEvent, bool& bInOutHaveAllHandlersAccepted);
	void OnQualityDelta(float OldQuality01, float NewQuality01, const FPointerEvent& InMouseEvent, bool& bInOutHaveAllHandlersAccepted);
	void OnMakeupGainDelta(float OldMakeupGain01, float NewMakeupGain01, const FPointerEvent& InMouseEvent, bool& bInOutHaveAllHandlersAccepted);
	void OnListenDelta(float FrequencyDelta, const FPointerEvent& InMouseEvent);
	void OnToggleNavelRemoveValueChanged(float QualityDelta01, const FPointerEvent& InMouseEvent);
	void OnToggleNavelOnOffValueChanged(float MakeupGainDelta01, const FPointerEvent& InMouseEvent);
	void OnToggleNavelOnOffStateChanged(bool bOldValue, bool bNewValue);

	void OnFrequencyCapture();
	void OnFrequencyCommit();
	void OnAmountCapture();
	void OnAmountCommit();
	void OnQualityCapture();
	void OnQualityCommit();
	void OnMakeupGainCapture();
	void OnMakeupGainCommit();

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

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	void                  RefreshVisual();
	static EEQBandType    GetBandTypeByPopupType(EBandPopupType InBandPopupType);
	static EBandPopupType GetPopupTypeByBandType(EEQBandType InBandPopupType);
	FVector2D             GetBandWPos();
	void                  TryAutoMakeup(const FPointerEvent& InMouseEvent);

	UPROPERTY(Transient)
	UW_ClathriQ* RootWidget = nullptr;

	TSharedPtr<FVM_ClathriQ> ViewModel;
	int32                    BandIndex = INDEX_NONE;
	int32                    PinIndex = INDEX_NONE;

	float PresstimeMakeupDb = 0.f;
	float PresstimeAvgDb = 0.f;

	EEQBandType               BandTypeBeforeListenTime = EEQBandType::AttBand;
	float                     BandMakeupBeforeListenTime = 0.f;
	TArray<bool>              BandsActivityBeforeListenTime;

	EBandPopupFocusMode FocusMode = EBandPopupFocusMode::Frequency;
	bool                bIsAltKeyDown = false;
};
