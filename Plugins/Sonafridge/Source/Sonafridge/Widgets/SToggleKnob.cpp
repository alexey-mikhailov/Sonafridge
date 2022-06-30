// Fill out your copyright notice in the Description page of Project Settings.


#include "SToggleKnob.h"

#include "SlateMaterialBrush.h"
#include "Widgets/Images/SImage.h"

void SToggleKnob::Construct(const FArguments& InArgs)
{
	IsLockedAttribute = InArgs._Locked;
	WheelStepAttribute = InArgs._WheelStep;
	MouseFastSpeedAttribute = InArgs._MouseFastSpeed;
	MouseFineSpeedAttribute = InArgs._MouseFineSpeed;
	IsOnAttribute = InArgs._IsOn;
	ValueAttribute = InArgs._Value;
	KnobNavelThreshold = InArgs._KnobNavelThreshold;
	BlurrinessAttribute = InArgs._Blurriness;
	Knob0ColorAttribute = InArgs._Knob0Color;
	Knob1ColorAttribute = InArgs._Knob1Color;
	Navel0ColorAttribute = InArgs._Navel0Color;
	Navel1ColorAttribute = InArgs._Navel1Color;
	IconAttribute = InArgs._Icon;
	IconScaleAttribute = InArgs._IconScale;

	KnobCaptureStarted = InArgs._KnobCaptureStarted;
	KnobCaptureFinished = InArgs._KnobCaptureFinished;
	NavelCaptureStarted = InArgs._NavelCaptureStarted;
	NavelCaptureFinished = InArgs._NavelCaptureFinished;
	KnobDeltaRequested = InArgs._KnobDeltaRequested;
	NavelDeltaRequested = InArgs._NavelDeltaRequested;
	ToggleStateRequested = InArgs._ToggleStateRequested;

	Image = SNew(SImage);
}

void SToggleKnob::SetBrush(const FSlateBrush& InBrush)
{
	Brush = InBrush;
	Image->SetImage(&Brush);

	UObject* BrushResource = Brush.GetResourceObject();
	UMaterialInstanceDynamic* BrushMID = Cast<UMaterialInstanceDynamic>(BrushResource);

	if (IsValid(BrushMID))
	{
		BrushMID->SetTextureParameterValue("Icon", IconAttribute.Get());
		BrushMID->SetScalarParameterValue("Icon Scale", IconScaleAttribute.Get());
	}
}

void SToggleKnob::UpdateMaterial()
{
	UMaterialInstanceDynamic* BrushMID = GetMaterial();

	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is On", IsOnAttribute.Get());
		BrushMID->SetScalarParameterValue("Value", ValueAttribute.Get());
		BrushMID->SetScalarParameterValue("Knob Navel Threshold", KnobNavelThreshold.Get());
		BrushMID->SetScalarParameterValue("Blurriness", BlurrinessAttribute.Get());
		BrushMID->SetVectorParameterValue("Knob 0 Color", Knob0ColorAttribute.Get());
		BrushMID->SetVectorParameterValue("Knob 1 Color", Knob1ColorAttribute.Get());
		BrushMID->SetVectorParameterValue("Navel 0 Color", Navel0ColorAttribute.Get());
		BrushMID->SetVectorParameterValue("Navel 1 Color", Navel1ColorAttribute.Get());
		BrushMID->SetTextureParameterValue("Icon", IconAttribute.Get());
		BrushMID->SetScalarParameterValue("Icon Scale", IconScaleAttribute.Get());
	}
}

void SToggleKnob::OnKnobEnter()
{
	if (!bIsDraggingKnob)
	{
		UMaterialInstanceDynamic* BrushMID = GetMaterial();
		if (IsValid(BrushMID))
		{
			BrushMID->SetScalarParameterValue("Is Over Knob", true);
		}
	}
}

void SToggleKnob::OnNavelEnter()
{
	if (!bIsDraggingKnob)
	{
		UMaterialInstanceDynamic* BrushMID = GetMaterial();
		if (IsValid(BrushMID))
		{
			BrushMID->SetScalarParameterValue("Is Over Navel", true);
		}
	}
}

void SToggleKnob::OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SLeafWidget::OnMouseEnter(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (IsInsideKnob(MousePos))
	{
		OnKnobEnter();
	}
	else if (IsInsideNavel(MousePos))
	{
		OnNavelEnter();
	}
}

void SToggleKnob::OnKnobLeave()
{
	if (!bIsDraggingKnob)
	{
		UMaterialInstanceDynamic* BrushMID = GetMaterial();
		if (IsValid(BrushMID))
		{
			BrushMID->SetScalarParameterValue("Is Over Knob", false);
		}
	}
}

void SToggleKnob::OnNavelLeave()
{
	if (!bIsDraggingNavel)
	{
		UMaterialInstanceDynamic* BrushMID = GetMaterial();
		if (IsValid(BrushMID))
		{
			BrushMID->SetScalarParameterValue("Is Over Navel", false);
		}
	}
}

void SToggleKnob::OnMouseLeave(const FPointerEvent& InMouseEvent)
{
	SLeafWidget::OnMouseLeave(InMouseEvent);
	OnKnobLeave();
	OnNavelLeave();
}

FReply SToggleKnob::OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = SLeafWidget::OnMouseWheel(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (IsInsideKnob(MousePos))
	{
		KnobDeltaRequested.ExecuteIfBound(WheelStepAttribute.Get() * InMouseEvent.GetWheelDelta());
	}
	else if (IsInsideNavel(MousePos))
	{
		NavelDeltaRequested.ExecuteIfBound(WheelStepAttribute.Get() * InMouseEvent.GetWheelDelta());
	}

	return Reply;
}

