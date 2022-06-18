#pragma once

#include "EditorUtilityWidget.h"
#include "EW_EQFrequencyResponse.generated.h"

struct IEQSettings;
struct FEQBand;
class UEW_EQ;

UCLASS()
class SONAFRIDGEEDITOR_API UEW_EQFrequencyResponse : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UEW_EQFrequencyResponse();
	void Init(UEW_EQ* InRootWidget, TSharedPtr<IEQSettings> InSettings);

	DECLARE_EVENT_TwoParams(UEW_EQFrequencyResponse, FSizeChanged, const FVector2D& OldSize, const FVector2D& NewSize)
	FSizeChanged GetEvent_SizeChanged() const { return SizeChanged; }

protected:
	virtual void OnBandChanged(TSharedPtr<FEQBand> InBand);
	virtual void OnSizeChanged(const FVector2D& NewSize,
	                           const FVector2D& OldSize);

	virtual void   NativeConstruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual int32  NativePaint(const FPaintArgs&        Args,
	                           const FGeometry&         AllottedGeometry,
	                           const FSlateRect&        MyCullingRect,
	                           FSlateWindowElementList& OutDrawElements,
	                           int32                    LayerId,
	                           const FWidgetStyle&      InWidgetStyle,
	                           bool                     bParentEnabled) const override;

	UPROPERTY()
	UEW_EQ* RootWidget;

	TSharedPtr<IEQSettings> Settings;
	float SampleRate = 44100.f;
	TArray<TArray<FVector2D>> GridPointPairs;
	TArray<FVector2D> ResponsePoints;
	TArray<FVector2D> BandPoints;

	const int32 Resolution = 1000;
	const float DynamicMin = -48.f;
	const float DynamicMax = +48.f;
	const float ResolutionStep = 1.f / Resolution;

	/// Frequency logarithmic audible minimum. 
	const float FLMin = FMath::Loge(20.f);

	/// Frequency logarithmic audible maximun. 
	const float FLMax = FMath::Loge(20000.f);

	/// Frequency logarithmic step. 
	const float FLS = (FLMax - FLMin) * ResolutionStep;

	TSharedPtr<FEQBand> PossessedBand;
	int32 PossessedBandIndex = -1;
	int32 HoverBandIndex = -1;
	bool bWasLeftMouseButtonPressed = false;
	float PresstimeFrequency = 0.f;
	float PresstimeNAmountDb = 0.f;
	FVector2D PresstimeMousePos;
	FVector2D LastMousePos;

private:
	void BakeGrid();
	void BakeResponse();

	FSizeChanged SizeChanged;
	FVector2D    LastSize;
};
