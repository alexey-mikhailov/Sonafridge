// Sonafridge 2022

#include "SFX_SonaQ.h"
#include "SonaMacroConvention.h"

bool FSFXSettings_SonaQBand::IsGainEnabled() const
{
	return Type == EEQBandType::AttBand ||
	       Type == EEQBandType::AttLow ||
	       Type == EEQBandType::AttHigh;
}

void FSFX_SonaQ::Init(const FSoundEffectSourceInitData& InInitData)
{
	SampleRate = InInitData.SampleRate;
	NumChannels = InInitData.NumSourceChannels;

	if (USFXPreset_SonaQ* SonaQPreset = StaticCast<USFXPreset_SonaQ*>(Preset.Get()))
	{
		FSFXSettings_SonaQ Settings = SonaQPreset->GetSettings();
	}
}

void FSFX_SonaQ::OnPresetChanged()
{
	// Macro to retrieve the current settings value of the parent preset asset.
	GET_SONAFX_SETTINGS(SonaQ);

	bEnabled = Settings.bEnabled;
	OutputGain = FMath::Pow(10.0f, Settings.OutputDb / 20.f);

	const int32 NumSettingBands = Settings.EQBands.Num();

	if (NumSettingBands != Filters.Num())
	{
		// Remove all
		Filters.Empty();
		Filters.AddDefaulted(NumSettingBands);

		// Re add all
		for (int32 I = 0; I < NumSettingBands; ++I)
		{
			const FSFXSettings_SonaQBand& Band = Settings.EQBands[I];
			Filters[I].SetEnabled(Band.bEnabled);
			Filters[I].Init(SampleRate,
			                NumChannels,
							Band.MakeupDb,
			                Band.Type,
			                Band.AmountDb,
			                FMath::Max(20.0f, Band.Frequency),
			                Band.Quality);
		}
	}
	else
	{
		check(Settings.EQBands.Num() <= Filters.Num());

		// Now make sure the filters settings are the same as the eq settings
		for (int32 I = 0; I < NumSettingBands; ++I)
		{
			const FSFXSettings_SonaQBand& Band = Settings.EQBands[I];

			Filters[I].SetEnabled(Band.bEnabled);
			Filters[I].SetParams(Band.MakeupDb, 
								 Band.Type,
			                     Band.AmountDb,
			                     FMath::Max(20.0f, Band.Frequency),
			                     Band.Quality);
		}
	}
}

void FSFX_SonaQ::ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData)
{
	float* InAudioBufferData = InData.InputSourceEffectBufferPtr;

	if (bEnabled)
	{
		if (Filters.Num() == 0)
		{
			FMemory::Memcpy(InAudioBufferData, InAudioBufferData, sizeof(float) * InData.NumSamples);
		}
		else
		{
			for (int32 FilterIndex = 0; FilterIndex < Filters.Num(); ++FilterIndex)
			{
				Filters[FilterIndex].ProcessAudio(InAudioBufferData, InData.NumSamples, InAudioBufferData);
			}
		}

		for (int32 SampleIndex = 0; SampleIndex < InData.NumSamples; ++SampleIndex)
		{
			OutAudioBufferData[SampleIndex] = InAudioBufferData[SampleIndex] * OutputGain;
		}
	}
	else
	{
		FMemory::Memcpy(OutAudioBufferData, InAudioBufferData, sizeof(float) * InData.NumSamples);
	}
}

void USFXPreset_SonaQ::SetSettings(const FSFXSettings_SonaQ& InSettings)
{
	// Performs necessary broadcast to effect instances
	UpdateSettings(InSettings);
}

void USFXPreset_SonaQ::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	Changed.Broadcast();
}

