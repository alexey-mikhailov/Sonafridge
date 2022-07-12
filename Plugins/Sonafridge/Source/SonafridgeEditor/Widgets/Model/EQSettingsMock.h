// Sonafridge 2022

#pragma once

#include "EQBand.h"

struct SONAFRIDGEEDITOR_API IEQSettings
{
	virtual                                    ~IEQSettings() = default;
	virtual bool                               GetIsEnabled() const = 0;
	virtual float                              GetSampleRate() const = 0;
	virtual const TArray<TSharedPtr<FEQBand>>& GetBands() const = 0;

	virtual void SetIsEnabled(bool bNewValue) = 0;

	virtual void AddBand(TSharedPtr<FEQBand> InBand) = 0;
	virtual void RemoveBand(TSharedPtr<FEQBand> InBand) = 0;
	virtual void ResetBand(TSharedPtr<FEQBand> InBand) = 0;
	virtual void Freeze() = 0;
	virtual void Unfreeze() = 0;

	DECLARE_EVENT(UEW_EQBandList, FChanging);
	virtual FChanging& GetEvent_Changing() = 0;

	virtual float Dtft(float InFrequency) const = 0;
	virtual float DtftDb(float InFrequency) const = 0;
};


class SONAFRIDGEEDITOR_API FEQSettingsMock : public IEQSettings
{
	bool bIsEnabled = true;
	float SampleRate = 44100.f;
	TArray<TSharedPtr<FEQBand>> Bands;

public:
	FEQSettingsMock();
	void Init(TArray<TSharedPtr<FEQBand>> InBands);

	virtual bool                               GetIsEnabled() const override { return bIsEnabled; }
	virtual float                              GetSampleRate() const override { return SampleRate; }
	virtual const TArray<TSharedPtr<FEQBand>>& GetBands() const override { return Bands; }

	virtual void SetIsEnabled(bool bNewValue) override { bIsEnabled = bNewValue; }

	virtual void AddBand(TSharedPtr<FEQBand> InBand) override;
	virtual void RemoveBand(TSharedPtr<FEQBand> InBand) override;
	virtual void ResetBand(TSharedPtr<FEQBand> InBand) override;
	virtual void Freeze() override;
	virtual void Unfreeze() override;

	virtual FChanging& GetEvent_Changing() override { return Changing; }

	virtual float Dtft(float InFrequency) const override;
	virtual float DtftDb(float InFrequency) const override;

private:
	FChanging Changing;
};

