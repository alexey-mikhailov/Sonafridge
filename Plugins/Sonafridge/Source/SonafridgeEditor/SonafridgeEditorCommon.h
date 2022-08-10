// Sonafridge 2022

#pragma once

namespace SonafridgeEditor
{
	enum class ESignMode { NegativeOnly, Always };
}

UENUM()
enum class EBandPopupFocusMode : uint8
{
	None,
	Frequency,
	Amount,
	Quality,
	Makeup
};

UENUM()
enum class EBandPopupType : uint8
{
	CutLow,
	CutHigh,
	AttLow,
	AttHigh,
	AttBand,
	Notch,
};

FText FloatToText(float                       InValue,
				  int32                       NumDigits,
				  SonafridgeEditor::ESignMode SignMode = SonafridgeEditor::ESignMode::NegativeOnly);
