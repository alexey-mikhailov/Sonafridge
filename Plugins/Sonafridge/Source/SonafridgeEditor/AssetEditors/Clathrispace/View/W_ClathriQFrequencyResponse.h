// Sonafridge 2022

#pragma once

#include "EditorUtilityWidget.h"
#include "W_ClathriQFrequencyResponse.generated.h"

class FVM_ClathriQ;
class UClathrispaceSettings;
class UW_ClathriQ;

UCLASS()
class SONAFRIDGEEDITOR_API UW_ClathriQFrequencyResponse : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UW_ClathriQFrequencyResponse();
	void Init(UW_ClathriQ* InRootWidget, TSharedPtr<FVM_ClathriQ> InViewModel);

	DECLARE_EVENT_TwoParams(UW_ClathriQFrequencyResponse, FSizeChanged, const FVector2D& OldSize, const FVector2D& NewSize)
	FSizeChanged GetEvent_SizeChanged() const { return SizeChanged; }

protected:
	virtual void OnPinIndexChanged(int32 NewPinIndex);
	virtual void OnBandChanging(int32 BandIndex);
	virtual void OnBandChanged(int32 BandIndex);
	virtual void OnSizeChanged(const FVector2D& NewSize,
	                           const FVector2D& OldSize);

	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, 
										   const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, 
										 const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, 
									 const FPointerEvent& InMouseEvent) override;

	virtual int32 NativePaint(const FPaintArgs&        Args,
	                          const FGeometry&         AllottedGeometry,
	                          const FSlateRect&        MyCullingRect,
	                          FSlateWindowElementList& OutDrawElements,
	                          int32                    LayerId,
	                          const FWidgetStyle&      InWidgetStyle,
	                          bool                     bParentEnabled) const override;
	UPROPERTY(Transient)
	UW_ClathriQ* RootWidget = nullptr;

	UPROPERTY(Transient)
	UClathrispaceSettings* Settings = nullptr;
	int32                  PinIndex = INDEX_NONE;

	TSharedPtr<FVM_ClathriQ> ViewModel;

	float SampleRate = 44100.f;
	TArray<TArray<FVector2D>> GridPointPairs;
	TArray<FVector2D> ResponsePoints;
	TArray<FVector2D> BandPoints;

	int32                     PossessedBandIndex = -1;
	int32                     HoverBandIndex = -1;
	bool                      bWasLeftMouseButtonPressed = false;

	float     PresstimeFrequency = 0.f;
	float     PresstimeNAmount = 0.f;
	float     PresstimeNMakeup = 0.f;
	float     PresstimeMakeupDb = 0.f;
	float     PresstimeAvgDb = 0.f;
	FVector2D PresstimeMousePos;
	FVector2D LastMousePos;

private:
	void BakeGrid();

public:
	void BakeResponse();

private:
	FVector2D GetBandWPos(int32 BandIndex);

	FSizeChanged SizeChanged;
	FVector2D    LastSize;
};
