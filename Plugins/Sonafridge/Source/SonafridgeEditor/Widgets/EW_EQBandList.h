// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Sonafridge/Settings_EQ/EQSettingsMock.h"
#include "EW_EQBandList.generated.h"

class UListView;
struct FEQBand;
class UProxy_EQBand;
class UEW_EQ;

UCLASS()
class SONAFRIDGEEDITOR_API UEW_EQBandList : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	void Init(UEW_EQ*                 InRootWidget,
	          TSharedPtr<IEQSettings> InSettings);

protected:
	virtual void NativeConstruct() override;
	void         OnSelectedBandChanged(UObject* Object);
	void         OnBandAdded(TSharedPtr<FEQBand> InBand);
	void         OnBandRemoved(TSharedPtr<FEQBand> InBand);

	TSharedPtr<IEQSettings> Settings;

	UPROPERTY()
	UEW_EQ* RootWidget;

	UPROPERTY(meta = (BindWidget))
	UListView* ListViewBands;

private:
	void ConcurrentInit();

	// No need to make it UPROPERTY, proxies are kept by list view itself. 
	TMap<TSharedPtr<FEQBand>, UProxy_EQBand*> BandProxies;
};
