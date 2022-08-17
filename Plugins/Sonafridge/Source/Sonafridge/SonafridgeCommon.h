// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSonafridge, Error, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSonafridgeEditor, Error, All);

constexpr float MaxFilterFreq = 18000.f;
constexpr float MinFilterFreq = 80.f;
constexpr int32 MaxFilterChannels = 8;

namespace Sonafridge
{
	enum class EChannelId
	{
		Left, Right,
	};
}

UENUM()
enum class EEQBandType : uint8
{
	None UMETA(DisplayName = "No filter"),
	CutLowFast UMETA(DisplayName = "Low cut (Fast)"),
	CutLowButterworth UMETA(DisplayName = "Low cut (Butterworth)"),
	CutHighFast UMETA(DisplayName = "High cut (Fast)"),
	CutHighButterworth UMETA(DisplayName = "High cut (Butterworth)"),
	PassBand UMETA(DisplayName = "Band pass"),
	PassAll UMETA(DisplayName = "All pass (phase shift)"),
	AttLow UMETA(DisplayName = "Low shelf"),
	AttHigh UMETA(DisplayName = "High shelf"),
	AttBand UMETA(DisplayName = "Bell"),
	Notch UMETA(DisplayName = "Notch"),
};

