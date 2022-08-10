// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "SonafridgeEditor/AssetEditors/Clathrispace/AssetEditor_Clathrispace.h"
#include "SonafridgeEditor/AssetEditors/Clathrispace/ViewModel/VM_ClathriQ.h"
#include "W_ClathriQ.generated.h"

class UClathrispaceSettings;
class UW_ClathriQBandPopup;
class UW_ClathriQFrequencyResponse;

UCLASS()
class SONAFRIDGEEDITOR_API UW_ClathriQ : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UW_ClathriQ();
	void Init(UClathrispaceSettings*        InSettings,
	          TDelegate<void(int32)>& InPinIndexChanged);

	const FVector2D& GetLastSize() const { return LastSize; }

protected:
	virtual void  NativeConstruct() override;
	void          OnSizeChanged(const FVector2D& OldSize, const FVector2D& NewSize);
	void          OnInternalChanging(int32 BandIndex);
	void          OnInternalChanged(int32 BandIndex);
	void          OnExternalChanged();

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, 
										   const FPointerEvent& InMouseEvent) override;

	virtual int32 NativePaint(const FPaintArgs&        Args,
	                          const FGeometry&         AllottedGeometry,
	                          const FSlateRect&        MyCullingRect,
	                          FSlateWindowElementList& OutDrawElements,
	                          int32                    LayerId,
	                          const FWidgetStyle&      InWidgetStyle,
	                          bool                     bParentEnabled) const override;

	UPROPERTY(Transient)
	UClathrispaceSettings* Settings = nullptr;

	TSharedPtr<FVM_ClathriQ> ViewModel;

	UPROPERTY(meta = (BindWidget))
	UW_ClathriQFrequencyResponse* FrequencyResponse;
	TDelegate<void(int32)>        PinIndexChanged;

	//UPROPERTY(meta = (BindWidget))
	//UW_ClathriQBandPopup* BandPopup;

private:
	FVector2D LastSize;
};
