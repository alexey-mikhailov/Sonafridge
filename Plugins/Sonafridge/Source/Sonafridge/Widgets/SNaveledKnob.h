// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateMaterialBrush.h"


class SONAFRIDGE_API SNaveledKnob final : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SNaveledKnob)
		: _Locked(false)
		, _WheelStep(0.01f)
		, _MouseFastSpeed(1.0f)
		, _MouseFineSpeed(0.2f)
		, _KnobValue(0.0f)
		, _NavelValue(0.0f)
		, _KnobNavelThreshold(0.f)
		, _Blurriness(0.f)
		, _Knob0Color(FLinearColor::Gray)
		, _Knob1Color(FLinearColor::White)
		, _Navel0Color(FLinearColor::White)
		, _Navel1Color(FLinearColor::White)
		, _Icon(nullptr)
		, _IconScale(1.f)
		{}

		SLATE_ATTRIBUTE(bool, Locked)
		SLATE_ATTRIBUTE(float, WheelStep)
		SLATE_ATTRIBUTE(float, MouseFastSpeed)
		SLATE_ATTRIBUTE(float, MouseFineSpeed)
		SLATE_ATTRIBUTE(float, KnobValue)
		SLATE_ATTRIBUTE(float, NavelValue)
		SLATE_ATTRIBUTE(float, KnobNavelThreshold)
		SLATE_ATTRIBUTE(float, Blurriness)
		SLATE_ATTRIBUTE(FLinearColor, Knob0Color)
		SLATE_ATTRIBUTE(FLinearColor, Knob1Color)
		SLATE_ATTRIBUTE(FLinearColor, Navel0Color)
		SLATE_ATTRIBUTE(FLinearColor, Navel1Color)
		SLATE_ATTRIBUTE(UTexture*, Icon)
		SLATE_ATTRIBUTE(float, IconScale)

		SLATE_EVENT(FSimpleDelegate, KnobCaptureStarted)
		SLATE_EVENT(FSimpleDelegate, KnobCaptureFinished)
		SLATE_EVENT(FSimpleDelegate, NavelCaptureStarted)
		SLATE_EVENT(FSimpleDelegate, NavelCaptureFinished)
		SLATE_EVENT(FOnFloatValueChanged, KnobDeltaRequested)
		SLATE_EVENT(FOnFloatValueChanged, NavelDeltaRequested)

	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs );

	void SetBrush(const FSlateBrush& InBrush);
	UMaterialInstanceDynamic* GetMaterial() const { return Cast<UMaterialInstanceDynamic>(Brush.GetResourceObject()); }

	void UpdateMaterial();

protected:
	void           OnKnobEnter();
	void           OnNavelEnter();
	virtual void   OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void           OnKnobLeave();
	void           OnNavelLeave();
	virtual void   OnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void OnKnobDrag(FReply&              InOutReply,
	                const FGeometry&     InGeometry,
	                const FPointerEvent& InMouseEvent,
	                const FVector2D&     InMousePos);

	void OnNavelDrag(FReply&              InOutReply,
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
	bool              IsInsideKnob(const FVector2D& InMousePos) const;
	bool              IsInsideNavel(const FVector2D& InMousePos) const;

	TAttribute<bool>         IsLockedAttribute;
	TAttribute<float>        WheelStepAttribute;
	TAttribute<float>        MouseFastSpeedAttribute;
	TAttribute<float>        MouseFineSpeedAttribute;
	TAttribute<float>        KnobValueAttribute;
	TAttribute<float>        NavelValueAttribute;
	TAttribute<float>        KnobNavelThreshold;
	TAttribute<float>        BlurrinessAttribute;
	TAttribute<FLinearColor> Knob0ColorAttribute;
	TAttribute<FLinearColor> Knob1ColorAttribute;
	TAttribute<FLinearColor> Navel0ColorAttribute;
	TAttribute<FLinearColor> Navel1ColorAttribute;
	TAttribute<UTexture*>    IconAttribute;
	TAttribute<float>        IconScaleAttribute;

	FSimpleDelegate           KnobCaptureStarted;
	FSimpleDelegate           KnobCaptureFinished;
	FSimpleDelegate           NavelCaptureStarted;
	FSimpleDelegate           NavelCaptureFinished;
	FOnFloatValueChanged      KnobDeltaRequested;
	FOnFloatValueChanged      NavelDeltaRequested;

	TSharedPtr<SImage> Image;
	FSlateBrush        Brush;
	FVector2D          LastSize;
	FVector2D          LastMousePos;
	FVector2D          PresstimeMousePos;

	bool bWasInsideKnob = false;
	bool bWasInsideNavel = false;
	bool bIsDraggingKnob = false;
	bool bIsDraggingNavel = false;
};
