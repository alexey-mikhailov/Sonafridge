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

	if (Type == EBandType::HighCut)
	{
		
	}
	else if (Type == EBandType::LowCut)
	{
		
	}
	else if (Type == EBandType::LowShelf)
	{
		
	}
	else if (Type == EBandType::HighShelf)
	{
		
	}
	else if (Type == EBandType::BandPass)
	{
		
	}
	else if (Type == EBandType::BandCut)
	{
		float Sn = FMath::Sin(Omega);
		float Cs = FMath::Cos(Omega);
		Alpha = .5f * Sn / Quality;
		float GainSqrt = FMath::Pow(10, AmountDb / 40.f);

		A0 = 1.f + Alpha / GainSqrt;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha / GainSqrt;
		B0 = 1.f + Alpha * GainSqrt;
		B1 = -2 * Cs;
		B2 = 1 - Alpha * GainSqrt;
	}
	else if (Type == EBandType::Notch)
	{

	}
}

