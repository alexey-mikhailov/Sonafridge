// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Sonafridge/ColorManagement/HSRColor.h"
#include "Components/Widget.h"
#include "CoreMinimal.h"
#include "NaveledKnob.generated.h"

class SNaveledKnob;

/// <summary>
/// Finite radial slider.
/// Nabel itself does not display slideable value. It transfers the change of value to others. 
/// </summary>
UCLASS(meta = (DisplayName = "Naveled Knob"))
class SONAFRIDGE_API UNaveledKnob : public UWidget
{
	GENERATED_BODY()

public:
	UNaveledKnob(const FObjectInitializer& ObjectInitializer);

	DECLARE_EVENT_TwoParams(UNaveledKnob, FSizeChanged, const FVector2D& OldSize, const FVector2D& NewSize)
	FSizeChanged& GetEvent_SizeChanged() { return SizeChanged; }

	DECLARE_EVENT(UNaveledKnob, FKnobCaptureStarted)
	FKnobCaptureStarted& GetEvent_KnobCaptureStarted() { return KnobCaptureStarted; }

	DECLARE_EVENT(UNaveledKnob, FKnobCaptureFinished)
	FKnobCaptureFinished& GetEvent_KnobCaptureFinished() { return KnobCaptureFinished; }

	DECLARE_EVENT(UNaveledKnob, FNavelCaptureStarted)
	FNavelCaptureStarted& GetEvent_NavelCaptureStarted() { return NavelCaptureStarted; }

	DECLARE_EVENT(UNaveledKnob, FNavelCaptureFinished)
	FNavelCaptureFinished& GetEvent_NavelCaptureFinished() { return NavelCaptureFinished; }

	DECLARE_EVENT_TwoParams(UNaveledKnob, FKnobValueChanged, float OldValue, float NewValue)
	FKnobValueChanged& GetEvent_KnobValueChanged() { return KnobValueChanged; }

	DECLARE_EVENT_OneParam(UNaveledKnob, FNavelValueChanged, float ValueDelta)
	FNavelValueChanged& GetEvent_NavelValueChanged() { return NavelValueChanged; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin = 0.f, UIMax = 1.f))
	float Value01 = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin = 0.f, UIMax = 1.f))
	float KnobNavelThreshold01 = .5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Blurriness = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FastResponsiveness = 2.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FineResponsiveness = .2f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Knob0Color = HSR(.62f, .333f, .333f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Knob1Color = HSR(.62f, .667f, .5f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Navel0Color = HSR(.62f, .5f, .417f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Navel1Color = HSR(.62f, .5f, .75f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* Material;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture* Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float IconScale = 1.f;

	void SetValue01(float InKnobValue01);
	void SetKnobNavelThreshold01(float InKnobNavelThreshold01);
	void RefreshVisual();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void                ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void                SynchronizeProperties() override;
	virtual const FText         GetPaletteCategory() override;

private:
	FSizeChanged          SizeChanged;
	FKnobCaptureStarted   KnobCaptureStarted;
	FKnobCaptureFinished  KnobCaptureFinished;
	FNavelCaptureStarted  NavelCaptureStarted;
	FNavelCaptureFinished NavelCaptureFinished;
	FKnobValueChanged     KnobValueChanged;
	FNavelValueChanged    NavelValueChanged;

	TSharedPtr<SNaveledKnob> SWidget;
	FVector2D   LastSize;
	FVector2D   LastMousePos;
	FVector2D   PresstimeMousePos;
	FSlateBrush Brush;

	bool bIsDragging = false;

	UPROPERTY()
	UMaterialInstanceDynamic* BrushMID;

	void OnKnobCaptureStarted();
	void OnKnobCaptureFinished();
	void OnNavelCaptureStarted();
	void OnNavelCaptureFinished();
	void OnKnobDeltaRequested(float ValueDelta);
	void OnNavelDeltaRequested(float ValueDelta);
};

