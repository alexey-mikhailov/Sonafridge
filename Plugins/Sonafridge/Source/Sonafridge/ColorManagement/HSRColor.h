// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"

/// <summary>
/// Hue-Saturaton-Radiance color model
/// </summary>
/// <remarks>
/// Radiance is amount of percepted light, where 0.0 is black, 1.0 is white, 0.5 is gray equivalent of saturated color.
/// CIE LCh 2003 does not offer this behavior because: 
/// 1. Their L = 1.f may be white or saturated cyan.
/// 2. Their L = 0.f should be black but it may be dark red or dark blue.
/// </remarks>
struct SONAFRIDGE_API FHSRColor
{
	static FLinearColor ToLinearRGBFromTable(float Hue, float Saturation, float Radiance);
	static FLinearColor ToLinearRGBAtRuntime(float Hue, float Saturation, float Radiance);

private:
	FHSRColor() = delete;
	static TArray<FLinearColor> RGBTable; 
};

#define HSR(Hue, Saturation, Radiance) FHSRColor::ToLinearRGBFromTable(Hue, Saturation, Radiance)  

