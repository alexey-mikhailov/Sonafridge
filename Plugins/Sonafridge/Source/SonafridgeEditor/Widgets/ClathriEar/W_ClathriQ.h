// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "W_ClathriQ.generated.h"

class UCanvasPanel;
class UClathrispaceSettings;

UCLASS()
class SONAFRIDGEEDITOR_API UW_ClathriQ : public UUserWidget
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

	UPROPERTY()
	UCanvasPanel* Panel;

private:
	FVector2D LastSize;
};
