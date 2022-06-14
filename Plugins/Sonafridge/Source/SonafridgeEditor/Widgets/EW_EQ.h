#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Sonafridge/Settings_EQ/EQSettingsMock.h"
#include "EW_EQ.generated.h"

class UEW_EQBandList;
class UEW_EQManagementStrip;
class UEW_EQFrequencyResponse;

UCLASS()
class SONAFRIDGEEDITOR_API UEW_EQ : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UEW_EQ();
	void Init(TSharedPtr<IEQSettings> InSettings);

	TSharedPtr<FEQBand> GetSelectedBand() const { return SelectedBand; }
	void                SetSelectedBand(TSharedPtr<FEQBand> InBand);

	int32 GetSelectedBandIndex() const { return SelectedBandIndex; }
	void  SetSelectedBandIndex(int32 InBandIndex);

	DECLARE_EVENT_OneParam(UEW_EQBandList, FBandSelectionChanged, TSharedPtr<FEQBand>)


	FBandSelectionChanged& GetEvent_BandSelectionChanged() { return BandSelectionChanged; }

	DECLARE_EVENT_OneParam(UEW_EQBandList, FBandChanged, TSharedPtr<FEQBand>);
	FBandChanged& GetEvent_BandChanged() { return BandChanged; }

	DECLARE_EVENT_OneParam(UEW_EQBandList, FBandAdded, TSharedPtr<FEQBand>);
	FBandAdded& GetEvent_BandAdded() { return BandAdded; }

	DECLARE_EVENT_OneParam(UEW_EQBandList, FBandRemoved, TSharedPtr<FEQBand>);
	FBandRemoved& GetEvent_BandRemoved() { return BandRemoved; }

protected:
	virtual void NativeConstruct() override;

	float SampleRate = 44100.f;
	TSharedPtr<IEQSettings> Settings;

	UPROPERTY(meta = (BindWidget))
	UEW_EQFrequencyResponse* FrequencyResponse;

	UPROPERTY(meta = (BindWidget))
	UEW_EQManagementStrip* ManagementStrip;

	UPROPERTY(meta = (BindWidget))
	UEW_EQBandList* BandList;

	TSharedPtr<FEQBand> SelectedBand;
	int32 SelectedBandIndex = -1;

private:
	FBandSelectionChanged BandSelectionChanged;
	FBandChanged BandChanged;
	FBandAdded BandAdded;
	FBandRemoved BandRemoved;
	bool bWasInitialized = false;
};
