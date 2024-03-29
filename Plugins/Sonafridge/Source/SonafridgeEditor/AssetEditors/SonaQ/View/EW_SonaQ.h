// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "SonafridgeEditor/AssetEditors/SonaQ/ViewModel/VM_SonaQ.h"
#include "EW_SonaQ.generated.h"

class USFXPreset_SonaQ;
class UEW_SonaQBandPopup;
class UEW_SonaQFrequencyResponse;

UCLASS()
class SONAFRIDGEEDITOR_API UEW_SonaQ : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UEW_SonaQ();
	void Init(USFXPreset_SonaQ* InPreset);

	const FVector2D& GetLastSize() const { return LastSize; }

protected:
	virtual void  NativeConstruct() override;
	void          OnSizeChanged(const FVector2D& OldSize, const FVector2D& NewSize);
	void          OnInternalChanging(TSharedPtr<FVM_SonaQBand> InBand);
	void          OnInternalChanged(TSharedPtr<FVM_SonaQBand> InBand);
	void          OnExternalChanged();
	virtual int32 NativePaint(const FPaintArgs&        Args,
	                          const FGeometry&         AllottedGeometry,
	                          const FSlateRect&        MyCullingRect,
	                          FSlateWindowElementList& OutDrawElements,
	                          int32                    LayerId,
	                          const FWidgetStyle&      InWidgetStyle,
	                          bool                     bParentEnabled) const override;

	UPROPERTY(Transient)
	USFXPreset_SonaQ* Preset = nullptr;

	TSharedPtr<FVM_SonaQ> ViewModel;

	UPROPERTY(meta = (BindWidget))
	UEW_SonaQFrequencyResponse* FrequencyResponse;

	UPROPERTY(meta = (BindWidget))
	UEW_SonaQBandPopup* BandPopup;

private:
	void TransferViewModelToPreset();

	FVector2D LastSize;
};
