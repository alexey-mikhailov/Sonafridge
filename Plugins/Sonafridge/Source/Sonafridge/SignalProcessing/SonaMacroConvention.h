// Sonafridge 2022

#pragma once

#define SONAFX_SETTINGS_NAME(SFX_NAME) FSFXSettings_ ## SFX_NAME
#define SONAFX_PRESET_NAME(SFX_NAME)   USFXPreset_ ## SFX_NAME
#define SONAFX_CLASS_NAME(SFX_NAME)    FSFX_ ## SFX_NAME

#define GET_SONAFX_SETTINGS(SFX_NAME) \
		SONAFX_PRESET_NAME(SFX_NAME)* _Preset = CastChecked<SONAFX_PRESET_NAME(SFX_NAME)>(Preset); \
		SONAFX_SETTINGS_NAME(SFX_NAME) Settings = _Preset->GetSettings(); \

#define SONAFX_PRESET_METHODS(SFX_NAME) \
		virtual bool CanFilter() const override { return false; } \
		virtual bool HasAssetActions() const { return true; } \
		virtual FText GetAssetActionName() const override { return FText::FromString(#SFX_NAME); } \
		virtual UClass* GetSupportedClass() const override { return SONAFX_PRESET_NAME(SFX_NAME)##::StaticClass(); } \
		virtual FSoundEffectBase* CreateNewEffect() const override { return new SONAFX_CLASS_NAME(SFX_NAME); } \
		virtual USoundEffectPreset* CreateNewPreset(UObject* InParent, FName Name, EObjectFlags Flags) const override \
		{ \
			USoundEffectPreset* NewPreset = NewObject<SONAFX_PRESET_NAME(SFX_NAME)>(InParent, GetSupportedClass(), Name, Flags); \
			NewPreset->Init(); \
			return NewPreset; \
		} \
		virtual void Init() override \
		{ \
			FScopeLock ScopeLock(&SettingsCritSect); \
			SettingsCopy = Settings; \
		} \
		void UpdateSettings(const SONAFX_SETTINGS_NAME(SFX_NAME)& InSettings) \
		{ \
			FScopeLock ScopeLock(&SettingsCritSect); \
			SettingsCopy = InSettings; \
			Update(); \
		} \
		void UpdateSettings(TUniqueFunction<void(SONAFX_SETTINGS_NAME(SFX_NAME)&)> InCommand) \
		{ \
			FScopeLock ScopeLock(&SettingsCritSect); \
			InCommand(SettingsCopy); \
			Update(); \
		} \
		SONAFX_SETTINGS_NAME(SFX_NAME) GetSettings() \
		{ \
			FScopeLock ScopeLock(&SettingsCritSect); \
			return SettingsCopy; \
		} \
		FCriticalSection SettingsCritSect; \
		SONAFX_SETTINGS_NAME(SFX_NAME) SettingsCopy; \
