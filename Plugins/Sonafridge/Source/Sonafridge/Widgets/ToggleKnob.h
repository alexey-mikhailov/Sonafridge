// Sonafridge 2022

#pragma once

#include "Sonafridge/Tools/HSRColor.h"
#include "Components/Widget.h"
#include "CoreMinimal.h"
#include "ToggleKnob.generated.h"

class SToggleKnob;

/// <summary>
/// Finite radial slider.
/// Nabel itself does not display slideable value. It transfers the change of value to others. 
/// </summary>
UCLASS(meta = (DisplayName = "Toggle Knob"))
class SONAFRIDGE_API UToggleKnob : public UWidget
{
	GENERATED_BODY()

public:
	UToggleKnob(const FObjectInitializer& ObjectInitializer);

	DECLARE_EVENT_TwoParams(UToggleKnob, FSizeChanged, const FVector2D& OldSize, const FVector2D& NewSize)
	FSizeChanged& GetEvent_SizeChanged() { return SizeChanged; }

	DECLARE_EVENT(UNaveledKnob, FKnobEntrance)
	FKnobEntrance& GetEvent_KnobEntrance() { return KnobEntrance; }

	DECLARE_EVENT(UNaveledKnob, FNavelEntrance)
	FNavelEntrance& GetEvent_NavelEntrance() { return NavelEntrance; }

	DECLARE_EVENT(UNaveledKnob, FKnobExit)
	FKnobExit& GetEvent_KnobExit() { return KnobExit; }

	DECLARE_EVENT(UNaveledKnob, FNavelExit)
	FNavelExit& GetEvent_NavelExit() { return NavelExit; }

	DECLARE_EVENT(UToggleKnob, FKnobCaptureStarted)
	FKnobCaptureStarted& GetEvent_KnobCaptureStarted() { return KnobCaptureStarted; }

	DECLARE_EVENT(UToggleKnob, FKnobCaptureFinished)
	FKnobCaptureFinished& GetEvent_KnobCaptureFinished() { return KnobCaptureFinished; }

	DECLARE_EVENT(UToggleKnob, FNavelCaptureStarted)
	FNavelCaptureStarted& GetEvent_NavelCaptureStarted() { return NavelCaptureStarted; }

	DECLARE_EVENT(UToggleKnob, FNavelCaptureFinished)
	FNavelCaptureFinished& GetEvent_NavelCaptureFinished() { return NavelCaptureFinished; }

	DECLARE_EVENT_TwoParams(UToggleKnob, FToggleStateChanged, bool Oldvalue, bool NewValue)
	FToggleStateChanged& GetEvent_ToggleStateChanged() { return ToggleStateChanged; }

	DECLARE_EVENT(UToggleKnob, FKnobClick)
	FKnobClick& GetEvent_KnobClick() { return KnobClick; }

	DECLARE_EVENT_FourParams(UToggleKnob, FKnobValueChanged, float OldValue, float NewValue, const FPointerEvent& InMouseEvent, bool& bInOutHaveAllHandlersAccepted)
	FKnobValueChanged& GetEvent_KnobValueChanged() { return KnobValueChanged; }

	DECLARE_EVENT_TwoParams(UToggleKnob, FNavelValueChanged, float ValueDelta, const FPointerEvent& InMouseEvent)
	FNavelValueChanged& GetEvent_NavelValueChanged() { return NavelValueChanged; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsOn = false;

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
	UTexture* IdleIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture* HoverIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float IconScale = 1.f;

	void SetIsOn(bool bInIsOn);
	void SetValue01(float InKnobValue01);
	void SetKnobNavelThreshold01(float InKnobNavelThreshold01);
	void RefreshVisual();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void                ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void                SynchronizeProperties() override;
#if WITH_EDITOR
	virtual const FText         GetPaletteCategory() override;
#endif

private:
	FSizeChanged          SizeChanged;
	FKnobEntrance         KnobEntrance;
	FNavelEntrance        NavelEntrance;
	FKnobExit             KnobExit;
	FNavelExit            NavelExit;
	FKnobCaptureStarted   KnobCaptureStarted;
	FKnobCaptureFinished  KnobCaptureFinished;
	FNavelCaptureStarted  NavelCaptureStarted;
	FNavelCaptureFinished NavelCaptureFinished;
	FToggleStateChanged   ToggleStateChanged;
	FKnobValueChanged     KnobValueChanged;
	FNavelValueChanged    NavelValueChanged;
	FKnobClick            KnobClick;

	TSharedPtr<SToggleKnob> SWidget;
	FVector2D   LastSize;
	FVector2D   LastMousePos;
	FVector2D   PresstimeMousePos;
	FSlateBrush Brush;

	bool bIsDragging = false;

	UPROPERTY()
	UMaterialInstanceDynamic* BrushMID;

	void OnKnobEntrance() const;
	void OnNavelEntrance() const;
	void OnKnobExit() const;
	void OnNavelExit() const;
	void OnKnobCaptureStarted();
	void OnKnobCaptureFinished();
	void OnNavelCaptureStarted();
	void OnNavelCaptureFinished();
	void OnToggleStateRequested();
	void OnKnobClick();
	void OnKnobDeltaRequested(float ValueDelta, const FPointerEvent& InMouseEvent);
	void OnNavelDeltaRequested(float ValueDelta, const FPointerEvent& InMouseEvent);
};


