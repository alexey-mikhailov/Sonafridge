// Sonafridge 2022

#include "EW_SonaQ.h"
#include "EW_SonaQFrequencyResponse.h"
#include "EW_SonaQBandPopup.h"
#include "Sonafridge/SignalProcessing/SFX_SonaQ.h"
#include "Sonafridge/SonafridgeCommon.h"
#include "Components/CanvasPanelSlot.h"
#include "AudioDevice.h"

UEW_SonaQ::UEW_SonaQ()
{
}

void UEW_SonaQ::Init(USFXPreset_SonaQ* InPreset)
{
	if (IsValid(InPreset))
	{
		Preset = InPreset;
		Preset->GetEvent_Changed().AddUObject(this, &UEW_SonaQ::OnExternalChanged);
	}
	else
	{
		UE_LOG(LogSonafridgeEditor, Error, TEXT("UEW_SonaQ::Init: InPreset argument is not valid. "));
	}
}

void UEW_SonaQ::NativeConstruct()
{
	Super::NativeConstruct();

	// Last known default value from project template settings. 
	float SampleRate = 48000.f;

	if (auto AudioDevice = GEngine->GetActiveAudioDevice())
	{
		// We cannot obtain SampleRate from SFX due to encapsulation
		// but we can do it this way.
		// Note that design time sample rate is not very important visualization input quality metric. 
		SampleRate = AudioDevice->GetSampleRate();
	}

	// Initialize view model (either from preset or from mock). 

	ViewModel = MakeShared<FVM_SonaQ>();
	ViewModel->Init(SampleRate);
	ViewModel->GetEvent_BandChanged().AddUObject(this, &UEW_SonaQ::OnInternalChanged);
	ViewModel->GetEvent_BandChanging().AddUObject(this, &UEW_SonaQ::OnInternalChanging);

	if (IsValid(Preset))
	{
		// Initialize from preset

		FSFXSettings_SonaQ Settings = Preset->Settings;
		TArray<TSharedPtr<FVM_SonaQBand>> Bands;

		for (const FSFXSettings_SonaQBand& Band : Settings.EQBands)
		{
			TSharedPtr<FVM_SonaQBand> BandViewModel = MakeShared<FVM_SonaQBand>();
			BandViewModel->Init(SampleRate);
			BandViewModel->SetIsEnabled(Band.bEnabled);
			BandViewModel->SetType(Band.Type);
			BandViewModel->SetFrequency(Band.Frequency);
			BandViewModel->SetQuality(Band.Quality);
			BandViewModel->SetAmountDb(Band.AmountDb);
			BandViewModel->SetMakeupDb(Band.MakeupDb);
			Bands.Add(BandViewModel);
		}

		ViewModel->SetBands(Bands);
	}
	else
	{
		// Deferred initialization in case of settings injection (Init call) is impossible.
		// It may happen when editor widget is called via 'Run Editor Widget' command.

		TArray<TSharedPtr<FVM_SonaQBand>> Bands;
		auto Band = MakeShared<FVM_SonaQBand>();
		Band->Init(SampleRate);
		Band->SetType(EEQBandType::AttLow);
		Band->SetFrequency(100.f);
		Band->SetQuality(.5f);
		Band->SetAmountDb(0.f);
		Band->SetMakeupDb(0.f);
		Bands.Add(Band);

		Band = MakeShared<FVM_SonaQBand>();
		Band->Init(SampleRate);
		Band->SetType(EEQBandType::AttBand);
		Band->SetFrequency(1000.f);
		Band->SetQuality(.667f);
		Band->SetAmountDb(0.f);
		Band->SetMakeupDb(0.f);
		Bands.Add(Band);
		
		Band = MakeShared<FVM_SonaQBand>();
		Band->Init(SampleRate);
		Band->SetType(EEQBandType::AttHigh);
		Band->SetFrequency(10000.f);
		Band->SetQuality(.5f);
		Band->SetAmountDb(0.f);
		Band->SetMakeupDb(0.f);
		Bands.Add(Band);

		ViewModel->SetBands(Bands);
	}

	// Children too
	FrequencyResponse->Init(this, ViewModel);
	BandPopup->Init(this, ViewModel);
	BandPopup->SetVisibility(ESlateVisibility::Hidden);
}

