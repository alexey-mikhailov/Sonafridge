// Sonafridge 2022

#pragma once

#include "EditorUtilityWidget.h"
#include "EW_SonaQFrequencyResponse.generated.h"

class FVM_SonaQ;
class FVM_SonaQBand;
class UEW_SonaQ;

UCLASS()
class SONAFRIDGEEDITOR_API UEW_SonaQFrequencyResponse : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UEW_SonaQFrequencyResponse();
	void Init(UEW_SonaQ* InRootWidget, TSharedPtr<FVM_SonaQ> InViewModel);

	DECLARE_EVENT_TwoParams(UEW_SonaQFrequencyResponse, FSizeChanged, const FVector2D& OldSize, const FVector2D& NewSize)
	FSizeChanged GetEvent_SizeChanged() const { return SizeChanged; }

protected:
	virtual void OnBandChanging(TSharedPtr<FVM_SonaQBand> InBand);
	virtual void OnBandChanged(TSharedPtr<FVM_SonaQBand> InBand);
	virtual void OnSizeChanged(const FVector2D& NewSize,
	                           const FVector2D& OldSize);

	virtual void   NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual int32  NativePaint(const FPaintArgs&        Args,
	                           const FGeometry&         AllottedGeometry,
	                           const FSlateRect&        MyCullingRect,
	                           FSlateWindowElementList& OutDrawElements,
	                           int32                    LayerId,
	                           const FWidgetStyle&      InWidgetStyle,
	                           bool                     bParentEnabled) const override;

	UPROPERTY()
	UEW_SonaQ* RootWidget;

	TSharedPtr<FVM_SonaQ> ViewModel;
	float SampleRate = 44100.f;
	TArray<TArray<FVector2D>> GridPointPairs;
	TArray<FVector2D> ResponsePoints;
	TArray<FVector2D> BandPoints;

	const int32 Resolution = 1000;
	const float ResolutionStep = 1.f / Resolution;

	/// Frequency logarithmic audible minimum. 
	const float FLMin = FMath::Loge(20.f);

	/// Frequency logarithmic audible maximun. 
	const float FLMax = FMath::Loge(20000.f);

	/// Frequency logarithmic step. 
	const float FLS = (FLMax - FLMin) * ResolutionStep;

	TSharedPtr<FVM_SonaQBand> PossessedBand;
	int32 PossessedBandIndex = -1;
	int32 HoverBandIndex = -1;
	bool bWasLeftMouseButtonPressed = false;
	float PresstimeFrequency = 0.f;
	float PresstimeNAmountDb = 0.f;
	FVector2D PresstimeMousePos;
	FVector2D LastMousePos;

private:
	void      BakeGrid();

public:
	void      BakeResponse();

private:
	FVector2D GetBandWPos(TSharedPtr<FVM_SonaQBand> InBand);

	FSizeChanged SizeChanged;
	FVector2D    LastSize;
};
