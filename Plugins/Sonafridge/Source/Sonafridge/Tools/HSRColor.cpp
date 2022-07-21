// Sonafridge 2022

#include "HSRColor.h"

TArray<FLinearColor> FHSRColor::RGBTable =
{
	{ 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, { 0.0000, 0.0000, 0.0000 }, 
	{ 0.1059, 0.0039, 0.0039 }, { 0.0784, 0.0431, 0.0039 }, { 0.0627, 0.0627, 0.0039 }, { 0.0392, 0.0706, 0.0039 }, { 0.0039, 0.0745, 0.0039 }, { 0.0039, 0.0745, 0.0392 }, { 0.0039, 0.0667, 0.0667 }, { 0.0039, 0.0431, 0.0863 }, { 0.0039, 0.0039, 0.2157 }, { 0.0549, 0.0039, 0.1059 }, { 0.0863, 0.0039, 0.0863 }, { 0.0902, 0.0039, 0.0431 }, { 0.1059, 0.0039, 0.0039 }, 
	{ 0.2353, 0.0039, 0.0039 }, { 0.1647, 0.0863, 0.0039 }, { 0.1451, 0.1451, 0.0039 }, { 0.0824, 0.1412, 0.0039 }, { 0.0039, 0.1725, 0.0039 }, { 0.0039, 0.1647, 0.0863 }, { 0.0039, 0.1608, 0.1608 }, { 0.0039, 0.1020, 0.2000 }, { 0.0039, 0.0039, 0.5020 }, { 0.1294, 0.0039, 0.2588 }, { 0.2000, 0.0039, 0.2000 }, { 0.2118, 0.0039, 0.1059 }, { 0.2353, 0.0039, 0.0039 }, 
	{ 0.3961, 0.0039, 0.0039 }, { 0.2863, 0.1451, 0.0039 }, { 0.2392, 0.2392, 0.0039 }, { 0.1333, 0.2588, 0.0039 }, { 0.0039, 0.2784, 0.0039 }, { 0.0039, 0.2745, 0.1412 }, { 0.0039, 0.2627, 0.2627 }, { 0.0039, 0.1686, 0.3373 }, { 0.0039, 0.0039, 0.8196 }, { 0.2196, 0.0039, 0.4392 }, { 0.3333, 0.0039, 0.3333 }, { 0.3686, 0.0039, 0.1843 }, { 0.3961, 0.0039, 0.0039 }, 
	{ 0.5922, 0.0039, 0.0039 }, { 0.4118, 0.2078, 0.0039 }, { 0.3451, 0.3451, 0.0039 }, { 0.1882, 0.3686, 0.0039 }, { 0.0039, 0.4078, 0.0039 }, { 0.0039, 0.4000, 0.2039 }, { 0.0039, 0.3843, 0.3843 }, { 0.0039, 0.2431, 0.4824 }, { 0.0314, 0.0314, 1.0000 }, { 0.3176, 0.0039, 0.6353 }, { 0.4863, 0.0039, 0.4863 }, { 0.5333, 0.0039, 0.2667 }, { 0.5922, 0.0039, 0.0039 }, 
	{ 0.7882, 0.0039, 0.0039 }, { 0.5412, 0.2745, 0.0039 }, { 0.4627, 0.4627, 0.0039 }, { 0.2510, 0.6314, 0.0039 }, { 0.0039, 0.5529, 0.0039 }, { 0.0039, 0.5255, 0.2667 }, { 0.0039, 0.5020, 0.5020 }, { 0.0039, 0.3294, 0.6549 }, { 0.0784, 0.0784, 1.0000 }, { 0.4353, 0.0039, 0.8627 }, { 0.6471, 0.0039, 0.6471 }, { 0.7098, 0.0039, 0.3529 }, { 0.7882, 0.0039, 0.0039 }, 
	{ 1.0000, 0.0078, 0.0078 }, { 0.6941, 0.3490, 0.0039 }, { 0.5843, 0.5843, 0.0039 }, { 0.3216, 0.6314, 0.0039 }, { 0.0039, 0.7020, 0.0039 }, { 0.0039, 0.6667, 0.3373 }, { 0.0039, 0.6353, 0.6353 }, { 0.0039, 0.4196, 0.8353 }, { 0.1373, 0.1373, 1.0000 }, { 0.5216, 0.0392, 1.0000 }, { 0.8275, 0.0039, 0.8275 }, { 0.9020, 0.0039, 0.4471 }, { 1.0000, 0.0078, 0.0078 }, 
	{ 1.0000, 0.0980, 0.0980 }, { 0.8471, 0.4275, 0.0039 }, { 0.7176, 0.7176, 0.0039 }, { 0.3882, 0.7608, 0.0039 }, { 0.0039, 0.8471, 0.0039 }, { 0.0039, 0.8118, 0.4118 }, { 0.0039, 0.7882, 0.7882 }, { 0.0118, 0.5059, 1.0000 }, { 0.1922, 0.1922, 1.0000 }, { 0.5608, 0.1255, 1.0000 }, { 1.0000, 0.0118, 1.0000 }, { 1.0000, 0.0627, 0.5294 }, { 1.0000, 0.0980, 0.0980 }, 
	{ 1.0000, 0.2235, 0.2157 }, { 0.9882, 0.4941, 0.0039 }, { 0.8118, 0.8118, 0.0039 }, { 0.4471, 0.8824, 0.0039 }, { 0.0039, 0.9922, 0.0039 }, { 0.0039, 0.9412, 0.4745 }, { 0.0039, 0.9059, 0.9059 }, { 0.1490, 0.5765, 1.0000 }, { 0.2863, 0.2863, 1.0000 }, { 0.6157, 0.2314, 1.0000 }, { 1.0000, 0.1490, 1.0000 }, { 1.0000, 0.1843, 0.5882 }, { 1.0000, 0.2235, 0.2157 }, 
	{ 1.0000, 0.3569, 0.3569 }, { 1.0000, 0.5882, 0.1765 }, { 0.8667, 0.8667, 0.0039 }, { 0.4902, 0.9686, 0.0039 }, { 0.1765, 1.0000, 0.1765 }, { 0.0941, 1.0000, 0.5490 }, { 0.0039, 0.9961, 0.9961 }, { 0.3020, 0.6510, 1.0000 }, { 0.4118, 0.4118, 1.0000 }, { 0.6824, 0.3686, 1.0000 }, { 1.0000, 0.3098, 1.0000 }, { 1.0000, 0.3294, 0.6627 }, { 1.0000, 0.3569, 0.3569 }, 
	{ 1.0000, 0.4902, 0.4941 }, { 1.0000, 0.6745, 0.3490 }, { 0.9216, 0.9216, 0.0039 }, { 0.5647, 1.0000, 0.1255 }, { 0.3451, 1.0000, 0.3451 }, { 0.2824, 1.0000, 0.6431 }, { 0.2000, 1.0000, 1.0000 }, { 0.4549, 0.7255, 1.0000 }, { 0.5373, 0.5373, 1.0000 }, { 0.7529, 0.5059, 1.0000 }, { 1.0000, 0.4510, 1.0000 }, { 1.0000, 0.4784, 0.7373 }, { 1.0000, 0.4902, 0.4941 }, 
	{ 1.0000, 0.6235, 0.6196 }, { 1.0000, 0.7569, 0.5137 }, { 0.9725, 0.9725, 0.0039 }, { 0.6784, 1.0000, 0.3529 }, { 0.5176, 1.0000, 0.5176 }, { 0.4706, 1.0000, 0.7373 }, { 0.4118, 1.0000, 1.0000 }, { 0.5882, 0.7922, 1.0000 }, { 0.6510, 0.6510, 1.0000 }, { 0.8157, 0.6353, 1.0000 }, { 1.0000, 0.5843, 1.0000 }, { 1.0000, 0.6118, 0.8627 }, { 1.0000, 0.6235, 0.6196 }, 
	{ 1.0000, 0.7451, 0.7412 }, { 1.0000, 0.8353, 0.6667 }, { 1.0000, 1.0000, 0.1451 }, { 0.7843, 1.0000, 0.5647 }, { 0.6706, 1.0000, 0.6706 }, { 0.6353, 1.0000, 0.8196 }, { 0.5961, 1.0000, 1.0000 }, { 0.7176, 0.8588, 1.0000 }, { 0.7608, 0.7608, 1.0000 }, { 0.8745, 0.7490, 1.0000 }, { 1.0000, 0.7255, 1.0000 }, { 1.0000, 0.7294, 0.8627 }, { 1.0000, 0.7451, 0.7412 }, 
	{ 1.0000, 0.8431, 0.8471 }, { 1.0000, 0.9020, 0.8039 }, { 1.0000, 1.0000, 0.5020 }, { 0.8667, 1.0000, 0.7294 }, { 0.8039, 1.0000, 0.8039 }, { 0.7804, 1.0000, 0.8941 }, { 0.7686, 1.0000, 1.0000 }, { 0.8314, 0.9137, 1.0000 }, { 0.8588, 0.8588, 1.0000 }, { 0.9255, 0.8510, 1.0000 }, { 1.0000, 0.8235, 1.0000 }, { 1.0000, 0.8392, 0.9176 }, { 1.0000, 0.8431, 0.8471 }, 
	{ 1.0000, 0.9294, 0.9294 }, { 1.0000, 0.9608, 0.9176 }, { 1.0000, 1.0000, 0.7882 }, { 0.9529, 1.0000, 0.8980 }, { 0.9176, 1.0000, 0.9176 }, { 0.9137, 1.0000, 0.9608 }, { 0.8980, 1.0000, 1.0000 }, { 0.9294, 0.9647, 1.0000 }, { 0.9373, 0.9373, 1.0000 }, { 0.9647, 0.9333, 1.0000 }, { 1.0000, 0.9294, 1.0000 }, { 1.0000, 0.9294, 0.9647 }, { 1.0000, 0.9294, 0.9294 }, 
	{ 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 }, { 1.0000, 1.0000, 1.0000 },  
};

FLinearColor FHSRColor::ToLinearRGBFromTable(float Hue, float Saturation, float Radiance)
{
	const float TableHue = 12.f * Hue;
	const float HueIndexA = FMath::FloorToFloat(TableHue);
	const float HueIndexB = FMath::CeilToFloat(TableHue);

	const float TableRadiance = 15.f * Radiance;
	const float RadianceIndexC = FMath::FloorToFloat(TableRadiance);
	const float RadianceIndexD = FMath::CeilToFloat(TableRadiance);

	float HueAlpha = 0.f;
	if (HueIndexA != HueIndexB)
	{
		HueAlpha = (TableHue - HueIndexA) / (HueIndexB - HueIndexA);
	}
	
	float RadianceAlpha = 0.f;
	if (RadianceIndexC != RadianceIndexD)
	{
		RadianceAlpha = (TableRadiance - RadianceIndexC) / (RadianceIndexD - RadianceIndexC);
	}

	const int32 IndexAC = static_cast<int32>(RadianceIndexC) * 13 + static_cast<int32>(HueIndexA); 
	const int32 IndexAD = static_cast<int32>(RadianceIndexD) * 13 + static_cast<int32>(HueIndexA); 
	const int32 IndexBC = static_cast<int32>(RadianceIndexC) * 13 + static_cast<int32>(HueIndexB); 
	const int32 IndexBD = static_cast<int32>(RadianceIndexD) * 13 + static_cast<int32>(HueIndexB); 

	const float LinearRadiance = FMath::Pow(Radiance, 2.2f);
	const FLinearColor GrayscaleColor = FLinearColor(LinearRadiance, LinearRadiance, LinearRadiance); 
	const FLinearColor HRColor = FMath::Lerp
	(
		FMath::Lerp(RGBTable[IndexAC], RGBTable[IndexBC], HueAlpha),
		FMath::Lerp(RGBTable[IndexAD], RGBTable[IndexBD], HueAlpha),
		RadianceAlpha
	);

	const FLinearColor Result = FMath::Lerp(GrayscaleColor, HRColor, Saturation);
	return Result;
}

FLinearColor FHSRColor::ToLinearRGBAtRuntime(float Hue, float Saturation, float Radiance)
{
	// Radiance should be linear (~.218 is the center)
	Radiance = FMath::Pow(Radiance, 2.2f); 
	
	constexpr float HueRed =     .0000f;
	constexpr float HueYellow =  .1667f;
	constexpr float HueGreen =   .3333f;
	constexpr float HueCyan =    .5000f;
	constexpr float HueBlue =    .6667f;
	constexpr float HueMagenta = .8333f;
	constexpr float HueRedAgain = 1.00f;

	// Weighted average luminocity.
	// Because truly luminosity retention will reduce saturation too much. 
	constexpr float LuminocityRed =     .1080f; // .0707f; // .30f;
	constexpr float LuminocityYellow =  .6700f; // .7739f; // .89f;
	constexpr float LuminocityGreen =   .3132f; // .3132f; // .59f;
	constexpr float LuminocityCyan =    .4420f; // .4420f; // .69f;
	constexpr float LuminocityBlue =    .0340f; // .0078f; // .11f;
	constexpr float LuminocityMagenta = .1406f; // .1406f; // .41f;

	FLinearColor Result;

	// Unset luminocity is half of linear space. 
	float Luminocity = .2176f;
	
	// Hue to RGB
	if (Hue >= HueRed && Hue < HueYellow)
	{
		const float HueT = (Hue - HueRed) / (HueYellow - HueRed);
		Result = FLinearColor(1.f, HueT, 0.f);
		Luminocity = FMath::Lerp(LuminocityRed, LuminocityYellow, HueT);
	}
	else if (Hue >= HueYellow && Hue < HueGreen)
	{
		const float HueT = (Hue - HueYellow) / (HueGreen - HueYellow);
		Result = FLinearColor(1.f - HueT, 1.f, 0.f);
		Luminocity = FMath::Lerp(LuminocityYellow, LuminocityGreen, HueT);
	}
	else if (Hue >= HueGreen && Hue < HueCyan)
	{
		const float HueT = (Hue - HueGreen) / (HueCyan - HueGreen);
		Result = FLinearColor(0.f, 1.f, HueT);
		Luminocity = FMath::Lerp(LuminocityGreen, LuminocityCyan, HueT);
	}
	else if (Hue >= HueCyan && Hue < HueBlue)
	{
		const float HueT = (Hue - HueCyan) / (HueBlue - HueCyan);
		Result = FLinearColor(0.f, 1.f - HueT, 1.f);
		Luminocity = FMath::Lerp(LuminocityCyan, LuminocityBlue, HueT);
	}
	else if (Hue >= HueBlue && Hue < HueMagenta)
	{
		const float HueT = (Hue - HueBlue) / (HueMagenta - HueBlue);
		Result = FLinearColor(HueT, 0.f, 1.f);
		Luminocity = FMath::Lerp(LuminocityBlue, LuminocityMagenta, HueT);
	}
	else if (Hue >= HueMagenta && Hue <= HueRedAgain)
	{
		const float HueT = (Hue - HueMagenta) / (HueRedAgain - HueMagenta);
		Result = FLinearColor(1.f, 0.f, 1.f - HueT);
		Luminocity = FMath::Lerp(LuminocityMagenta, LuminocityRed, HueT);
	}

	// H&S to RGB
	FLinearColor Gr = FLinearColor(Luminocity, Luminocity, Luminocity);
	Result = FLinearColor(FMath::Lerp(Gr.R, Result.R, Saturation),
	                      FMath::Lerp(Gr.G, Result.G, Saturation),
	                      FMath::Lerp(Gr.B, Result.B, Saturation));

	// HSL to RGB
	if (Radiance < Luminocity)
	{
		const float T = Radiance / Luminocity;
		Result = FLinearColor(Result.R * T, Result.G * T, Result.B * T);
	}
	else
	{
		const float T = (Radiance - Luminocity) / (1.f - Luminocity);
		Result = FLinearColor(FMath::Lerp(Result.R, 1.f, T),
		                      FMath::Lerp(Result.G, 1.f, T),
		                      FMath::Lerp(Result.B, 1.f, T));
	}

	return Result;
}
