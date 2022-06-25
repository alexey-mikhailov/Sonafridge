// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sonafridge/ColorManagement/HSRColor.h"
#include "ToggleNavelWidget.generated.h"

class SToggleNavelWidget;


/// <summary>
/// Widget which combines checkbox and finite radial slider.
/// ToggleNabel itself does not display slideable value. It transfers the change of value to others. 
/// </summary>
UCLASS(meta = (DisplayName = "Toggle Navel"))
class SONAFRIDGE_API UToggleNavelWidget : public UWidget
{
	GENERATED_BODY()

public:
	UToggleNavelWidget(const FObjectInitializer& ObjectInitializer);

	DECLARE_EVENT_TwoParams(UToggleNavelWidget, FSizeChanged, const FVector2D& OldSize, const FVector2D& NewSize)
	FSizeChanged GetEvent_SizeChanged() const { return SizeChanged; }

	DECLARE_EVENT_TwoParams(UToggleNavelWidget, FToggleStateChanged, bool Oldvalue, bool NewValue)
	FToggleStateChanged GetEvent_ToggleStateChanged() const { return ToggleStateChanged; }

	DECLARE_EVENT(UToggleNavelWidget, FMouseCaptureStarted)
	FMouseCaptureStarted GetEvent_MouseCaptureStarted() const { return MouseCaptureStarted; }

	DECLARE_EVENT(UToggleNavelWidget, FMouseCaptureFinished)
	FMouseCaptureFinished GetEvent_MouseCaptureFinished() const { return MouseCaptureFinished; }

	DECLARE_EVENT_OneParam(UToggleNavelWidget, FValueChanged, float ValueDelta)
	FValueChanged GetEvent_ValueChanged() const { return ValueChanged; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsOn = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Blurriness = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FastResponsiveness = 2.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FineResponsiveness = .2f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor BackColor = HSR(.62f, .333f, .4f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor ForeColor = HSR(.62f, .667f, .6f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* Material;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* AlphaMask;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void SynchronizeProperties() override;
	virtual const FText GetPaletteCategory() override;

private:
	FSizeChanged          SizeChanged;
	FToggleStateChanged   ToggleStateChanged;
	FMouseCaptureStarted  MouseCaptureStarted;
	FMouseCaptureFinished MouseCaptureFinished;
	FValueChanged         ValueChanged;

	TSharedPtr<SToggleNavelWidget> NavelWidget;
	FVector2D     LastSize;
	FVector2D     LastMousePos;
	FVector2D     PresstimeMousePos;
	FSlateBrush   Brush;

	bool          bIsDragging = false;

	UPROPERTY()
	UMaterialInstanceDynamic* BrushMID;

	void OnToggleStateRequested();
	void OnMouseCaptureStarted();
	void OnMouseCaptureFinished();
	void OnValueDeltaRequested(float ValueDelta);
};
