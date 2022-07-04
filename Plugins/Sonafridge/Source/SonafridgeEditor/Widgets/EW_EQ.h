// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Sonafridge/Settings_EQ/EQSettingsMock.h"
#include "EW_EQ.generated.h"

class UEW_EQBandList;
class UEW_EQBandPopup;
class UEW_EQManagementStrip;
class UEW_EQFrequencyResponse;

UCLASS()
class SONAFRIDGEEDITOR_API UEW_EQ : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	static constexpr float DynamicMin = -48.f;
	static constexpr float DynamicMax = +48.f;

	UEW_EQ();
	void Init(TSharedPtr<IEQSettings> InSettings);

	TSharedPtr<FEQBand> GetSelectedBand() const { return SelectedBand; }
	void                SetSelectedBand(TSharedPtr<FEQBand> InBand);

	int32 GetSelectedBandIndex() const { return SelectedBandIndex; }
	void  SetSelectedBandIndex(int32 InBandIndex);

	const FVector2D& GetLastSize() const { return LastSize; }

	DECLARE_EVENT_OneParam(UEW_EQBandList, FBandSelectionChanged, TSharedPtr<FEQBand>)
	FBandSelectionChanged& GetEvent_BandSelectionChanged() { return BandSelectionChanged; }

	DECLARE_EVENT_OneParam(UEW_EQBandList, FBandChanging, TSharedPtr<FEQBand>);
	FBandChanging& GetEvent_BandChanging() { return BandChanging; }

	DECLARE_EVENT_OneParam(UEW_EQBandList, FBandChanged, TSharedPtr<FEQBand>);
	FBandChanged& GetEvent_BandChanged() { return BandChanged; }

	DECLARE_EVENT_OneParam(UEW_EQBandList, FBandAdded, TSharedPtr<FEQBand>);
	FBandAdded& GetEvent_BandAdded() { return BandAdded; }

	DECLARE_EVENT_OneParam(UEW_EQBandList, FBandRemoved, TSharedPtr<FEQBand>);
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
	TSharedPtr<IEQSettings> Settings;

	UPROPERTY(meta = (BindWidget))
	UEW_EQFrequencyResponse* FrequencyResponse;

	UPROPERTY(meta = (BindWidget))
	UEW_EQBandPopup* BandPopup;

	TSharedPtr<FEQBand> SelectedBand;
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
