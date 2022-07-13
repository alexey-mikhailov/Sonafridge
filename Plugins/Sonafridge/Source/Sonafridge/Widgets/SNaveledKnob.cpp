// Sonafridge 2022

#include "SNaveledKnob.h"

#include "SlateMaterialBrush.h"
#include "Widgets/Images/SImage.h"

void SNaveledKnob::Construct(const FArguments& InArgs)
{
	IsLockedAttribute = InArgs._Locked;
	WheelStepAttribute = InArgs._WheelStep;
	MouseFastSpeedAttribute = InArgs._MouseFastSpeed;
	MouseFineSpeedAttribute = InArgs._MouseFineSpeed;
	ValueAttribute = InArgs._Value;
	KnobNavelThreshold = InArgs._KnobNavelThreshold;
	BlurrinessAttribute = InArgs._Blurriness;
	Knob0ColorAttribute = InArgs._Knob0Color;
	Knob1ColorAttribute = InArgs._Knob1Color;
	NavelColorAttribute = InArgs._NavelColor;
	IdleIconAttribute = InArgs._IdleIcon;
	HoverIconAttribute = InArgs._HoverIcon;
	IconScaleAttribute = InArgs._IconScale;

	KnobEntrance = InArgs._KnobEntrance;
	NavelEntrance = InArgs._NavelEntrance;
	KnobExit = InArgs._KnobExit;
	NavelExit = InArgs._NavelExit;
	KnobCaptureStarted = InArgs._KnobCaptureStarted;
	KnobCaptureFinished = InArgs._KnobCaptureFinished;
	NavelCaptureStarted = InArgs._NavelCaptureStarted;
	NavelCaptureFinished = InArgs._NavelCaptureFinished;
	KnobDeltaRequested = InArgs._KnobDeltaRequested;
	NavelDeltaRequested = InArgs._NavelDeltaRequested;
	NavelClick = InArgs._NavelClick;

	Image = SNew(SImage);
}

void SNaveledKnob::SetBrush(const FSlateBrush& InBrush)
{
	Brush = InBrush;
	Image->SetImage(&Brush);

	UObject* BrushResource = Brush.GetResourceObject();
	UMaterialInstanceDynamic* BrushMID = Cast<UMaterialInstanceDynamic>(BrushResource);

	if (IsValid(BrushMID))
	{
		UTexture* Icon = bWasInsideNavel ? HoverIconAttribute.Get() : IdleIconAttribute.Get();
		BrushMID->SetTextureParameterValue("Icon", Icon);
		BrushMID->SetScalarParameterValue("Icon Scale", IconScaleAttribute.Get());
	}
}

void SNaveledKnob::UpdateMaterial()
{
	UMaterialInstanceDynamic* BrushMID = GetMaterial();

	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Value", ValueAttribute.Get());
		BrushMID->SetScalarParameterValue("Knob Navel Threshold", KnobNavelThreshold.Get());
		BrushMID->SetScalarParameterValue("Blurriness", BlurrinessAttribute.Get());
		BrushMID->SetVectorParameterValue("Knob 0 Color", Knob0ColorAttribute.Get());
		BrushMID->SetVectorParameterValue("Knob 1 Color", Knob1ColorAttribute.Get());
		BrushMID->SetVectorParameterValue("Navel Color", NavelColorAttribute.Get());
		UTexture* Icon = bWasInsideNavel ? HoverIconAttribute.Get() : IdleIconAttribute.Get();
		BrushMID->SetTextureParameterValue("Icon", Icon);
		BrushMID->SetScalarParameterValue("Icon Scale", IconScaleAttribute.Get());
	}
}

void SNaveledKnob::OnKnobEntrance()
{
	UMaterialInstanceDynamic* BrushMID = GetMaterial();
	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is Over Knob", true);
		BrushMID->SetScalarParameterValue("Is Over Navel", false);
		BrushMID->SetTextureParameterValue("Icon", IdleIconAttribute.Get());
	}

	bWasInsideKnob = true;
	bWasInsideNavel = false;
	KnobEntrance.ExecuteIfBound();
}

void SNaveledKnob::OnNavelEntrance()
{
	UMaterialInstanceDynamic* BrushMID = GetMaterial();
	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is Over Knob", false);
		BrushMID->SetScalarParameterValue("Is Over Navel", true);
		BrushMID->SetTextureParameterValue("Icon", HoverIconAttribute.Get());
	}

	bWasInsideNavel = true;
	bWasInsideKnob = false;
	NavelEntrance.ExecuteIfBound();
}

