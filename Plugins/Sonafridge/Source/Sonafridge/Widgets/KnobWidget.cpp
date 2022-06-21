// Fill out your copyright notice in the Description page of Project Settings.


#include "KnobWidget.h"

#include "SKnobWidget.h"
#include "Components/RetainerBox.h"
#include "Components/ScaleBox.h"
#include "Components/Widget.h"
#include "Widgets/Images/SImage.h"


UKnobWidget::UKnobWidget(const FObjectInitializer& ObjectInitializer)
	: UWidget(ObjectInitializer)
{
	Material = LoadObject<UMaterialInterface>
	(
		nullptr,
		TEXT("/Sonafridge/UI/Materials/M_Knob.M_Knob")
	);
}

TSharedRef<SWidget> UKnobWidget::RebuildWidget()
{
	KnobWidget = SNew(SKnobWidget)
		.MouseFastSpeed_Lambda([this] { return FastResponsiveness; })
		.MouseFineSpeed_Lambda([this] { return FineResponsiveness; })
		.Value_Lambda([this] { return Value; })
		.Thickness_Lambda([this] { return NormalThickness; })
		.Blurriness_Lambda([this] { return Blurriness; })
		.BackColor_Lambda([this] { return BackColor; })
		.ForeColor_Lambda([this] { return ForeColor; })
		.ValueDeltaRequested_UObject(this, &UKnobWidget::OnValueDeltaRequested);

	if (Material)
	{
		BrushMID = Cast<UMaterialInstanceDynamic>(Material);
		if (!BrushMID)
		{
			BrushMID = UMaterialInstanceDynamic::Create(Material, GetTransientPackage());
		}

		Brush.SetImageSize({ 128.f, 128.f });
		Brush.SetResourceObject(BrushMID);
		KnobWidget->SetBrush(Brush);
	}

	return KnobWidget.ToSharedRef();
}

void UKnobWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	KnobWidget.Reset();
}

void UKnobWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	ensureMsgf(KnobWidget, TEXT("UKnobWidget::SynchronizeProperties. "
	                            "RebuildWidget must already be called. "));
	KnobWidget->UpdateMaterial();
}

void UKnobWidget::OnValueDeltaRequested(float ValueDelta)
{
	float OldValue = Value;
	Value += ValueDelta;
	Value = FMath::Clamp(Value, 0.f, 1.f);

	if (KnobWidget)
	{
		KnobWidget->UpdateMaterial();
	}

	ValueChanged.Broadcast(OldValue, Value);
}


