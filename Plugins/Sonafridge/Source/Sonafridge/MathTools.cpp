#include "MathTools.h"

FRcn FRcn::FromExponential(float Radius, float Angle)
{
	return
	{
		Radius * FMath::Cos(Angle),
		Radius * FMath::Sin(Angle)
	};
}

FRcn FRcn::FromAngle(float Angle)
{
	return
	{
		FMath::Cos(Angle),
		FMath::Sin(Angle)
	};
}

FRcn::FRcn(float InReal)
	: Real(InReal)
{
}

FRcn::FRcn(float InReal, float InImag)
	: Real(InReal)
	, Imag(InImag)
{
}

FEcn FRcn::ToExponential()
{
	return
	{
		FMath::Sqrt(Real * Real + Imag * Imag),
		FMath::Atan2(Imag, Real)
	};
}

float FRcn::GetRadius() const
{
	return FMath::Sqrt(Real * Real + Imag * Imag);
}

float FRcn::GetAngle() const
{
	return FMath::Atan2(Imag, Real);
}

FRcn FRcn::ToThe(float Logarithm)
{
	return
	{
		FMath::Sqrt(Real * Real + Imag * Imag) * FMath::Cos(GetAngle() * Logarithm),
		FMath::Sqrt(Real * Real + Imag * Imag) * FMath::Sin(GetAngle() * Logarithm)
	};
}

FRcn FRcn::operator+(float InReal)
{
	return { Real + InReal, Imag };
}

FRcn FRcn::operator+(const FRcn& Other)
{
	return { Real + Other.Real, Imag + Other.Imag };
}

FRcn FRcn::operator*(float Scale)
{
	return { Scale * Real, Scale * Imag };
}

FRcn FRcn::operator*(const FRcn& Other)
{
	return
	{
		Real * Other.Real - Imag * Other.Imag,
		Real * Other.Imag - Imag * Other.Real
	};
}

FRcn FRcn::operator/(float Scale)
{
	return { Scale / Real, Scale / Imag };
}

FRcn FRcn::operator/(const FRcn& Other)
{
	return
	{
		(Real * Other.Real + Imag * Other.Imag) / (Other.Real * Other.Real + Other.Imag * Other.Imag),
		(Imag * Other.Real - Real * Other.Imag) / (Other.Real * Other.Real + Other.Imag * Other.Imag)
	};
}

FRcn FRcn::operator^(float Logarithm)
{
	return
	{
		FMath::Sqrt(Real * Real + Imag * Imag) * FMath::Cos(GetAngle() * Logarithm),
		FMath::Sqrt(Real * Real + Imag * Imag) * FMath::Sin(GetAngle() * Logarithm)
	};
}

FRcn& FRcn::operator=(const FRcn& Copy)
{
	Real = Copy.Real;
	Imag = Copy.Imag;
	return *this;
}

FRcn& FRcn::operator+=(float InReal)
{
	Real += InReal;
	return *this;
}

FRcn& FRcn::operator+=(const FRcn& Other)
{
	Real += Other.Real;
	Imag += Other.Imag;
	return *this;
}

FRcn& FRcn::operator*=(float Scale)
{
	Real *= Scale;
	Imag *= Scale;
	return *this;
}

FRcn& FRcn::operator/=(float Scale)
{
	Real /= Scale;
	Imag /= Scale;
	return *this;
}


FEcn FEcn::FromRectangular(float InReal, float InImag)
{
	float Radius = FMath::Sqrt(InReal * InReal + InImag * InImag);
	return { Radius, FMath::Atan2(InImag, InReal) };
}

FEcn::FEcn(float InAngle)
	: Radius(1.f)
	, Angle(InAngle)
{
}

FEcn::FEcn(float InRadius, 
           float InAngle)
	: Radius(InRadius)
	, Angle(InAngle)
{
}

FEcn::FEcn(const FEcn& From)
	: Radius(From.Radius)
	, Angle(From.Angle)
{
}

void FEcn::Empower(float Logarithm)
{
	Angle *= Logarithm;
}

FEcn FEcn::ToThe(float Logarithm)
{
	return { Angle * Logarithm };
}

FRcn FEcn::ToRectangular()
{
	return
	{
		Radius * FMath::Cos(Angle),
		Radius * FMath::Sin(Angle)
	};
}

FEcn FEcn::operator+(float InReal)
{
	return FromRectangular(GetReal() + InReal, GetImag());
}

FEcn FEcn::operator+(const FEcn& Other)
{
	return FromRectangular(GetReal() + Other.GetReal(),
	                       GetImag() + Other.GetImag());
}

FEcn FEcn::operator*(float Scale)
{
	return { Radius * Scale, Angle };
}

FEcn FEcn::operator*(const FEcn& Other)
{
	return { Radius * Other.Radius, Angle + Other.Angle };
}

FEcn FEcn::operator/(float Scale)
{
	return { Radius / Scale, Angle };
}

FEcn FEcn::operator/(const FEcn& Other)
{
	return { Radius / Other.Radius, Angle - Other.Angle };
}

FEcn FEcn::operator^(float Logarithm)
{
	return { FMath::Pow(Radius, Logarithm), Angle * Logarithm };
}

FEcn& FEcn::operator=(const FEcn& Copy)
{
	Radius = Copy.Radius;
	Angle = Copy.Angle;
	return *this;
}

FEcn& FEcn::operator*=(float Scale)
{
	Radius *= Scale;
	return *this;
}

FEcn& FEcn::operator*=(const FEcn& Other)
{
	Radius *= Other.Radius;
	Angle += Other.Angle;
	return *this;
}

FEcn& FEcn::operator/=(float Scale)
{
	Radius /= Scale;
	return *this;
}

FEcn& FEcn::operator/=(const FEcn& Other)
{
	Radius /= Other.Radius;
	Angle -= Other.Angle;
	return *this;
}

FEcn& FEcn::operator^=(float Logarithm)
{
	Radius = FMath::Pow(Radius, Logarithm);
	Angle *= Logarithm;
	return *this;
}

