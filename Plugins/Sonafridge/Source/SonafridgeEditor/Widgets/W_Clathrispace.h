// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Model/VM_SonaQ.h"
#include "W_Clathrispace.generated.h"

class UClathrispaceSettings;

UCLASS()
class SONAFRIDGEEDITOR_API UW_Clathrispace : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(UClathrispaceSettings* InPreset);

	const FVector2D& GetLastSize() const { return LastSize; }

protected:
	virtual void  NativeConstruct() override;
	void          OnSizeChanged(const FVector2D& OldSize, const FVector2D& NewSize);
	virtual int32 NativePaint(const FPaintArgs&        Args,
	                          const FGeometry&         AllottedGeometry,
	                          const FSlateRect&        MyCullingRect,
	                          FSlateWindowElementList& OutDrawElements,
	                          int32                    LayerId,
	                          const FWidgetStyle&      InWidgetStyle,
	                          bool                     bParentEnabled) const override;

	UPROPERTY()
	UClathrispaceSettings* Settings;

private:
	FVector2D LastSize;
};
