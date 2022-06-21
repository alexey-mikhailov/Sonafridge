// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sonafridge/ColorManagement/HSRColor.h"
#include "KnobWidget.generated.h"

class SKnobWidget;

UCLASS()
class SONAFRIDGE_API UKnobWidget : public UWidget
{
	GENERATED_BODY()

public:
	UKnobWidget(const FObjectInitializer& ObjectInitializer);

	DECLARE_EVENT_TwoParams(UKnobWidget, FSizeChanged, const FVector2D& OldSize, const FVector2D& NewSize)
	FSizeChanged GetEvent_SizeChanged() const { return SizeChanged; }

	DECLARE_EVENT_TwoParams(UKnobWidget, FValueChanged, float OldValue, float NewValue)
	FValueChanged GetEvent_ValueChanged() const { return ValueChanged; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin = 0.f, UIMax = 1.f))
	float Value = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float NormalThickness = .5f;

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

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void SynchronizeProperties() override;


private:
	bool          bIsDragging = false;
	FSizeChanged  SizeChanged;
	FValueChanged ValueChanged;
	FVector2D     LastSize;
	FVector2D     LastMousePos;
	FVector2D     PresstimeMousePos;
	FSlateBrush   Brush;

	TSharedPtr<SKnobWidget> KnobWidget;

	UPROPERTY()
	UMaterialInstanceDynamic* BrushMID;

	void OnValueDeltaRequested(float ValueDelta);
};
