// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sonafridge/ColorManagement/HSRColor.h"
#include "KnobWidget.generated.h"

class SKnobWidget;

/// <summary>
/// Finite radial slider.
/// Nabel itself does not display slideable value. It transfers the change of value to others. 
/// </summary>
UCLASS(meta = (DisplayName = "Knob"))
class SONAFRIDGE_API UKnobWidget : public UWidget
{
	GENERATED_BODY()

public:
	UKnobWidget(const FObjectInitializer& ObjectInitializer);

	DECLARE_EVENT_TwoParams(UKnobWidget, FSizeChanged, const FVector2D& OldSize, const FVector2D& NewSize)
	FSizeChanged& GetEvent_SizeChanged() { return SizeChanged; }

	DECLARE_EVENT(UKnobWidget, FMouseCaptureStarted)
	FMouseCaptureStarted& GetEvent_MouseCaptureStarted() { return MouseCaptureStarted; }

	DECLARE_EVENT(UKnobWidget, FMouseCaptureFinished)
	FMouseCaptureFinished& GetEvent_MouseCaptureFinished() { return MouseCaptureFinished; }

	DECLARE_EVENT_TwoParams(UKnobWidget, FValueChanged, float OldValue, float NewValue)
	FValueChanged& GetEvent_ValueChanged() { return ValueChanged; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin = 0.f, UIMax = 1.f))
	float Value01 = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin = 0.f, UIMax = 1.f))
	float Thickness01 = .5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Blurriness = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FastResponsiveness = 2.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FineResponsiveness = .2f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor BackColor = HSR(.62f, .333f, .333f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor ForeColor = HSR(.62f, .667f, .5f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* Material;

	void SetValue01(float InValue01);
	void SetThickness01(float InThickness01);
	void RefreshVisual();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void                ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void                SynchronizeProperties() override;
	virtual const FText         GetPaletteCategory() override;

private:
	FSizeChanged          SizeChanged;
	FMouseCaptureStarted  MouseCaptureStarted;
	FMouseCaptureFinished MouseCaptureFinished;
	FValueChanged         ValueChanged;

	TSharedPtr<SKnobWidget> KnobWidget;
	FVector2D   LastSize;
	FVector2D   LastMousePos;
	FVector2D   PresstimeMousePos;
	FSlateBrush Brush;

	bool bIsDragging = false;

	UPROPERTY()
	UMaterialInstanceDynamic* BrushMID;

	void OnMouseCaptureStarted();
	void OnMouseCaptureFinished();
	void OnValueDeltaRequested(float ValueDelta);
};


