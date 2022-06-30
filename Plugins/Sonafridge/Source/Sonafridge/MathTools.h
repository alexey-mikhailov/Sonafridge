// Sonafridge 2022

#pragma once

#include <complex>

struct FEcn;

/// <summary>
/// Complex number with rectangular form.
///	Use it when sum is more relevant op than rotation. 
///	Scale may be produced a little bit slower than in rectangular version. 
/// </summary>
struct SONAFRIDGE_API FRcn final
{
	static FRcn FromExponential(float Radius, float Angle);
	static FRcn FromAngle(float Angle);

	FRcn() = default;
	FRcn(float InReal);
	FRcn(float InReal, float InImag);

	FEcn ToExponential();

	float GetRadius() const;
	float GetAngle() const;

	FRcn ToThe(float Logarithm);

	FRcn operator+(float InReal);
	FRcn operator+(const FRcn& Other);
	FRcn operator*(float Scale);
	FRcn operator*(const FRcn& Other);
	FRcn operator/(float Scale);
	FRcn operator/(const FRcn& Other);
	FRcn operator^(float Logarithm);

	FRcn& operator=(const FRcn& Copy);
	FRcn& operator+=(float InReal);
	FRcn& operator+=(const FRcn& Other);
	FRcn& operator*=(float Scale);
	FRcn& operator/=(float Scale);

	friend FRcn operator*(float Scale, const FRcn& Value)
	{
		return { Scale * Value.Real, Scale * Value.Imag };
	}

	float Real = 1.f;
	float Imag = 0.f;
};


/// <summary>
/// Complex number with exponential form. 
///	Use it when sum is less relevant op than rotation.
///	Scale may be produced a little bit faster than in rectangular version. 
/// </summary>
struct SONAFRIDGE_API FEcn final
{
	static FEcn FromRectangular(float InReal, float InImag);

	FEcn() = default;
	FEcn(float InAngle);
	FEcn(float InRadius, float InAngle);
	FEcn(const FEcn& From);

	void Empower(float Logarithm);
	FEcn ToThe(float Logarithm);
	FRcn ToRectangular();

	FORCEINLINE float GetReal() const { return Radius * FMath::Cos(Angle); }
	FORCEINLINE float GetImag() const { return Radius * FMath::Sin(Angle); }

	FEcn operator+(float InReal);
	FEcn operator+(const FEcn& Other);
	FEcn operator*(float Scale);
	FEcn operator*(const FEcn& Other);
	FEcn operator/(float Scale);
	FEcn operator/(const FEcn& Other);
	FEcn operator^(float Logarithm);

	FEcn& operator=(const FEcn& Copy);
	FEcn& operator*=(float Scale);
	FEcn& operator*=(const FEcn& Other);
	FEcn& operator/=(float Scale);
	FEcn& operator/=(const FEcn& Other);
	FEcn& operator^=(float Logarithm);

	friend FEcn operator*(float Scale, const FEcn& Value)
	{
		return { Scale * Value.Radius, Value.Angle };
	}

	float Radius = 0.f;
	float Angle = 0.f;
};


struct MathLogTool final
{
	/// <summary>
	/// Converts squared linear value to decibels. <br>
	/// Useful for conversion from power ratio to log10/5 scale. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A quintabel is 5 times smaller than a bel, where bel is a base-10 logarithm unit.  <br>
	///	</remarks>
	FORCEINLINE float LinearToQuintabel(float Input)
	{
		return 5.f * FMath::LogX(10.f, Input);
	}

	/// <summary>
	/// Converts decibels to squared linear value. <br>
	/// Useful for conversion from log10/5 scale to power ratio. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A quintabel is 5 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE float QuintabelToLinear(float Input)
	{
		return FMath::Pow(10.f, .2f * Input);
	}

	/// <summary>
	/// Converts linear value to decibels. <br>
	/// Useful for conversion from power ratio to log10/10 scale. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float LinearToDecibel(float Input)
	{
		return 10.f * FMath::LogX(10.f, Input);
	}

	/// <summary>
	/// Converts decibels to linear value. <br>
	/// Useful for conversion from log10/10 scale to power ratio. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float DecibelToLinear(float Input)
	{
		return FMath::Pow(10.f, .1f * Input);
	}