void UEW_SonaQ::OnSizeChanged(const FVector2D& OldSize, const FVector2D& NewSize)
{
	BandPopup->FollowBand();
}

void UEW_SonaQ::OnInternalChanging(TSharedPtr<FVM_SonaQBand> InBand)
{
	TransferViewModelToPreset();
}

void UEW_SonaQ::OnInternalChanged(TSharedPtr<FVM_SonaQBand> InBand)
{
	TransferViewModelToPreset();
}

void UEW_SonaQ::OnExternalChanged()
{
	FSFXSettings_SonaQ Settings = Preset->SettingsCopy;
	TArray<TSharedPtr<FVM_SonaQBand>> Bands;

	for (const FSFXSettings_SonaQBand& Band : Settings.EQBands)
	{
		TSharedPtr<FVM_SonaQBand> BandViewModel = MakeShared<FVM_SonaQBand>();
		BandViewModel->Init(ViewModel->GetSampleRate());
		BandViewModel->SetType(Band.Type);
		BandViewModel->SetFrequency(Band.Frequency);
		BandViewModel->SetAmountDb(Band.AmountDb);
		BandViewModel->SetQuality(Band.Quality);
		BandViewModel->SetMakeupDb(Band.MakeupDb);
		Bands.Add(BandViewModel);
	}

	ViewModel->SetBands(Bands);
	FrequencyResponse->BakeResponse();
}

int32 UEW_SonaQ::NativePaint(const FPaintArgs&        Args,
                             const FGeometry&         AllottedGeometry,
                             const FSlateRect&        MyCullingRect,
                             FSlateWindowElementList& OutDrawElements,
                             int32                    LayerId,
                             const FWidgetStyle&      InWidgetStyle,
                             bool                     bParentEnabled) const
{
	int32 Result = Super::NativePaint(Args,
	                                  AllottedGeometry,
	                                  MyCullingRect,
	                                  OutDrawElements,
	                                  LayerId,
	                                  InWidgetStyle,
	                                  bParentEnabled);

	UEW_SonaQ* MutableThis = const_cast<UEW_SonaQ*>(this);
	FVector2D  Size = AllottedGeometry.GetLocalSize();

	if (Size != LastSize)
	{
		MutableThis->OnSizeChanged(LastSize, Size);
	}

	MutableThis->LastSize = Size;
	return Result;
}

void UEW_SonaQ::TransferViewModelToPreset()
{
	FSFXSettings_SonaQ                Settings = Preset->GetSettings();
	TArray<TSharedPtr<FVM_SonaQBand>> VMBands = ViewModel->GetBands();

	Settings.EQBands.Reset(VMBands.Num());

	for (int32 Index = 0; Index < VMBands.Num(); ++Index)
	{
		TSharedPtr<FVM_SonaQBand> BandViewModel = VMBands[Index];
		FSFXSettings_SonaQBand    Band;

		Band.bEnabled = BandViewModel->GetIsEnabled();
		Band.Type = BandViewModel->GetType();
		Band.Frequency = BandViewModel->GetFrequency();
		Band.AmountDb = BandViewModel->GetAmountDb();
		Band.Quality = BandViewModel->GetQuality();
		Band.MakeupDb = BandViewModel->GetMakeupDb();

		Settings.EQBands.Add(Band);
	}

	// Sync external widgets. 
	Preset->SetSettings(Settings);
	Preset->Settings = Settings;

	if (IsValid(Preset))
	{
		if (!Preset->MarkPackageDirty())
		{
			UE_LOG(LogSonafridgeEditor, Error, TEXT("Could not mark package dirty during USFXPreset changing. "));
		}
	}
}

