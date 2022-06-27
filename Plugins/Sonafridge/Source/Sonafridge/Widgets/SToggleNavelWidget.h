// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateMaterialBrush.h"


class SONAFRIDGE_API SToggleNavelWidget final : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SToggleNavelWidget)
		: _Locked(false)
		, _WheelStep(0.01f)
		, _MouseFastSpeed(1.0f)
		, _MouseFineSpeed(0.2f)
		, _IsOn(false)
		, _Thickness(0.f)
		, _Blurriness(0.f)
		, _BackColor(FLinearColor::Gray)
		, _ForeColor(FLinearColor::White)
		{}

		SLATE_ATTRIBUTE(bool, Locked)
		SLATE_ATTRIBUTE(float, WheelStep)
		SLATE_ATTRIBUTE(float, MouseFastSpeed)
		SLATE_ATTRIBUTE(float, MouseFineSpeed)
		SLATE_ATTRIBUTE(bool, IsOn)
		SLATE_ATTRIBUTE(float, Thickness)
		SLATE_ATTRIBUTE(float, Blurriness)
		SLATE_ATTRIBUTE(FLinearColor, BackColor)
		SLATE_ATTRIBUTE(FLinearColor, ForeColor)
		SLATE_ATTRIBUTE(UTexture*, AlphaMask)

		SLATE_EVENT(FSimpleDelegate, MouseEntered)
		SLATE_EVENT(FSimpleDelegate, MouseLeaved)
		SLATE_EVENT(FSimpleDelegate, MouseCaptureStarted)
		SLATE_EVENT(FSimpleDelegate, MouseCaptureFinished)
		SLATE_EVENT(FSimpleDelegate, ToggleStateRequested)
		SLATE_EVENT(FOnFloatValueChanged, ValueDeltaRequested)

	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs );

	void SetBrush(const FSlateBrush& InBrush);
	UMaterialInstanceDynamic* GetMaterial() const { return Cast<UMaterialInstanceDynamic>(Brush.GetResourceObject()); }

	void UpdateMaterial();

protected:
	void           OnMouseEnter();
	virtual void   OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void           OnMouseLeave();
	virtual void   OnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void           OnMouseDrag(FReply&              InOutReply,
	                           const FGeometry&     InGeometry,
	                           const FPointerEvent& InMouseEvent,
	                           const FVector2D&     InMousePos);

	virtual void OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent) override;

	virtual int32 OnPaint(const FPaintArgs&        Args,
	                      const FGeometry&         AllottedGeometry,
	                      const FSlateRect&        MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements,
	                      int32                    LayerId,
	                      const FWidgetStyle&      InWidgetStyle,
	                      bool                     bParentEnabled) const override;
private:
	virtual FVector2D ComputeDesiredSize(float) const override;
	bool              IsInsideCircle(const FVector2D& InMousePos) const;

	TAttribute<bool>         IsLockedAttribute;
	TAttribute<float>        WheelStepAttribute;
	TAttribute<float>        MouseFastSpeedAttribute;
	TAttribute<float>        MouseFineSpeedAttribute;
	TAttribute<bool>         IsOnAttribute;
	TAttribute<float>        BlurrinessAttribute;
	TAttribute<FLinearColor> BackColorAttribute;
	TAttribute<FLinearColor> ForeColorAttribute;
	TAttribute<UTexture*>    AlphaMaskAttribute;

	FSimpleDelegate      MouseEntered;
	FSimpleDelegate      MouseLeaved;
	FSimpleDelegate      MouseCaptureStarted;
	FSimpleDelegate      MouseCaptureFinished;
	FSimpleDelegate      ToggleStateRequested;
	FOnFloatValueChanged ValueDeltaRequested;

	TSharedPtr<SImage> Image;
	FSlateBrush        Brush;
	FVector2D          LastSize;
	FVector2D          LastMousePos;
	FVector2D          PresstimeMousePos;

	bool bWasInsideCircle = false;
	bool bIsDragging = false;
};
