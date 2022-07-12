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
	if (Type == EBandType::LowCutFast)
	{
		const float Omega = 2.f * PI * Frequency / SampleRate;
		const float Sn = FMath::Sin(Omega);
		const float Cs = FMath::Cos(Omega);
		const float Alpha = .5f * Sn / Quality;

		A0 = 1.f + Alpha;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha;
		B0 = .5f * (1.f + Cs);
		B1 = - 1.f - Cs;
		B2 = .5f * (1.f + Cs);
	}
	else if (Type == EBandType::LowCutButterworth)
	{
		const float Lambda = FMath::Tan(PI * Frequency / SampleRate);
		const float LambdaScaled = UE_SQRT_2 * Lambda / Quality;
		const float LambdaSq = Lambda * Lambda;

		A0 = 1.f;
		A1 = 2.f * (LambdaSq - 1.f) / (1.f + LambdaScaled + LambdaSq);
		A2 = (1.f - LambdaScaled + LambdaSq) / (1.f + LambdaScaled + LambdaSq);
		B0 = 1.f / (1.f + LambdaScaled + LambdaSq);
		B1 = -2.f * B0;
		B2 = B0;
	}
	else if (Type == EBandType::HighCutFast)
	{
		const float Omega = 2.f * PI * Frequency / SampleRate;
		const float Sn = FMath::Sin(Omega);
		const float Cs = FMath::Cos(Omega);
		const float Alpha = .5f * Sn / Quality;

		A0 = 1.f + Alpha;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha;
		B0 = .5f * (1.f - Cs);
		B1 = 1.f - Cs;
		B2 = .5f * (1.f - Cs);
	}
	else if (Type == EBandType::HighCutButterworth)
	{
		const float Lambda = 1.f / FMath::Tan(PI * Frequency / SampleRate);
		const float LambdaScaled = UE_SQRT_2 * Lambda / Quality;
		const float LambdaSq = Lambda * Lambda;

		A0 = 1.f;
		A1 = 2.f * (1.f - LambdaSq) / (1.f + LambdaScaled + LambdaSq);
		A2 = (1.f - LambdaScaled + LambdaSq) / (1.f + LambdaScaled + LambdaSq);
		B0 = 1.f / (1.f + LambdaScaled + LambdaSq);
		B1 = 2.f * B0;
		B2 = B0;
	}
	else if (Type == EBandType::LowShelf)
	{
		const float GainSqrt = FMath::Pow(10, AmountDb / 40.f);
		const float Omega = 2.f * PI * Frequency / SampleRate;
		const float Sn = FMath::Sin(Omega);
		const float Cs = FMath::Cos(Omega);
		const float Alpha = .5f * Sn / Quality;
		const float DoubleAlphaGain = 2.f * Alpha * FMath::Sqrt(GainSqrt);

		A0 = GainSqrt + 1.f + Cs * (GainSqrt - 1.f) + DoubleAlphaGain;
		A1 = -2.f * (GainSqrt - 1.f + Cs * (GainSqrt + 1.f));
		A2 = GainSqrt + 1.f + Cs * (GainSqrt - 1.f) - DoubleAlphaGain;
		B0 = GainSqrt * (GainSqrt + 1.f - Cs * (GainSqrt - 1.f) + DoubleAlphaGain);
		B1 = +2.f * GainSqrt * (GainSqrt - 1.f - Cs * (GainSqrt + 1.f));
		B2 = GainSqrt * (GainSqrt + 1.f - Cs * (GainSqrt - 1.f) - DoubleAlphaGain);
	}
	else if (Type == EBandType::HighShelf)
	{
		const float GainSqrt = FMath::Pow(10, AmountDb / 40.f);
		const float Omega = 2.f * PI * Frequency / SampleRate;
		const float Sn = FMath::Sin(Omega);
		const float Cs = FMath::Cos(Omega);
		const float Alpha = .5f * Sn / Quality;
		const float DoubleAlphaGain = 2.f * Alpha * FMath::Sqrt(GainSqrt);

		A0 = GainSqrt + 1.f - Cs * (GainSqrt - 1.f) + DoubleAlphaGain;
		A1 = +2.f * (GainSqrt - 1.f - Cs * (GainSqrt + 1.f));
		A2 = GainSqrt + 1.f - Cs * (GainSqrt - 1.f) - DoubleAlphaGain;
		B0 = GainSqrt * (GainSqrt + 1.f + Cs * (GainSqrt - 1.f) + DoubleAlphaGain);
		B1 = -2.f * GainSqrt * (GainSqrt - 1.f + Cs * (GainSqrt + 1.f));
		B2 = GainSqrt * (GainSqrt + 1.f + Cs * (GainSqrt - 1.f) - DoubleAlphaGain);
	}
	else if (Type == EBandType::BandPass)
	{
		const float Omega = 2.f * PI * Frequency / SampleRate;
		const float Sn = FMath::Sin(Omega);
		const float Cs = FMath::Cos(Omega);
		const float Alpha = .5f * Sn / Quality;

		A0 = 1.f + Alpha;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha;
		B0 = +Quality * Alpha;
		B1 = 0.f;
		B2 = -Quality * Alpha;
	}
	else if (Type == EBandType::BandCut)
	{
		const float GainSqrt = FMath::Pow(10, AmountDb / 40.f);
		const float Omega = 2.f * PI * Frequency / SampleRate;
		const float Sn = FMath::Sin(Omega);
		const float Cs = FMath::Cos(Omega);
		const float Alpha = .5f * Sn / Quality;

		A0 = 1.f + Alpha / GainSqrt;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha / GainSqrt;
		B0 = 1.f + Alpha * GainSqrt;
		B1 = -2.f * Cs;
		B2 = 1.f - Alpha * GainSqrt;
	}
	else if (Type == EBandType::Notch)
	{
		const float Omega = 2.f * PI * Frequency / SampleRate;
		const float Sn = FMath::Sin(Omega);
		const float Cs = FMath::Cos(Omega);
		const float Alpha = .5f * Sn / Quality;

		A0 = 1.f + Alpha;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha;
		B0 = 1.f;
		B1 = -2.f * Cs;
		B2 = 1.f;
	}
}

