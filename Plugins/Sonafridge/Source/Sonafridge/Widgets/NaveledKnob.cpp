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

void UNaveledKnob::SetValue01(float InKnobValue01)
{
	Value01 = InKnobValue01;
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
		.Value_Lambda([this] { return Value01; })
		.KnobNavelThreshold_Lambda([this] { return KnobNavelThreshold01; })
		.Blurriness_Lambda([this] { return Blurriness; })
		.Knob0Color_Lambda([this] { return Knob0Color; })
		.Knob1Color_Lambda([this] { return Knob1Color; })
		.NavelColor_Lambda([this] { return NavelColor; })
		.IdleIcon_Lambda([this] { return IdleIcon; })
		.HoverIcon_Lambda([this] { return HoverIcon; })
		.IconScale_Lambda([this] { return IconScale; })
		.KnobEntrance_UObject(this, &UNaveledKnob::OnKnobEntrance)
		.NavelEntrance_UObject(this, &UNaveledKnob::OnNavelEntrance)
		.KnobExit_UObject(this, &UNaveledKnob::OnKnobExit)
		.NavelExit_UObject(this, &UNaveledKnob::OnNavelExit)
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
	SWidget->UpdateMaterial();
}

const FText UNaveledKnob::GetPaletteCategory()
{
	return LOCTEXT("Sonafridge", "Sonafridge");
}

void UNaveledKnob::OnKnobEntrance() const
{
	KnobEntrance.Broadcast();
}

void UNaveledKnob::OnNavelEntrance() const
{
	NavelEntrance.Broadcast();
}

void UNaveledKnob::OnKnobExit() const
{
	KnobExit.Broadcast();
}

void UNaveledKnob::OnNavelExit() const
{
	KnobExit.Broadcast();
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
	float OldValue01 = Value01;
	Value01 += ValueDelta;
	Value01 = FMath::Clamp(Value01, 0.f, 1.f);

	if (SWidget)
	{
		SWidget->UpdateMaterial();
	}

	KnobValueChanged.Broadcast(OldValue01, Value01);
}

void UNaveledKnob::OnNavelDeltaRequested(float ValueDelta)
{
	NavelValueChanged.Broadcast(ValueDelta);
}

#undef LOCTEXT_NAMESPACE