	/// <summary>
	/// Converts square root of linear value to decibels. <br>
	/// Useful for conversion from power ratio to log10/20 scale. <br>
	/// Useful for conversion from amplitude ratio to log10/10 scale. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A vigesibel is 20 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float LinearToVigesibel(float Input)
	{
		return 20.f * FMath::LogX(10.f, Input);
	}

	/// <summary>
	/// Converts decibels to square root of linear value. <br>
	/// Useful for conversion from log10/20 scale to power ratio. <br>
	/// Useful for conversion from log10/10 scale to amplitude ratio. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A vigesibel is 20 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float VigesibelToLinear(float Input)
	{
		return FMath::Pow(10.f, .05f * Input);
	}

	/// <summary>
	/// Converts 4th root of linear value to decibels. <br>
	/// Useful for conversion from power ratio to log10/40 scale. <br>
	/// Useful for conversion from amplitude ratio square root to log10/10 scale.
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A quadragesibel is 40 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float LinearToQuadragesibel(float Input)
	{
		return 40.f * FMath::LogX(10.f, Input);
	}

	/// <summary>
	/// Converts decibels to 4th root of linear value. <br>
	/// Useful for conversion from log10/40 scale to power ratio. <br>
	/// Useful for conversion from log10/10 scale to square roots of amplitude ratio. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A quadragesibel is 40 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float QuadragesibelToLinear(float Input)
	{
		return FMath::Pow(10.f, 0.025f * Input);
	}

	/// <summary>
	/// Converts linear value to tribels. <br>
	/// Useful for conversion from linear scale in range [1, 1K] to log10 scale in range [0, 1]. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A tribel is 3 times greater than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float LinearToTribel(float Input)
	{
		return FMath::LogX(10.f, Input) / 3.f;
	}

	/// <summary>
	/// Converts tribels to linear value. <br>
	/// Useful for conversion from log10 scale in range [0, 1] to linear scale in range [1, 1K].
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A tribel is 3 times greater than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float TribelToLinear(float Input)
	{
		return FMath::Pow(10.f, 3.f * Input);
	}

	/// <summary>
	/// Converts twentieths of linear value to tribels. <br>
	/// Useful for conversion from linear scale in range [20, 20K] to log10 scale in range [0, 1]. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A tribel is 3 times greater than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float TwentiethsToTribel(float Input)
	{
		return FMath::LogX(10.f, 0.05f * Input) / 3.f;
	}

	/// <summary>
	/// Converts tribels to twentieths of linear value. <br>
	/// Useful for conversion from log10 scale in range [0, 1] to linear scale in range [20, 20K]. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A tribel is 3 times greater than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float TribelToTwentieths(float Input)
	{
		return 20.f * FMath::Pow(10.f, 3.f * Input);
	}

	/// <summary>
	/// Converts linear value to hexabels. <br>
	/// Useful for conversion from linear scale in range [1, 1M] to log10 scale in range [0, 1]. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A hexabel is 6 times greater than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float LinearToHexabel(float Input)
	{
		return FMath::LogX(10.f, Input) / 6.f;
	}

	/// <summary>
	/// Converts hexabels to linear value. <br>
	/// Useful for conversion from log10 scale in range [0, 1] to linear scale in range [1, 1M]. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br> 
	///	A hexabel is 6 times greater than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float HexabelToLinear(float Input)
	{
		return FMath::Pow(10.f, 6.f * Input);
	}

	/// <summary>
	/// Converts thousands of linear value to hexabels. <br>
	/// Useful for conversion from linear scale in range [1m, 1K] to log10 scale in range [0, 1]. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A hexabel is 6 times greater than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float ThousandsToHexabel(float Input)
	{
		return FMath::LogX(10.f, 1000.f * Input) / 6.f;
	}

	/// <summary>
	/// Converts hexabels to thousands of linear value. <br>
	/// Useful for conversion from log10 scale in range [0, 1] to linear scale in range [1m, 1K]. <br>
	/// </summary>
	///	<remarks>
	///	A decibel is 10 times smaller than a bel, where bel is a base-10 logarithm unit. <br>
	///	A hexabel is 6 times greater than a bel, where bel is a base-10 logarithm unit. <br>
	///	</remarks>
	FORCEINLINE static float HexabelToThousands(float Input)
	{
		return .001f * FMath::Pow(10.f, 6.f * Input);
	}
};