FReply SToggleKnob::OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = SLeafWidget::OnMouseButtonDown(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (IsInsideKnob(MousePos))
	{
		Reply = Reply.CaptureMouse(AsShared());
		KnobCaptureStarted.ExecuteIfBound();
		PresstimeMousePos = MousePos;
		bIsDraggingKnob = true;

		UMaterialInstanceDynamic* BrushMID = GetMaterial();
		if (IsValid(BrushMID))
		{
			BrushMID->SetScalarParameterValue("Is Knob Taken", true);
		}
	}
	else if (IsInsideNavel(MousePos))
	{
		Reply = Reply.CaptureMouse(AsShared());
		NavelCaptureStarted.ExecuteIfBound();
		PresstimeMousePos = MousePos;
		bIsDraggingNavel = true;

		UMaterialInstanceDynamic* BrushMID = GetMaterial();
		if (IsValid(BrushMID))
		{
			BrushMID->SetScalarParameterValue("Is Navel Taken", true);
		}
	}

	return Reply;
}

FReply SToggleKnob::OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = SLeafWidget::OnMouseButtonUp(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (bIsDraggingKnob)
	{
		Reply = Reply.ReleaseMouseCapture();
		KnobCaptureFinished.ExecuteIfBound();
		bIsDraggingKnob = false;
	}
	else if (bIsDraggingNavel)
	{
		Reply = Reply.ReleaseMouseCapture();
		NavelCaptureFinished.ExecuteIfBound();
		bIsDraggingNavel = false;
	}

	if (MousePos == PresstimeMousePos)
	{
		ToggleStateRequested.ExecuteIfBound();
	}

	UMaterialInstanceDynamic* BrushMID = GetMaterial();
	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is On", IsOnAttribute.Get());
		BrushMID->SetScalarParameterValue("Is Knob Taken", false);
		BrushMID->SetScalarParameterValue("Is Navel Taken", false);
	}

	return Reply;
}

FReply SToggleKnob::OnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply    Reply = SLeafWidget::OnMouseMove(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	bool      bIsInsideKnob = IsInsideKnob(MousePos);
	bool      bIsInsideNavel = IsInsideNavel(MousePos);

	if (bIsDraggingKnob)
	{
		OnKnobDrag(Reply, InGeometry, InMouseEvent, MousePos);
	}
	else if (bIsDraggingNavel)
	{
		OnNavelDrag(Reply, InGeometry, InMouseEvent, MousePos);
	}
	else
	{
		if (!bWasInsideKnob && bIsInsideKnob)
		{
			OnKnobEnter();
		}
		else if (bWasInsideKnob && !bIsInsideKnob)
		{
			OnKnobLeave();
		}

		if (!bWasInsideNavel && bIsInsideNavel)
		{
			OnNavelEnter();
		}
		else if (bWasInsideNavel && !bIsInsideNavel)
		{
			OnNavelLeave();
		}
	}

	LastMousePos = MousePos;
	bWasInsideKnob = bIsInsideKnob;
	bWasInsideNavel = bIsInsideNavel;
	return Reply;
}

void SToggleKnob::OnKnobDrag(FReply&              InOutReply,
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

	KnobDeltaRequested.ExecuteIfBound(ValueDelta);
}

void SToggleKnob::OnNavelDrag(FReply&              InOutReply,
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

	NavelDeltaRequested.ExecuteIfBound(ValueDelta);
}

void SToggleKnob::OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
{
	SLeafWidget::OnMouseCaptureLost(CaptureLostEvent);

	if (bIsDraggingKnob)
	{
		KnobCaptureFinished.ExecuteIfBound();
	}

	if (bIsDraggingNavel)
	{
		NavelCaptureFinished.ExecuteIfBound();
	}

	bIsDraggingKnob = false;
	bIsDraggingNavel = false;

	UMaterialInstanceDynamic* BrushMID = GetMaterial();
	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is Knob Taken", false);
		BrushMID->SetScalarParameterValue("Is Navel Taken", false);
	}
}

int32 SToggleKnob::OnPaint(const FPaintArgs&        Args,
                           const FGeometry&         AllottedGeometry,
                           const FSlateRect&        MyCullingRect,
                           FSlateWindowElementList& OutDrawElements,
                           int32                    LayerId,
                           const FWidgetStyle&      InWidgetStyle,
                           bool                     bParentEnabled) const
{
    int32 Result = LayerId;
    SToggleKnob* MutableThis = const_cast<SToggleKnob*>(this);

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

FVector2D SToggleKnob::ComputeDesiredSize(float) const
{
	return FVector2D::ZeroVector;
}

bool SToggleKnob::IsInsideKnob(const FVector2D& InMousePos) const
{
	FVector2D Center = LastSize / 2.f;
	float DistanceToMouse = (InMousePos - Center).Size();
	float OuterRadius = FMath::Min(Center.X, Center.Y);
	float InnerRadius = OuterRadius * KnobNavelThreshold.Get();

	return DistanceToMouse >= InnerRadius && DistanceToMouse <= OuterRadius;
}

bool SToggleKnob::IsInsideNavel(const FVector2D& InMousePos) const
{
	FVector2D Center = LastSize / 2.f;
	float DistanceToMouse = (InMousePos - Center).Size();
	float OuterRadius = FMath::Min(Center.X, Center.Y);
	float InnerRadius = OuterRadius * KnobNavelThreshold.Get();

	return DistanceToMouse < InnerRadius;
}