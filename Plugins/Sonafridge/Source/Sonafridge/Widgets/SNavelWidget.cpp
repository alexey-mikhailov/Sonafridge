// Fill out your copyright notice in the Description page of Project Settings.


#include "SNavelWidget.h"

#include "SlateMaterialBrush.h"
#include "Widgets/Images/SImage.h"

void SNavelWidget::Construct(const FArguments& InArgs)
{
	IsLockedAttribute = InArgs._Locked;
	WheelStepAttribute = InArgs._WheelStep;
	MouseFastSpeedAttribute = InArgs._MouseFastSpeed;
	MouseFineSpeedAttribute = InArgs._MouseFineSpeed;
	IsOnAttribute = InArgs._IsOn;
	BlurrinessAttribute = InArgs._Blurriness;
	BackColorAttribute = InArgs._BackColor;
	ForeColorAttribute = InArgs._ForeColor;
	AlphaMaskAttribute = InArgs._AlphaMask;

	MouseEntered = InArgs._MouseEntered;
	MouseLeaved = InArgs._MouseLeaved;
	MouseCaptureStarted = InArgs._MouseCaptureStarted;
	MouseCaptureFinished = InArgs._MouseCaptureFinished;
	ValueDeltaRequested = InArgs._ValueDeltaRequested;
	ToggleStateRequested = InArgs._ToggleStateRequested;

	Image = SNew(SImage);
}

void SNavelWidget::SetBrush(const FSlateBrush& InBrush)
{
	Brush = InBrush;
	Image->SetImage(&Brush);

	UObject* BrushResource = Brush.GetResourceObject();
	UMaterialInstanceDynamic* BrushMID = Cast<UMaterialInstanceDynamic>(BrushResource);

	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is On", IsOnAttribute.Get());
		BrushMID->SetScalarParameterValue("Blurriness", BlurrinessAttribute.Get());
		BrushMID->SetVectorParameterValue("Back Color", BackColorAttribute.Get());
		BrushMID->SetVectorParameterValue("Fore Color", ForeColorAttribute.Get());
		BrushMID->SetTextureParameterValue("Alpha Mask", AlphaMaskAttribute.Get());
	}
}

void SNavelWidget::UpdateMaterial()
{
	UMaterialInstanceDynamic* BrushMID = GetMaterial();

	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is On", IsOnAttribute.Get());
		BrushMID->SetScalarParameterValue("Blurriness", BlurrinessAttribute.Get());
		BrushMID->SetVectorParameterValue("Back Color", BackColorAttribute.Get());
		BrushMID->SetVectorParameterValue("Fore Color", ForeColorAttribute.Get());
		BrushMID->SetTextureParameterValue("Alpha Mask", AlphaMaskAttribute.Get());
	}
}

void SNavelWidget::OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SLeafWidget::OnMouseEnter(InGeometry, InMouseEvent);

	if (!bIsDragging)
	{
		UMaterialInstanceDynamic* BrushMID = GetMaterial();
		if (IsValid(BrushMID))
		{
			BrushMID->SetScalarParameterValue("Is Hover", true);
		}
	}

	MouseEntered.ExecuteIfBound();
}

void SNavelWidget::OnMouseLeave(const FPointerEvent& InMouseEvent)
{
	SLeafWidget::OnMouseLeave(InMouseEvent);

	if (!bIsDragging)
	{
		UMaterialInstanceDynamic* BrushMID = GetMaterial();
		if (IsValid(BrushMID))
		{
			BrushMID->SetScalarParameterValue("Is Hover", false);
		}
	}

	MouseLeaved.ExecuteIfBound();
}

FReply SNavelWidget::OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = SLeafWidget::OnMouseWheel(InGeometry, InMouseEvent);
	
	ValueDeltaRequested.ExecuteIfBound(WheelStepAttribute.Get() * InMouseEvent.GetWheelDelta());

	return Reply;
}

FReply SNavelWidget::OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = SLeafWidget::OnMouseButtonDown(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (IsInsideCircle(MousePos))
	{
		Reply = Reply.CaptureMouse(AsShared());
		MouseCaptureStarted.ExecuteIfBound();
		PresstimeMousePos = MousePos;
		bIsDragging = true;
	}

	UMaterialInstanceDynamic* BrushMID = GetMaterial();
	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is Pressed", true);
	}

	return Reply;
}

FReply SNavelWidget::OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = SLeafWidget::OnMouseButtonUp(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (bIsDragging)
	{
		Reply = Reply.ReleaseMouseCapture();
		MouseCaptureFinished.ExecuteIfBound();
		bIsDragging = false;
	}

	if (MousePos == PresstimeMousePos)
	{
		ToggleStateRequested.ExecuteIfBound();
	}

	UMaterialInstanceDynamic* BrushMID = GetMaterial();
	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is Pressed", false);
		BrushMID->SetScalarParameterValue("Is On", IsOnAttribute.Get());
	}
	
	return Reply;
}

FReply SNavelWidget::OnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

void SNavelWidget::OnMouseDrag(FReply&              InOutReply,
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

void SNavelWidget::OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
{
	SLeafWidget::OnMouseCaptureLost(CaptureLostEvent);
	bIsDragging = false;
}

int32 SNavelWidget::OnPaint(const FPaintArgs&        Args,
                            const FGeometry&         AllottedGeometry,
                            const FSlateRect&        MyCullingRect,
                            FSlateWindowElementList& OutDrawElements,
                            int32                    LayerId,
                            const FWidgetStyle&      InWidgetStyle,
                            bool                     bParentEnabled) const
{
    int32        Result = LayerId;
    SNavelWidget* MutableThis = const_cast<SNavelWidget*>(this);

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

FVector2D SNavelWidget::ComputeDesiredSize(float) const
{
	return FVector2D::ZeroVector;
}

bool SNavelWidget::IsInsideCircle(const FVector2D& InMousePos) const
{
	FVector2D Center = LastSize / 2.f;
	float DistanceToMouse = (InMousePos - Center).Size();
	float OuterRadius = FMath::Min(Center.X, Center.Y);

	return DistanceToMouse <= OuterRadius;
}
