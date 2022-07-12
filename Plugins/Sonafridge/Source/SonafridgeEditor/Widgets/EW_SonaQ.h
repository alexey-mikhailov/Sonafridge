// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Model/VM_SonaQ.h"
#include "EW_SonaQ.generated.h"

class UEW_EQBandList;
class UEW_SonaQBandPopup;
class UEW_EQManagementStrip;
class UEW_SonaQFrequencyResponse;

UCLASS()
class SONAFRIDGEEDITOR_API UEW_SonaQ : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	static constexpr float DynamicMin = -48.f;
	static constexpr float DynamicMax = +48.f;

	UEW_SonaQ();
	void Init(TSharedPtr<FVM_SonaQ> InViewModel);

	TSharedPtr<FVM_SonaQBand> GetSelectedBand() const { return SelectedBand; }
	void                      SetSelectedBand(TSharedPtr<FVM_SonaQBand> InBand);

	int32 GetSelectedBandIndex() const { return SelectedBandIndex; }
	void  SetSelectedBandIndex(int32 InBandIndex);

	const FVector2D& GetLastSize() const { return LastSize; }

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandSelectionChanged, TSharedPtr<FVM_SonaQBand>)
	FBandSelectionChanged& GetEvent_BandSelectionChanged() { return BandSelectionChanged; }

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandChanging, TSharedPtr<FVM_SonaQBand>);
	FBandChanging& GetEvent_BandChanging() { return BandChanging; }

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandChanged, TSharedPtr<FVM_SonaQBand>);
	FBandChanged& GetEvent_BandChanged() { return BandChanged; }

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandAdded, TSharedPtr<FVM_SonaQBand>);
	FBandAdded& GetEvent_BandAdded() { return BandAdded; }

	DECLARE_EVENT_OneParam(UEW_SonaQ, FBandRemoved, TSharedPtr<FVM_SonaQBand>);
	FBandRemoved& GetEvent_BandRemoved() { return BandRemoved; }

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

	float SampleRate = 44100.f;
	TSharedPtr<FVM_SonaQ> ViewModel;

	UPROPERTY(meta = (BindWidget))
	UEW_SonaQFrequencyResponse* FrequencyResponse;

	UPROPERTY(meta = (BindWidget))
	UEW_SonaQBandPopup* BandPopup;

	TSharedPtr<FVM_SonaQBand> SelectedBand;
	int32 SelectedBandIndex = -1;

private:
	FBandSelectionChanged BandSelectionChanged;
	FBandChanging BandChanging;
	FBandChanged BandChanged;
	FBandAdded BandAdded;
	FBandRemoved BandRemoved;

	FVector2D LastSize;
	bool bWasInitialized = false;
};
