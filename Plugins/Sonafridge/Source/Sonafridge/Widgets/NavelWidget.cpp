// Fill out your copyright notice in the Description page of Project Settings.


#include "NavelWidget.h"

#include "SNavelWidget.h"
#include "Components/RetainerBox.h"
#include "Components/ScaleBox.h"
#include "Components/Widget.h"
#include "Widgets/Images/SImage.h"


#define LOCTEXT_NAMESPACE "Sonafridge"

UNavelWidget::UNavelWidget(const FObjectInitializer& ObjectInitializer)
	: UWidget(ObjectInitializer)
{
	Material = LoadObject<UMaterialInterface>
	(
		nullptr,
		TEXT("/Sonafridge/UI/Materials/M_Navel.M_Navel")
	);
}

TSharedRef<SWidget> UNavelWidget::RebuildWidget()
{
	NavelWidget = SNew(SNavelWidget)
		.MouseFastSpeed_Lambda([this] { return FastResponsiveness; })
		.MouseFineSpeed_Lambda([this] { return FineResponsiveness; })
		.Blurriness_Lambda([this] { return Blurriness; })
		.BackColor_Lambda([this] { return BackColor; })
		.ForeColor_Lambda([this] { return ForeColor; })
		.AlphaMask_Lambda([this] { return AlphaMask; })
		.MouseCaptureStarted_UObject(this, &UNavelWidget::OnMouseCaptureStarted)
		.MouseCaptureFinished_UObject(this, &UNavelWidget::OnMouseCaptureFinished)
		.ValueDeltaRequested_UObject(this, &UNavelWidget::OnValueDeltaRequested);

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

void UNavelWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	NavelWidget.Reset();
}

void UNavelWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	ensureMsgf(NavelWidget, TEXT("UNavelWidget::SynchronizeProperties. "
	                             "RebuildWidget should already be called. "));
	NavelWidget->UpdateMaterial();
}

const FText UNavelWidget::GetPaletteCategory()
{
	return LOCTEXT("Sonafridge", "Sonafridge");
}

void UNavelWidget::OnMouseCaptureStarted()
{
	MouseCaptureStarted.Broadcast();
}

void UNavelWidget::OnMouseCaptureFinished()
{
	MouseCaptureFinished.Broadcast();
}

void UNavelWidget::OnValueDeltaRequested(float ValueDelta)
{
	ValueChanged.Broadcast(ValueDelta);
}

#undef LOCTEXT_NAMESPACE
