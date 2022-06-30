// Sonafridge 2022

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
		, _Value(0.0f)
		, _KnobNavelThreshold(0.f)
		, _Blurriness(0.f)
		, _Knob0Color(FLinearColor::Gray)
		, _Knob1Color(FLinearColor::White)
		, _NavelColor(FLinearColor::White)
		, _IdleIcon(nullptr)
		, _HoverIcon(nullptr)
		, _IconScale(1.f)
		{}

		SLATE_ATTRIBUTE(bool, Locked)
		SLATE_ATTRIBUTE(float, WheelStep)
		SLATE_ATTRIBUTE(float, MouseFastSpeed)
		SLATE_ATTRIBUTE(float, MouseFineSpeed)
		SLATE_ATTRIBUTE(float, Value)
		SLATE_ATTRIBUTE(float, KnobNavelThreshold)
		SLATE_ATTRIBUTE(float, Blurriness)
		SLATE_ATTRIBUTE(FLinearColor, Knob0Color)
		SLATE_ATTRIBUTE(FLinearColor, Knob1Color)
		SLATE_ATTRIBUTE(FLinearColor, NavelColor)
		SLATE_ATTRIBUTE(UTexture*, IdleIcon)
		SLATE_ATTRIBUTE(UTexture*, HoverIcon)
		SLATE_ATTRIBUTE(float, IconScale)

		SLATE_EVENT(FSimpleDelegate, KnobEntrance)
		SLATE_EVENT(FSimpleDelegate, NavelEntrance)
		SLATE_EVENT(FSimpleDelegate, KnobExit)
		SLATE_EVENT(FSimpleDelegate, NavelExit)
		SLATE_EVENT(FSimpleDelegate, KnobCaptureStarted)
		SLATE_EVENT(FSimpleDelegate, KnobCaptureFinished)
		SLATE_EVENT(FSimpleDelegate, NavelCaptureStarted)
		SLATE_EVENT(FSimpleDelegate, NavelCaptureFinished)
		SLATE_EVENT(FSimpleDelegate, NavelClick)
		SLATE_EVENT(FOnFloatValueChanged, KnobDeltaRequested)
		SLATE_EVENT(FOnFloatValueChanged, NavelDeltaRequested)

	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs );

	void SetBrush(const FSlateBrush& InBrush);
	UMaterialInstanceDynamic* GetMaterial() const { return Cast<UMaterialInstanceDynamic>(Brush.GetResourceObject()); }

	void UpdateMaterial();

protected:
	void           OnKnobEntrance();
	void           OnNavelEntrance();
	virtual void   OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void           OnKnobExit();
	void           OnNavelExit();
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
	TAttribute<float>        ValueAttribute;
	TAttribute<float>        KnobNavelThreshold;
	TAttribute<float>        BlurrinessAttribute;
	TAttribute<FLinearColor> Knob0ColorAttribute;
	TAttribute<FLinearColor> Knob1ColorAttribute;
	TAttribute<FLinearColor> NavelColorAttribute;
	TAttribute<UTexture*>    IdleIconAttribute;
	TAttribute<UTexture*>    HoverIconAttribute;
	TAttribute<float>        IconScaleAttribute;

	FSimpleDelegate      KnobEntrance;
	FSimpleDelegate      NavelEntrance;
	FSimpleDelegate      KnobExit;
	FSimpleDelegate      NavelExit;
	FSimpleDelegate      KnobCaptureStarted;
	FSimpleDelegate      KnobCaptureFinished;
	FSimpleDelegate      NavelCaptureStarted;
	FSimpleDelegate      NavelCaptureFinished;
	FOnFloatValueChanged KnobDeltaRequested;
	FOnFloatValueChanged NavelDeltaRequested;
	FSimpleDelegate      NavelClick;

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