void SNaveledKnob::OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	SLeafWidget::OnMouseEnter(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (!bIsDraggingKnob && 
		!bIsDraggingNavel && 
		IsInsideKnob(MousePos))
	{
		OnKnobEntrance();
	}
	else if (!bIsDraggingKnob && 
			 !bIsDraggingNavel && 
			 IsInsideNavel(MousePos))
	{
		OnNavelEntrance();
	}
}

void SNaveledKnob::OnKnobExit()
{
	UMaterialInstanceDynamic* BrushMID = GetMaterial();
	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is Over Knob", false);
	}

	bWasInsideKnob = false;
	KnobExit.ExecuteIfBound();
}

void SNaveledKnob::OnNavelExit()
{
	UMaterialInstanceDynamic* BrushMID = GetMaterial();
	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is Over Navel", false);
		BrushMID->SetTextureParameterValue("Icon", IdleIconAttribute.Get());
	}

	bWasInsideNavel = false;
	NavelExit.ExecuteIfBound();
}

void SNaveledKnob::OnMouseLeave(const FPointerEvent& InMouseEvent)
{
	SLeafWidget::OnMouseLeave(InMouseEvent);

	if (!bIsDraggingKnob)
	{
		OnKnobExit();
	}

	if (!bIsDraggingNavel)
	{
		OnNavelExit();
	}
}

FReply SNaveledKnob::OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

FReply SNaveledKnob::OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

FReply SNaveledKnob::OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

	constexpr float ClickTolerance = 1.f;
	if ((MousePos - PresstimeMousePos).Size() < ClickTolerance && IsInsideNavel(MousePos))
	{
		NavelClick.ExecuteIfBound();
	}

	UMaterialInstanceDynamic* BrushMID = GetMaterial();
	if (IsValid(BrushMID))
	{
		BrushMID->SetScalarParameterValue("Is Knob Taken", false);
		BrushMID->SetScalarParameterValue("Is Navel Taken", false);
	}

	return Reply;
}

FReply SNaveledKnob::OnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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
			OnKnobEntrance();
		}
		else if (bWasInsideKnob && !bIsInsideKnob)
		{
			OnKnobExit();
		}

		if (!bWasInsideNavel && bIsInsideNavel)
		{
			OnNavelEntrance();
		}
		else if (bWasInsideNavel && !bIsInsideNavel)
		{
			OnNavelExit();
		}
	}

	LastMousePos = MousePos;
	return Reply;
}

void SNaveledKnob::OnKnobDrag(FReply&              InOutReply,
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

void SNaveledKnob::OnNavelDrag(FReply&              InOutReply,
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

void SNaveledKnob::OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
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

int32 SNaveledKnob::OnPaint(const FPaintArgs&        Args,
                            const FGeometry&         AllottedGeometry,
                            const FSlateRect&        MyCullingRect,
                            FSlateWindowElementList& OutDrawElements,
                            int32                    LayerId,
                            const FWidgetStyle&      InWidgetStyle,
                            bool                     bParentEnabled) const
{
    int32 Result = LayerId;
    SNaveledKnob* MutableThis = const_cast<SNaveledKnob*>(this);

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

FVector2D SNaveledKnob::ComputeDesiredSize(float) const
{
	return FVector2D::ZeroVector;
}

bool SNaveledKnob::IsInsideKnob(const FVector2D& InMousePos) const
{
	FVector2D Center = LastSize / 2.f;
	float DistanceToMouse = (InMousePos - Center).Size();
	float OuterRadius = FMath::Min(Center.X, Center.Y);
	float InnerRadius = OuterRadius * KnobNavelThreshold.Get();

	return DistanceToMouse >= InnerRadius && DistanceToMouse <= OuterRadius;
}

bool SNaveledKnob::IsInsideNavel(const FVector2D& InMousePos) const
{
	FVector2D Center = LastSize / 2.f;
	float DistanceToMouse = (InMousePos - Center).Size();
	float OuterRadius = FMath::Min(Center.X, Center.Y);
	float InnerRadius = OuterRadius * KnobNavelThreshold.Get();

	return DistanceToMouse < InnerRadius;
}
