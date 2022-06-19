// Fill out your copyright notice in the Description page of Project Settings.


#include "KnobWidget.h"

#include "Components/RetainerBox.h"
#include "Components/ScaleBox.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Slate/SRetainerWidget.h"
#include "Widgets/Images/SImage.h"

UKnobWidget::UKnobWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	SizeChanged.AddUObject(this, &UKnobWidget::OnSizeChanged);
}

void UKnobWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!RetainerWidget)
	{
		RetainerWidget = SNew(SRetainerWidget).RenderOnInvalidation(true)
		                                      .RenderOnPhase(true)
		                                      .Phase(0)
		                                      .PhaseCount(1)
		                 [
			                 SNew(SImage)
		                 ];
	}

	if (EffectMaterial)
	{
		MaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, EffectMaterial);
		RetainerWidget->SetEffectMaterial(MaterialInstance);
	}

	if (IsValid(MaterialInstance))
	{
		MaterialInstance->SetScalarParameterValue("Value", Value);
		MaterialInstance->SetScalarParameterValue("Thickness", NormalizedThickness);
		MaterialInstance->SetScalarParameterValue("Blurriness", Blurriness);
		MaterialInstance->SetVectorParameterValue("BackColor", BackColor);
		MaterialInstance->SetVectorParameterValue("ForeColor", ForeColor);
	}
}

void UKnobWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	RetainerWidget.Reset();
	MaterialInstance = nullptr;
}

void UKnobWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!RetainerWidget)
	{
		RetainerWidget = SNew(SRetainerWidget).RenderOnInvalidation(true)
		                                      .RenderOnPhase(true)
		                                      .Phase(0)
		                                      .PhaseCount(1)
		                 [
			                 SNew(SImage)
		                 ];
	}

	MaterialInstance = RetainerWidget->GetEffectMaterial();
	if (!IsValid(MaterialInstance) && EffectMaterial)
	{
		MaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, EffectMaterial);
		RetainerWidget->SetEffectMaterial(MaterialInstance);
	}

	if (IsValid(MaterialInstance))
	{
		MaterialInstance->SetScalarParameterValue("Value", Value);
		MaterialInstance->SetScalarParameterValue("Thickness", NormalizedThickness);
		MaterialInstance->SetScalarParameterValue("Blurriness", Blurriness);
		MaterialInstance->SetVectorParameterValue("BackColor", BackColor);
		MaterialInstance->SetVectorParameterValue("ForeColor", ForeColor);
	}
}

void UKnobWidget::OnSizeChanged(const FVector2D& OldSize, 
                                const FVector2D& NewSize)
{
	LastSize = NewSize;
}

void UKnobWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UKnobWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UKnobWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (IsInsideRing(MousePos))
	{
		bIsDragging = true;
		PresstimeMousePos = MousePos;
		Reply = Reply.CaptureMouse(TakeWidget());
	}

	return Reply;
}

FReply UKnobWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (bIsDragging)
	{
		Reply = Reply.ReleaseMouseCapture();
	}

	bIsDragging = false;
	return Reply;
}

FReply UKnobWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	if (bIsDragging)
	{
		OnMouseDrag(Reply, InGeometry, InMouseEvent, MousePos);
	}

	LastMousePos = MousePos;
	return Reply;
}

void UKnobWidget::NativeOnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
{
	Super::NativeOnMouseCaptureLost(CaptureLostEvent);
	bIsDragging = false;
}

void UKnobWidget::OnMouseDrag(FReply&              InOutReply,
                              const FGeometry&     InGeometry,
                              const FPointerEvent& InMouseEvent,
                              const FVector2D&     InMousePos)
{
	float OldValue = Value;
	FVector2D MouseDelta = InMousePos - LastMousePos;
	Value -= MouseDelta.Y * Responsiveness;
	Value = FMath::Clamp(Value, 0.f, 1.f);

	if (IsValid(MaterialInstance))
	{
		MaterialInstance->SetScalarParameterValue("Value", Value);
	}

	ValueChanged.Broadcast(OldValue, Value);
}

FReply UKnobWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

int32 UKnobWidget::NativePaint(const FPaintArgs&        Args,
                               const FGeometry&         AllottedGeometry,
                               const FSlateRect&        MyCullingRect,
                               FSlateWindowElementList& OutDrawElements,
                               int32                    LayerId,
                               const FWidgetStyle&      InWidgetStyle,
                               bool                     bParentEnabled) const
{
	int32 Result = Super::NativePaint(Args,
	                                  AllottedGeometry,
	                                  MyCullingRect,
	                                  OutDrawElements,
	                                  LayerId,
	                                  InWidgetStyle,
	                                  bParentEnabled);

	FVector2D Size = AllottedGeometry.GetLocalSize();

	if (Size != LastSize)
	{
		SizeChanged.Broadcast(LastSize, Size);
	}

	if (RetainerWidget.IsValid())
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

		RetainerWidget->Paint(Args,
		                      Geometry,
		                      MyCullingRect,
		                      OutDrawElements,
		                      LayerId,
		                      InWidgetStyle,
		                      bParentEnabled);
	}

	return Result;
}

bool UKnobWidget::IsInsideRing(const FVector2D& InMousePos) const
{
	FVector2D Center = LastSize / 2.f;
	float DistanceToMouse = (InMousePos - Center).Size();
	float OuterRadius = FMath::Min(Center.X, Center.Y);
	float InnerRadius = OuterRadius * (1.f - NormalizedThickness);

	return DistanceToMouse >= InnerRadius && DistanceToMouse <= OuterRadius;
}
