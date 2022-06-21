// Fill out your copyright notice in the Description page of Project Settings.


#include "SKnobWidget.h"

#include "SlateMaterialBrush.h"
#include "Widgets/Images/SImage.h"

void SKnobWidget::Construct(const FArguments& InArgs)
{
	IsLockedAttribute = InArgs._Locked;
	WheelStepAttribute = InArgs._WheelStep;
	MouseFastSpeedAttribute = InArgs._MouseFastSpeed;
	MouseFineSpeedAttribute = InArgs._MouseFineSpeed;
	ValueAttribute = InArgs._Value;
	ThicknessAttribute = InArgs._Thickness;
	BlurrinessAttribute = InArgs._Blurriness;
	BackColorAttribute = InArgs._BackColor;
	ForeColorAttribute = InArgs._ForeColor;

	MouseEntered = InArgs._MouseEntered;
	MouseLeaved = InArgs._MouseLeaved;
	MouseCaptureStarted = InArgs._MouseCaptureStarted;
	MouseCaptureFinished = InArgs._MouseCaptureFinished;
	ValueDeltaRequested = InArgs._ValueDeltaRequested;

	Image = SNew(SImage);
}

void SKnobWidget::SetBrush(const FSlateBrush& InBrush)
{
	Brush = InBrush;
	Image->SetImage(&Brush);

	UObject* BrushResource = Brush.GetResourceObject();
	UMaterialInstanceDynamic* BrushMID = Cast<UMaterialInstanceDynamic>(BrushResource);

	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Value", ValueAttribute.Get());
		BrushMID->SetScalarParameterValue("Thickness", ThicknessAttribute.Get());
		BrushMID->SetScalarParameterValue("Blurriness", BlurrinessAttribute.Get());
		BrushMID->SetVectorParameterValue("BackColor", BackColorAttribute.Get());
		BrushMID->SetVectorParameterValue("ForeColor", ForeColorAttribute.Get());
	}
}

void SKnobWidget::UpdateMaterial()
{
	auto BrushMID = GetMaterial();

	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Value", ValueAttribute.Get());
		BrushMID->SetScalarParameterValue("Thickness", ThicknessAttribute.Get());
		BrushMID->SetScalarParameterValue("Blurriness", BlurrinessAttribute.Get());
		BrushMID->SetVectorParameterValue("BackColor", BackColorAttribute.Get());
		BrushMID->SetVectorParameterValue("ForeColor", ForeColorAttribute.Get());
	}
}

void SKnobWidget::OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SLeafWidget::OnMouseEnter(InGeometry, InMouseEvent);
	MouseEntered.ExecuteIfBound();
}

void SKnobWidget::OnMouseLeave(const FPointerEvent& InMouseEvent)
{
	SLeafWidget::OnMouseLeave(InMouseEvent);
	MouseLeaved.ExecuteIfBound();
}

FReply SKnobWidget::OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = SLeafWidget::OnMouseWheel(InGeometry, InMouseEvent);
	
	ValueDeltaRequested.ExecuteIfBound(WheelStepAttribute.Get());

	return Reply;
}

FReply SKnobWidget::OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = SLeafWidget::OnMouseButtonDown(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (IsInsideRing(MousePos))
	{
		Reply = Reply.CaptureMouse(AsShared());
		MouseCaptureStarted.ExecuteIfBound();
		PresstimeMousePos = MousePos;
		bIsDragging = true;
	}

	return Reply;
}

FReply SKnobWidget::OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = SLeafWidget::OnMouseButtonUp(InGeometry, InMouseEvent);

	if (bIsDragging)
	{
		Reply = Reply.ReleaseMouseCapture();
		MouseCaptureFinished.ExecuteIfBound();
		bIsDragging = false;
	}

	return Reply;
}

FReply SKnobWidget::OnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = SLeafWidget::OnMouseMove(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (bIsDragging)
	{
		OnMouseDrag(Reply, InGeometry, InMouseEvent, MousePos);
	}

	LastMousePos = MousePos;
	return Reply;
}

void SKnobWidget::OnMouseDrag(FReply&              InOutReply,
                              const FGeometry&     InGeometry,
                              const FPointerEvent& InMouseEvent,
                              const FVector2D&     InMousePos)
{
	FVector2D MouseDelta = InMousePos - LastMousePos;

	// Mouse delta per ~1Kpx-height screen.
	constexpr float ScreensPerPxApprox = -.001f;

	float ValueDelta = InMouseEvent.IsShiftDown()
	                   ? ValueDelta = MouseFineSpeedAttribute.Get() * MouseDelta.Y * ScreensPerPxApprox
	                   : ValueDelta = MouseFastSpeedAttribute.Get() * MouseDelta.Y * ScreensPerPxApprox;

	UE_LOG(LogTemp, Log, TEXT("Value delta is %f"), ValueDelta);

	ValueDeltaRequested.ExecuteIfBound(ValueDelta);
}

void SKnobWidget::OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
{
	SLeafWidget::OnMouseCaptureLost(CaptureLostEvent);
	bIsDragging = false;
}

int32 SKnobWidget::OnPaint(const FPaintArgs&        Args,
                           const FGeometry&         AllottedGeometry,
                           const FSlateRect&        MyCullingRect,
                           FSlateWindowElementList& OutDrawElements,
                           int32                    LayerId,
                           const FWidgetStyle&      InWidgetStyle,
                           bool                     bParentEnabled) const
{
    int32        Result = LayerId;
    SKnobWidget* MutableThis = const_cast<SKnobWidget*>(this);

    FVector2D Size = AllottedGeometry.GetLocalSize();

	if (Image.IsValid())
	{
		float     Side;
		FVector2D Pos = FVector2D::ZeroVector;

		if (Size.X > Size.Y)
		{
			Side = Size.Y;
			Pos += { .5f * (Size.X - Size.Y), 0.f };
		}
		else
		{
			Side = Size.X;
			Pos += { 0.f, .5f * (Size.Y - Size.X) };
		}
		
		FGeometry Geometry = AllottedGeometry.MakeChild({ Side, Side }, FSlateLayoutTransform(Pos));

		Image->Paint(Args,
		             Geometry,
		             MyCullingRect,
		             OutDrawElements,
		             LayerId,
		             InWidgetStyle,
		             bParentEnabled);
	}

	MutableThis->LastSize = Size;
    return Result;
}

FVector2D SKnobWidget::ComputeDesiredSize(float) const
{
	return FVector2D::ZeroVector;
}

bool SKnobWidget::IsInsideRing(const FVector2D& InMousePos) const
{
	FVector2D Center = LastSize / 2.f;
	float DistanceToMouse = (InMousePos - Center).Size();
	float OuterRadius = FMath::Min(Center.X, Center.Y);
	float InnerRadius = OuterRadius * (1.f - ThicknessAttribute.Get());

	return DistanceToMouse >= InnerRadius && DistanceToMouse <= OuterRadius;
}
