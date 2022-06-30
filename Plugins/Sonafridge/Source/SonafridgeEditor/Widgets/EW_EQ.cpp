// Sonafridge 2022

#include "EW_EQ.h"
#include "EW_EQFrequencyResponse.h"
#include "AudioDevice.h"
#include "EW_EQBandList.h"
#include "EW_EQManagementStrip.h"

UEW_EQ::UEW_EQ()
{
}

void UEW_EQ::Init(TSharedPtr<IEQSettings> InSettings)
{
	SampleRate = InSettings->GetSampleRate();
	Settings = InSettings;
	FrequencyResponse->Init(this, Settings);
	ManagementStrip->Init(this, Settings);
	BandList->Init(this, Settings);
	bWasInitialized = true;
}

void UEW_EQ::SetSelectedBand(TSharedPtr<FEQBand> InBand)
{
	SelectedBand = InBand;
	SelectedBandIndex = -1;

	auto& Bands = Settings->GetBands();
	for (int32 Index = 0; Index < Bands.Num(); ++Index)
	{
		auto Band = Bands[Index];

		if (Band == InBand)
		{
			SelectedBandIndex = Index;
			break;
		}
	}

	BandSelectionChanged.Broadcast(SelectedBand);
}

void UEW_EQ::SetSelectedBandIndex(int32 InBandIndex)
{
	SelectedBand = {};
	SelectedBandIndex = InBandIndex;

	auto& Bands = Settings->GetBands();

	for (int32 Index = 0; Index < Bands.Num(); ++Index)
	{
		auto& Band = Bands[Index];

		if (Index == InBandIndex)
		{
			SelectedBand = Band;
			break;
		}
	}

	BandSelectionChanged.Broadcast(SelectedBand);
}

void UEW_EQ::NativeConstruct()
{
	Super::NativeConstruct();

	if (!bWasInitialized)
	{
		// Deferred initialization in case of settings injection (Init call) is impossible. 

		if (auto AudioDevice = GEngine->GetActiveAudioDevice())
		{
			SampleRate = AudioDevice->GetSampleRate();
		}

		TArray<TSharedPtr<FEQBand>> Bands;
		auto Band_BC = MakeShared<FEQBand>();
		Band_BC->Init(SampleRate);
		Band_BC->SetType(EBandType::BandCut);
		Band_BC->SetFrequency(1000.f);
		Band_BC->SetQuality(10.f);
		Band_BC->SetAmountDb(-6.f);
		Band_BC->SetLoudCompDb(2.f);
		
		Bands.Add(Band_BC);
		
		auto SettingsMock = MakeShared<FEQSettingsMock>();
		SettingsMock->Init(Bands);
		Settings = SettingsMock;

		// Children too

		FrequencyResponse->Init(this, Settings);
		ManagementStrip->Init(this, Settings);
		BandList->Init(this, Settings);
	}
}

