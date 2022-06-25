// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleNavelWidget.h"

#include "SToggleNavelWidget.h"
#include "Components/RetainerBox.h"
#include "Components/ScaleBox.h"
#include "Components/Widget.h"
#include "Widgets/Images/SImage.h"


#define LOCTEXT_NAMESPACE "Sonafridge"

UToggleNavelWidget::UToggleNavelWidget(const FObjectInitializer& ObjectInitializer)
	: UWidget(ObjectInitializer)
{
	Material = LoadObject<UMaterialInterface>
	(
		nullptr,
		TEXT("/Sonafridge/UI/Materials/M_Navel.M_Navel")
	);
}

TSharedRef<SWidget> UToggleNavelWidget::RebuildWidget()
{
	NavelWidget = SNew(SToggleNavelWidget)
		.MouseFastSpeed_Lambda([this] { return FastResponsiveness; })
		.MouseFineSpeed_Lambda([this] { return FineResponsiveness; })
		.IsOn_Lambda([this] { return IsOn; })
		.Blurriness_Lambda([this] { return Blurriness; })
		.BackColor_Lambda([this] { return BackColor; })
		.ForeColor_Lambda([this] { return ForeColor; })
		.AlphaMask_Lambda([this] { return AlphaMask; })
		.ToggleStateRequested_UObject(this, &UToggleNavelWidget::OnToggleStateRequested)
		.MouseCaptureStarted_UObject(this, &UToggleNavelWidget::OnMouseCaptureStarted)
		.MouseCaptureFinished_UObject(this, &UToggleNavelWidget::OnMouseCaptureFinished)
		.ValueDeltaRequested_UObject(this, &UToggleNavelWidget::OnValueDeltaRequested);

	if (Material)
	{
		BrushMID = Cast<UMaterialInstanceDynamic>(Material);
		if (!BrushMID)
		{
			BrushMID = UMaterialInstanceDynamic::Create(Material, GetTransientPackage());
		}

		Brush.SetImageSize({ 128.f, 128.f });
		Brush.SetResourceObject(BrushMID);
		NavelWidget->SetBrush(Brush);
	}

	return NavelWidget.ToSharedRef();
}

void UToggleNavelWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	NavelWidget.Reset();
}

void UToggleNavelWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	ensureMsgf(NavelWidget, TEXT("UToggleNavelWidget::SynchronizeProperties. "
	                             "RebuildWidget should already be called. "));
	NavelWidget->UpdateMaterial();
}

const FText UToggleNavelWidget::GetPaletteCategory()
{
	return LOCTEXT("Sonafridge", "Sonafridge");
}

void UToggleNavelWidget::OnToggleStateRequested()
{
	IsOn = !IsOn;
	ToggleStateChanged.Broadcast(!IsOn, IsOn);
}

void UToggleNavelWidget::OnMouseCaptureStarted()
{
	MouseCaptureStarted.Broadcast();
}

void UToggleNavelWidget::OnMouseCaptureFinished()
{
	MouseCaptureFinished.Broadcast();
}

void UToggleNavelWidget::OnValueDeltaRequested(float ValueDelta)
{
	ValueChanged.Broadcast(ValueDelta);
}

#undef LOCTEXT_NAMESPACE


