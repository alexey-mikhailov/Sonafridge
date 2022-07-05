// Sonafridge 2022

#include "EQBand.h"

#include "Sonafridge/MathTools.h"

void FEQBand::Init(float InSampleRate)
{
	SampleRate = InSampleRate;
	bIsInitialized = true;
}

void FEQBand::SetType(EBandType Value)
{
	LogIfUninitialized();
	Type = Value;
	Recalculate();
}

void FEQBand::SetIsEnabled(bool bInValue)
{
	LogIfUninitialized();
	bIsEnabled = bInValue;
	Recalculate();
}

void FEQBand::SetFrequency(float Value)
{
	LogIfUninitialized();
	Frequency = Value;
	Recalculate();
}

void FEQBand::SetQuality(float Value)
{
	LogIfUninitialized();
	Quality = Value;
	Recalculate();
}

void FEQBand::SetAmountDb(float Value)
{
	LogIfUninitialized();
	AmountDb = Value;
	Recalculate();
}

void FEQBand::SetLoudCompDb(float Value)
{
	LogIfUninitialized();
	LoudCompDb = Value;
	LoudCompCoef = MathLogTool::VigesibelToLinear(LoudCompDb);
	Recalculate();
}

float FEQBand::Dtft(float InFrequency) const
{
	if (bIsEnabled)
	{
		auto InOmega = 2.f * PI * InFrequency / SampleRate;
		FEcn Angle0 = FEcn(InOmega)^-0;
		FEcn Angle1 = FEcn(InOmega)^-1;
		FEcn Angle2 = FEcn(InOmega)^-2;

		return LoudCompCoef *
		(
			(B0 * Angle0 + B1 * Angle1 + B2 * Angle2) /
			(A0 * Angle0 + A1 * Angle1 + A2 * Angle2)
		)
		.Radius;
	}

	return 1.f;
}

EBandType FEQBand::GetType() const
{
	LogIfUninitialized();
	return Type;
}

bool FEQBand::GetIsEnabled() const
{
	LogIfUninitialized();
	return bIsEnabled;
}

float FEQBand::GetFrequency() const
{
	LogIfUninitialized();
	return Frequency;
}

float FEQBand::GetQuality() const
{
	LogIfUninitialized();
	return Quality;
}

float FEQBand::GetAmountDb() const
{
	LogIfUninitialized();
	return AmountDb;
}

float FEQBand::GetLoudCompDb() const
{
	LogIfUninitialized();
	return LoudCompDb;
}

void FEQBand::Recalculate()
{
	Omega = 2.f * PI * Frequency / SampleRate;

	if (Type == EBandType::LowCut)
	{
		float Sn = FMath::Sin(Omega);
		float Cs = FMath::Cos(Omega);
		Alpha = .5f * Sn / Quality;

		A0 = 1.f + Alpha;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha;
		B0 = .5f * (1.f + Cs);
		B1 = - 1.f - Cs;
		B2 = .5f * (1.f + Cs);
	}
	else if (Type == EBandType::HighCut)
	{
		float Sn = FMath::Sin(Omega);
		float Cs = FMath::Cos(Omega);
		Alpha = .5f * Sn / Quality;

		A0 = 1.f + Alpha;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha;
		B0 = .5f * (1.f - Cs);
		B1 = 1.f - Cs;
		B2 = .5f * (1.f - Cs);
	}
	else if (Type == EBandType::LowShelf)
	{
		float GainSqrt = FMath::Pow(10, AmountDb / 40.f);
		float Sn = FMath::Sin(Omega);
		float Cs = FMath::Cos(Omega);
		// Alpha = .5f * Sn * FMath::Sqrt((GainSqrt + 1.f / GainSqrt) * (1.f / Quality - 1.f) + 2.f);
		Alpha = .5f * Sn / Quality;
		float DoubleAlphaGain = 2.f * Alpha * FMath::Sqrt(GainSqrt);

		A0 = GainSqrt + 1.f + Cs * (GainSqrt - 1.f) + DoubleAlphaGain;
		A1 = -2.f * (GainSqrt - 1.f + Cs * (GainSqrt + 1.f));
		A2 = GainSqrt + 1.f + Cs * (GainSqrt - 1.f) - DoubleAlphaGain;
		B0 = GainSqrt * (GainSqrt + 1.f - Cs * (GainSqrt - 1.f) + DoubleAlphaGain);
		B1 = +2.f * GainSqrt * (GainSqrt - 1.f - Cs * (GainSqrt + 1.f));
		B2 = GainSqrt * (GainSqrt + 1.f - Cs * (GainSqrt - 1.f) - DoubleAlphaGain);
	}
	else if (Type == EBandType::HighShelf)
	{
		float GainSqrt = FMath::Pow(10, AmountDb / 40.f);
		float Sn = FMath::Sin(Omega);
		float Cs = FMath::Cos(Omega);
		Alpha = .5f * Sn / Quality;
		float DoubleAlphaGain = 2.f * Alpha * FMath::Sqrt(GainSqrt);

		A0 = GainSqrt + 1.f - Cs * (GainSqrt - 1.f) + DoubleAlphaGain;
		A1 = +2.f * (GainSqrt - 1.f - Cs * (GainSqrt + 1.f));
		A2 = GainSqrt + 1.f - Cs * (GainSqrt - 1.f) - DoubleAlphaGain;
		B0 = GainSqrt * (GainSqrt + 1.f + Cs * (GainSqrt - 1.f) + DoubleAlphaGain);
		B1 = -2.f * GainSqrt * (GainSqrt - 1.f + Cs * (GainSqrt + 1.f));
		B2 = GainSqrt * (GainSqrt + 1.f + Cs * (GainSqrt - 1.f) - DoubleAlphaGain);
	}
	else if (Type == EBandType::BandPass)
	{
		float Sn = FMath::Sin(Omega);
		float Cs = FMath::Cos(Omega);
		Alpha = .5f * Sn / Quality;

		A0 = 1.f + Alpha;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha;
		B0 = +Quality * Alpha;
		B1 = 0.f;
		B2 = -Quality * Alpha;
	}
	else if (Type == EBandType::BandCut)
	{
		float GainSqrt = FMath::Pow(10, AmountDb / 40.f);
		float Sn = FMath::Sin(Omega);
		float Cs = FMath::Cos(Omega);
		Alpha = .5f * Sn / Quality;

		A0 = 1.f + Alpha / GainSqrt;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha / GainSqrt;
		B0 = 1.f + Alpha * GainSqrt;
		B1 = -2.f * Cs;
		B2 = 1.f - Alpha * GainSqrt;
	}
	else if (Type == EBandType::Notch)
	{
		float Sn = FMath::Sin(Omega);
		float Cs = FMath::Cos(Omega);
		Alpha = .5f * Sn / Quality;

		A0 = 1.f + Alpha;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha;
		B0 = 1.f;
		B1 = -2.f * Cs;
		B2 = 1.f;
	}
}

