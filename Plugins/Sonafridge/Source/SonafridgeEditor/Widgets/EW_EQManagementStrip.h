// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Sonafridge/Settings_EQ/EQSettingsMock.h"
#include "EW_EQManagementStrip.generated.h"

class UEW_EQ;
class UListView;
class UHorizontalBox;
class UButton;

UCLASS()
class SONAFRIDGEEDITOR_API UEW_EQManagementStrip : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	void Init(UEW_EQ*                 InRootWidget,
	          TSharedPtr<IEQSettings> InSettings);

protected:
	virtual void NativeConstruct() override;

	TSharedPtr<IEQSettings> Settings;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonPower;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonAdd;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonRemove;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonReset;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonToggleFreeze;

private:
	bool bIsFrozen = false;

	UPROPERTY()
	UEW_EQ* RootWidget;

	UFUNCTION()
	void OnToggleOnOff();
	UFUNCTION()
	void OnAddBand();
	UFUNCTION()
	void OnRemoveBand();
	UFUNCTION()
	void OnResetBand();
	UFUNCTION()
	void OnToggleFreeze();
};
