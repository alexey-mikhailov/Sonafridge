// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KnobWidget.generated.h"

/**
 * 
 */
UCLASS()
class SONAFRIDGE_API UKnobWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UKnobWidget(const FObjectInitializer& ObjectInitializer);

	DECLARE_EVENT_TwoParams(UKnobWidget, FSizeChanged, const FVector2D& OldSize, const FVector2D& NewSize)
	FSizeChanged GetEvent_SizeChanged() const { return SizeChanged; }

	DECLARE_EVENT_TwoParams(UKnobWidget, FValueChanged, float OldValue, float NewValue)
	FValueChanged GetEvent_ValueChanged() const { return ValueChanged; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin = 0.f, UIMax = 1.f))
	float Value = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float NormalizedThickness = .1f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Blurriness = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Responsiveness = .0025;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor BackColor = FLinearColor::White;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor ForeColor = FLinearColor::White;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterial* EffectMaterial;

protected:
	virtual void NativeConstruct() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	virtual void SynchronizeProperties() override;

	void OnSizeChanged(const FVector2D& OldSize,
	                   const FVector2D& NewSize);

	virtual void NativeOnMouseEnter(const FGeometry&     InGeometry,
	                                const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry&     InGeometry,
	                                       const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry&     InGeometry,
	                                     const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry&     InGeometry,
	                                 const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent) override;

	void OnMouseDrag(FReply&              InOutReply,
	                 const FGeometry&     InGeometry,
	                 const FPointerEvent& InMouseEvent,
	                 const FVector2D&     InMousePos);

	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual int32 NativePaint(const FPaintArgs&        Args,
	                          const FGeometry&         AllottedGeometry,
	                          const FSlateRect&        MyCullingRect,
	                          FSlateWindowElementList& OutDrawElements,
	                          int32                    LayerId,
	                          const FWidgetStyle&      InWidgetStyle,
	                          bool                     bParentEnabled) const override;
private:
	bool          bIsDragging = false;
	FSizeChanged  SizeChanged;
	FValueChanged ValueChanged;
	FVector2D     LastSize;
	FVector2D     LastMousePos;
	FVector2D     PresstimeMousePos;

	TSharedPtr<SRetainerWidget> RetainerWidget;

	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance;

	bool IsInsideRing(const FVector2D& InMousePos) const;
};
