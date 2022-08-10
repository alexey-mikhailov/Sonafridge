// Sonafridge 2022

#include "W_ClathriQ.h"
#include "W_ClathriQFrequencyResponse.h"
#include "W_ClathriQBandPopup.h"
#include "Sonafridge/Attenuator/Clathrispace.h"
#include "Components/CanvasPanelSlot.h"
#include "AudioDevice.h"

UW_ClathriQ::UW_ClathriQ()
{
}

void UW_ClathriQ::Init(UClathrispaceSettings*        InSettings,
                       TDelegate<void(int32)>& InPinIndexChanged)
{
	if (IsValid(InSettings))
	{
		Settings = InSettings;
		Settings->GetEvent_ExternallyChanged().AddUObject(this, &UW_ClathriQ::OnExternalChanged);
	}
	else
	{
		UE_LOG(LogSonafridgeEditor, Error, TEXT("UW_ClathriQ::Init: InSettings argument is not valid. "));
	}

	// Last known default value from project template settings. 
	float SampleRate = 48000.f;

	if (auto AudioDevice = GEngine->GetActiveAudioDevice())
	{
		// We cannot obtain SampleRate from SFX due to encapsulation
		// but we can do it this way.
		// Note that design time sample rate is not very important visualization input quality metric. 
		SampleRate = AudioDevice->GetSampleRate();
	}

	// Initialize view model from preset. 

	ViewModel = MakeShared<FVM_ClathriQ>();
	ViewModel->Init(Settings, InPinIndexChanged, SampleRate);
	ViewModel->GetEvent_BandChanged().AddUObject(this, &UW_ClathriQ::OnInternalChanged);
	ViewModel->GetEvent_BandChanging().AddUObject(this, &UW_ClathriQ::OnInternalChanging);
}

void UW_ClathriQ::NativeConstruct()
{
	Super::NativeConstruct();

	FrequencyResponse->Init(this, ViewModel);
	BandPopup->Init(this, ViewModel);
	BandPopup->SetVisibility(ESlateVisibility::Hidden);
}

void UW_ClathriQ::OnSizeChanged(const FVector2D& OldSize, const FVector2D& NewSize)
{
	if (IsValid(BandPopup))
	{
		BandPopup->FollowBand();
	}
}

void UW_ClathriQ::OnInternalChanging(int32 BandIndex)
{
	if (IsValid(Settings))
	{
		if (!Settings->MarkPackageDirty())
		{
			UE_LOG(LogSonafridgeEditor, Error, TEXT("UW_ClathriQ::TransferViewModelToPreset: Could not mark package dirty. "));
		}
	}
}

void UW_ClathriQ::OnInternalChanged(int32 BandIndex)
{
	if (IsValid(Settings))
	{
		if (!Settings->MarkPackageDirty())
		{
			UE_LOG(LogSonafridgeEditor, Error, TEXT("UW_ClathriQ::TransferViewModelToPreset: Could not mark package dirty. "));
		}
	}
}

void UW_ClathriQ::OnExternalChanged()
{
	FrequencyResponse->BakeResponse();
}

FReply UW_ClathriQ::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

int32 UW_ClathriQ::NativePaint(const FPaintArgs&        Args,
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

	UW_ClathriQ* MutableThis = const_cast<UW_ClathriQ*>(this);
	FVector2D  Size = AllottedGeometry.GetLocalSize();

	if (Size != LastSize)
	{
		MutableThis->OnSizeChanged(LastSize, Size);
	}

	MutableThis->LastSize = Size;
	return Result;
}


