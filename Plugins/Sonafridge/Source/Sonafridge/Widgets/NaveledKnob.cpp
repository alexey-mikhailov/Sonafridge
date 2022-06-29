// Fill out your copyright notice in the Description page of Project Settings.


#include "NaveledKnob.h"

#include "SNaveledKnob.h"
#include "Components/RetainerBox.h"
#include "Components/ScaleBox.h"
#include "Components/Widget.h"
#include "Widgets/Images/SImage.h"

#define LOCTEXT_NAMESPACE "Sonafridge"

UNaveledKnob::UNaveledKnob(const FObjectInitializer& ObjectInitializer)
	: UWidget(ObjectInitializer)
{
	Material = LoadObject<UMaterialInterface>
	(
		nullptr,
		TEXT("/Sonafridge/UI/Materials/M_NaveledKnob.M_NaveledKnob")
	);
}

void UNaveledKnob::SetKnobValue01(float InKnobValue01)
{
	KnobValue01 = InKnobValue01;
	SWidget->UpdateMaterial();
}

void UNaveledKnob::SetNavelValue01(float InNavelValue01)
{
	NavelValue01 = InNavelValue01;
	SWidget->UpdateMaterial();
}

void UNaveledKnob::SetKnobNavelThreshold01(float InKnobNavelThreshold01)
{
	KnobNavelThreshold01 = InKnobNavelThreshold01;
	SWidget->UpdateMaterial();
}

void UNaveledKnob::RefreshVisual()
{
	SWidget->UpdateMaterial();
}

TSharedRef<SWidget> UNaveledKnob::RebuildWidget()
{
	SWidget = SNew(SNaveledKnob)
		.MouseFastSpeed_Lambda([this] { return FastResponsiveness; })
		.MouseFineSpeed_Lambda([this] { return FineResponsiveness; })
		.KnobValue_Lambda([this] { return KnobValue01; })
		.NavelValue_Lambda([this] { return NavelValue01; })
		.KnobNavelThreshold_Lambda([this] { return KnobNavelThreshold01; })
		.Blurriness_Lambda([this] { return Blurriness; })
		.Knob0Color_Lambda([this] { return Knob0Color; })
		.Knob1Color_Lambda([this] { return Knob1Color; })
		.Navel0Color_Lambda([this] { return Navel0Color; })
		.Navel1Color_Lambda([this] { return Navel1Color; })
		.Icon_Lambda([this] { return Icon; })
		.IconScale_Lambda([this] { return IconScale; })
		.KnobCaptureStarted_UObject(this, &UNaveledKnob::OnKnobCaptureStarted)
		.KnobCaptureFinished_UObject(this, &UNaveledKnob::OnKnobCaptureFinished)
		.NavelCaptureStarted_UObject(this, &UNaveledKnob::OnNavelCaptureStarted)
		.NavelCaptureFinished_UObject(this, &UNaveledKnob::OnNavelCaptureFinished)
		.KnobDeltaRequested_UObject(this, &UNaveledKnob::OnKnobDeltaRequested)
		.NavelDeltaRequested_UObject(this, &UNaveledKnob::OnNavelDeltaRequested);

	if (Material)
	{
		BrushMID = Cast<UMaterialInstanceDynamic>(Material);
		if (!BrushMID)
		{
			BrushMID = UMaterialInstanceDynamic::Create(Material, GetTransientPackage());
		}

		Brush.SetImageSize({ 128.f, 128.f });
		Brush.SetResourceObject(BrushMID);
		SWidget->SetBrush(Brush);
		SWidget->UpdateMaterial();
	}

	return SWidget.ToSharedRef();
}

void UNaveledKnob::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	SWidget.Reset();
}

void UNaveledKnob::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	ensureMsgf(SWidget, TEXT("UNaveledKnob::SynchronizeProperties. "
	                         "RebuildWidget must already be called. "));
	SWidget->UpdateMaterial();
}

const FText UNaveledKnob::GetPaletteCategory()
{
	return LOCTEXT("Sonafridge", "Sonafridge");
}

void UNaveledKnob::OnKnobCaptureStarted()
{
	KnobCaptureStarted.Broadcast();
}

void UNaveledKnob::OnKnobCaptureFinished()
{
	KnobCaptureFinished.Broadcast();
}

void UNaveledKnob::OnNavelCaptureStarted()
{
	NavelCaptureStarted.Broadcast();
}

void UNaveledKnob::OnNavelCaptureFinished()
{
	NavelCaptureFinished.Broadcast();
}

void UNaveledKnob::OnKnobDeltaRequested(float ValueDelta)
{
	float OldValue01 = KnobValue01;
	KnobValue01 += ValueDelta;
	KnobValue01 = FMath::Clamp(KnobValue01, 0.f, 1.f);

	if (SWidget)
	{
		SWidget->UpdateMaterial();
	}

	KnobValueChanged.Broadcast(OldValue01, KnobValue01);
}

void UNaveledKnob::OnNavelDeltaRequested(float ValueDelta)
{
	float OldValue01 = NavelValue01;
	NavelValue01 += ValueDelta;
	NavelValue01 = FMath::Clamp(NavelValue01, 0.f, 1.f);

	if (SWidget)
	{
		SWidget->UpdateMaterial();
	}

	NavelValueChanged.Broadcast(OldValue01, NavelValue01);
}

#undef LOCTEXT_NAMESPACE
