// Sonafridge 2022

#include "ToggleKnob.h"

#include "SToggleKnob.h"
#include "Components/RetainerBox.h"
#include "Components/ScaleBox.h"
#include "Components/Widget.h"
#include "Widgets/Images/SImage.h"

#define LOCTEXT_NAMESPACE "Sonafridge"

UToggleKnob::UToggleKnob(const FObjectInitializer& ObjectInitializer)
	: UWidget(ObjectInitializer)
{
	Material = LoadObject<UMaterialInterface>
	(
		nullptr,
		TEXT("/Sonafridge/UI/Materials/M_ToggleKnob.M_ToggleKnob")
	);
}

void UToggleKnob::SetIsOn(bool bInIsOn)
{
	bIsOn = bInIsOn;
	SWidget->UpdateMaterial();
}

void UToggleKnob::SetValue01(float InKnobValue01)
{
	Value01 = InKnobValue01;
	SWidget->UpdateMaterial();
}

void UToggleKnob::SetKnobNavelThreshold01(float InKnobNavelThreshold01)
{
	KnobNavelThreshold01 = InKnobNavelThreshold01;
	SWidget->UpdateMaterial();
}

void UToggleKnob::RefreshVisual()
{
	SWidget->UpdateMaterial();
}

TSharedRef<SWidget> UToggleKnob::RebuildWidget()
{
	SWidget = SNew(SToggleKnob)
		.MouseFastSpeed_Lambda([this] { return FastResponsiveness; })
		.MouseFineSpeed_Lambda([this] { return FineResponsiveness; })
		.IsOn_Lambda([this] { return bIsOn; })
		.Value_Lambda([this] { return Value01; })
		.KnobNavelThreshold_Lambda([this] { return KnobNavelThreshold01; })
		.Blurriness_Lambda([this] { return Blurriness; })
		.Knob0Color_Lambda([this] { return Knob0Color; })
		.Knob1Color_Lambda([this] { return Knob1Color; })
		.Navel0Color_Lambda([this] { return Navel0Color; })
		.Navel1Color_Lambda([this] { return Navel1Color; })
		.IdleIcon_Lambda([this] { return IdleIcon; })
		.HoverIcon_Lambda([this] { return HoverIcon; })
		.IconScale_Lambda([this] { return IconScale; })
		.KnobEntrance_UObject(this, &UToggleKnob::OnKnobEntrance)
		.NavelEntrance_UObject(this, &UToggleKnob::OnNavelEntrance)
		.KnobExit_UObject(this, &UToggleKnob::OnKnobExit)
		.NavelExit_UObject(this, &UToggleKnob::OnNavelExit)
		.KnobCaptureStarted_UObject(this, &UToggleKnob::OnKnobCaptureStarted)
		.KnobCaptureFinished_UObject(this, &UToggleKnob::OnKnobCaptureFinished)
		.NavelCaptureStarted_UObject(this, &UToggleKnob::OnNavelCaptureStarted)
		.NavelCaptureFinished_UObject(this, &UToggleKnob::OnNavelCaptureFinished)
		.ToggleStateRequested_UObject(this, &UToggleKnob::OnToggleStateRequested)
		.KnobDeltaRequested_UObject(this, &UToggleKnob::OnKnobDeltaRequested)
		.NavelDeltaRequested_UObject(this, &UToggleKnob::OnNavelDeltaRequested);

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

void UToggleKnob::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	SWidget.Reset();
}

void UToggleKnob::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	SWidget->UpdateMaterial();
}

const FText UToggleKnob::GetPaletteCategory()
{
	return LOCTEXT("Sonafridge", "Sonafridge");
}

void UToggleKnob::OnKnobEntrance() const
{
	KnobEntrance.Broadcast();
}

void UToggleKnob::OnNavelEntrance() const
{
	NavelEntrance.Broadcast();
}

void UToggleKnob::OnKnobExit() const
{
	KnobExit.Broadcast();
}

void UToggleKnob::OnNavelExit() const
{
	KnobExit.Broadcast();
}

void UToggleKnob::OnKnobCaptureStarted()
{
	KnobCaptureStarted.Broadcast();
}

void UToggleKnob::OnKnobCaptureFinished()
{
	KnobCaptureFinished.Broadcast();
}

void UToggleKnob::OnNavelCaptureStarted()
{
	NavelCaptureStarted.Broadcast();
}

void UToggleKnob::OnNavelCaptureFinished()
{
	NavelCaptureFinished.Broadcast();
}

void UToggleKnob::OnToggleStateRequested()
{
	bIsOn = !bIsOn;
	ToggleStateChanged.Broadcast(!bIsOn, bIsOn);
}

void UToggleKnob::OnKnobDeltaRequested(float ValueDelta)
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

void UToggleKnob::OnNavelDeltaRequested(float ValueDelta)
{
	NavelValueChanged.Broadcast(ValueDelta);
}

#undef LOCTEXT_NAMESPACE
