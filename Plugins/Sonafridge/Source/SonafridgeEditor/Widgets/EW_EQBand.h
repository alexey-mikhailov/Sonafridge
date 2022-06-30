// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Sonafridge/Settings_EQ/EQSettingsMock.h"
#include "EW_EQBand.generated.h"

class UBorder;
struct FEQBand;
class UTextBlock;
class USynthKnob;
class UEW_EQ;

UCLASS()
class UProxy_EQBand : public UObject
{
	GENERATED_BODY()

public:
	TSharedPtr<IEQSettings> Settings;
	TSharedPtr<FEQBand> Band;

	UPROPERTY()
	UEW_EQ* RootWidget;
};

UCLASS()
class SONAFRIDGEEDITOR_API UEW_EQBand : public UEditorUtilityWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	TSharedPtr<FEQBand> GetBand() const { return Band; }

protected:
	virtual void  NativeConstruct() override;
	virtual void  NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual int32 NativePaint(const FPaintArgs&        Args,
	                          const FGeometry&         AllottedGeometry,
	                          const FSlateRect&        MyCullingRect,
	                          FSlateWindowElementList& OutDrawElements,
	                          int32                    LayerId,
	                          const FWidgetStyle&      InWidgetStyle,
	                          bool                     bParentEnabled) const override;

	TSharedPtr<IEQSettings> Settings;
	TSharedPtr<FEQBand> Band;

	UPROPERTY()
	UEW_EQ* RootWidget;

	UPROPERTY(meta = (BindWidget))
	UBorder* BorderSelectionMarker;

	UPROPERTY(meta = (BindWidget))
	USynthKnob* KnobLoudnessCompensation;

	UPROPERTY(meta = (BindWidget))
	USynthKnob* KnobFrequency;

	UPROPERTY(meta = (BindWidget))
	USynthKnob* KnobQuality;

	UPROPERTY(meta = (BindWidget))
	USynthKnob* KnobAmountDb;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextLoudnessCompensation;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextFrequency;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextQuality;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextAmountDb;

private:
	UFUNCTION()
	void OnLoudnessCompensationChanged(float NewValue);

	UFUNCTION()
	void OnFrequencyChanged(float NewValue);

	UFUNCTION()
	void OnQualityChanged(float NewValue);

	UFUNCTION()
	void OnAmountDbChanged(float NewValue);

	void OnBandSelectionChanged(TSharedPtr<FEQBand> InBand);
	void OnBandChanged(TSharedPtr<FEQBand> InBand);

	FORCEINLINE bool IsValidDataContext()
	{
		if (Band) return true;

		UE_LOG(LogTemp, Error, TEXT("UEW_EQBand: Data context is not set. "));
		return false;
	}

	FORCEINLINE static FText FormatDb(float Value)
	{
		FNumberFormattingOptions NumberFormatOptions;
		NumberFormatOptions.AlwaysSign = true;
		NumberFormatOptions.UseGrouping = false;
		NumberFormatOptions.RoundingMode = HalfToZero;
		NumberFormatOptions.MinimumIntegralDigits = 0;
		NumberFormatOptions.MaximumIntegralDigits = 32;
		NumberFormatOptions.MinimumFractionalDigits = 1;
		NumberFormatOptions.MaximumFractionalDigits = 1;

		return FText::Format(NSLOCTEXT("", "", "{0} dB"), FText::AsNumber(Value, &NumberFormatOptions));
	}

	FORCEINLINE static FText FormatHz(float Value)
	{
		FNumberFormattingOptions NumberFormatOptions;
		NumberFormatOptions.AlwaysSign = false;
		NumberFormatOptions.UseGrouping = false;
		NumberFormatOptions.RoundingMode = HalfToZero;
		NumberFormatOptions.MinimumIntegralDigits = 0;
		NumberFormatOptions.MaximumIntegralDigits = 32;
		NumberFormatOptions.MinimumFractionalDigits = 1;
		NumberFormatOptions.MaximumFractionalDigits = 1;

		return FText::Format(NSLOCTEXT("", "", "{0} Hz"), FText::AsNumber(Value, &NumberFormatOptions));
	}

	FORCEINLINE static FText FormatQunits(float Value)
	{
		FNumberFormattingOptions NumberFormatOptions;
		NumberFormatOptions.AlwaysSign = false;
		NumberFormatOptions.UseGrouping = false;
		NumberFormatOptions.RoundingMode = HalfToZero;
		NumberFormatOptions.MinimumIntegralDigits = 0;
		NumberFormatOptions.MaximumIntegralDigits = 32;
		NumberFormatOptions.MinimumFractionalDigits = 3;
		NumberFormatOptions.MaximumFractionalDigits = 3;

		return FText::AsNumber(Value, &NumberFormatOptions);
	}
};
