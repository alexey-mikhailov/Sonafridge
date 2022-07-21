// Sonafridge 2022

#include "W_Clathrispace.h"
#include "Sonafridge/Attenuator/Clathrispace.h"
#include "Sonafridge/SonafridgeCommon.h"
#include "Components/CanvasPanelSlot.h"
#include "AudioDevice.h"

void UW_Clathrispace::Init(UClathrispaceSettings* InSettings)
{
	if (IsValid(InSettings))
	{
		Settings = InSettings;
	}
	else
	{
		UE_LOG(LogSonafridgeEditor, Error, TEXT("UW_Clathrispace::Init: InSettings argument is not valid. "));
	}
}

void UW_Clathrispace::NativeConstruct()
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

	if (IsValid(Settings))
	{
		// Initialize from preset


	}
}

void UW_Clathrispace::OnSizeChanged(const FVector2D& OldSize, const FVector2D& NewSize)
{
}

int32 UW_Clathrispace::NativePaint(const FPaintArgs&        Args,
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

	UW_Clathrispace* MutableThis = const_cast<UW_Clathrispace*>(this);
	FVector2D  Size = AllottedGeometry.GetLocalSize();

	if (Size != LastSize)
	{
		MutableThis->OnSizeChanged(LastSize, Size);
	}

	MutableThis->LastSize = Size;
	return Result;
}

