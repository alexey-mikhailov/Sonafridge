// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sonafridge/ColorManagement/HSRColor.h"
#include "NavelWidget.generated.h"

class SNavelWidget;


/// <summary>
/// Widget which combines checkbox and slider.
/// Nabel itself does not display slideable value. It transfers the change of value to others. 
/// </summary>
UCLASS()
class SONAFRIDGE_API UNavelWidget : public UWidget
{
	GENERATED_BODY()

public:
	UNavelWidget(const FObjectInitializer& ObjectInitializer);

	DECLARE_EVENT_TwoParams(UNavelWidget, FSizeChanged, const FVector2D& OldSize, const FVector2D& NewSize)
	FSizeChanged GetEvent_SizeChanged() const { return SizeChanged; }

	DECLARE_EVENT_TwoParams(UNavelWidget, FToggleStateChanged, bool Oldvalue, bool NewValue)
	FToggleStateChanged GetEvent_ToggleStateChanged() const { return ToggleStateChanged; }

	DECLARE_EVENT_OneParam(UNavelWidget, FValueChanged, float ValueDelta)
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


private:
	bool          bIsDragging = false;

	FSizeChanged        SizeChanged;
	FToggleStateChanged ToggleStateChanged;
	FValueChanged       ValueChanged;

	FVector2D     LastSize;
	FVector2D     LastMousePos;
	FVector2D     PresstimeMousePos;
	FSlateBrush   Brush;

	TSharedPtr<SNavelWidget> NavelWidget;

	UPROPERTY()
	UMaterialInstanceDynamic* BrushMID;

	void OnToggleStateRequested();
	void OnValueDeltaRequested(float ValueDelta);
};
