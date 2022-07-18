// Sonafridge 2022

#include "VM_SonaQBand.h"

#include "VM_SonaQ.h"
#include "Sonafridge/MathTools.h"

FVM_SonaQBand::FVM_SonaQBand()
{
	Response.SetNumZeroed(FVM_SonaQ::Resolution + 1);
}

void FVM_SonaQBand::Init(float InSampleRate)
{
	SampleRate = InSampleRate;
	bIsInitialized = true;
}

void FVM_SonaQBand::SetType(EEQBandType Value)
{
	LogIfUninitialized();
	Type = Value;
	Recalculate();
}

void FVM_SonaQBand::SetIsEnabled(bool bInValue)
{
	LogIfUninitialized();
	bIsEnabled = bInValue;
	Recalculate();
}

void FVM_SonaQBand::SetFrequency(float Value)
{
	LogIfUninitialized();
	Frequency = FMath::Clamp(Value, FVM_SonaQ::FrequencyMin, FVM_SonaQ::FrequencyMax);
	Recalculate();
}

void FVM_SonaQBand::SetQuality(float Value)
{
	LogIfUninitialized();
	Quality = Value;
	Recalculate();
}

void FVM_SonaQBand::SetAmountDb(float Value)
{
	LogIfUninitialized();
	AmountDb = Value;
	Recalculate();
}

void FVM_SonaQBand::SetMakeupDb(float Value)
{
	LogIfUninitialized();
	MakeupDb = Value;
	MakeupCoeff = MathLogTool::VigesibelToLinear(Value);
	Recalculate();
}

void FVM_SonaQBand::SetMakeupCoeff(float Value)
{
	LogIfUninitialized();
	MakeupDb = MathLogTool::LinearToVigesibel(Value);
	MakeupCoeff = Value;
	Recalculate();
}

float FVM_SonaQBand::Dtft(float InFrequency) const
{
	float Index = (FMath::Loge(InFrequency) - FVM_SonaQ::FLMin) / FVM_SonaQ::FLS;

	int32 Index1 = FMath::FloorToInt(Index);
	int32 Index2 = FMath::CeilToInt(Index);
	float Alpha = FMath::Frac(Index);
	return FMath::Lerp(Response[Index1], Response[Index2], Alpha);;
}

EEQBandType FVM_SonaQBand::GetType() const
{
	LogIfUninitialized();
	return Type;
}

bool FVM_SonaQBand::GetIsEnabled() const
{
	LogIfUninitialized();
	return bIsEnabled;
}

float FVM_SonaQBand::GetFrequency() const
{
	LogIfUninitialized();
	return Frequency;
}

float FVM_SonaQBand::GetQuality() const
{
	LogIfUninitialized();
	return Quality;
}

float FVM_SonaQBand::GetAmountDb() const
{
	LogIfUninitialized();
	return AmountDb;
}

float FVM_SonaQBand::GetMakeupDb() const
{
	LogIfUninitialized();
	return MakeupDb;
}

float FVM_SonaQBand::GetResponseAvgDb() const
{
	LogIfUninitialized();
	return ResponseAvgDb;
}

void FVM_SonaQBand::Recalculate()
{
	if (Type == EEQBandType::CutLowFast)
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
	else if (Type == EEQBandType::CutLowButterworth)
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
	else if (Type == EEQBandType::CutHighFast)
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
	else if (Type == EEQBandType::CutHighButterworth)
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
	else if (Type == EEQBandType::PassBand)
	{
		const float Omega = 2.f * PI * Frequency / SampleRate;
		const float Sn = FMath::Sin(Omega);
		const float Cs = FMath::Cos(Omega);
		const float Alpha = .5f * Sn / Quality;

		A0 = 1.f + Alpha;
		A1 = -2.f * Cs;
		A2 = 1.f - Alpha;
		B0 = +Alpha * MakeupCoeff;
		B1 = 0.f;
		B2 = -Alpha * MakeupCoeff;
	}
	else if (Type == EEQBandType::AttLow)
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
	else if (Type == EEQBandType::AttHigh)
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
	else if (Type == EEQBandType::AttBand)
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
	else if (Type == EEQBandType::Notch)
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

	// Bake response
	ResponseAvgDb = 0.f;

	if (Type == EEQBandType::AttBand)
	{
		float PrevResponse = 1.f;
		for (int32 I = 0; I < FVM_SonaQ::Resolution + 1; ++I)
		{
			float F = FMath::Exp(I * FVM_SonaQ::FLS + FVM_SonaQ::FLMin);
			float ThisResponse = DtftImpl(F);
			float ThisRDb = MathLogTool::LinearToVigesibel(ThisResponse);
			float PrevRDb = MathLogTool::LinearToVigesibel(PrevResponse);

			float Alpha96Db = (ThisRDb - FVM_SonaQ::DynamicMin) / (FVM_SonaQ::DynamicMax - FVM_SonaQ::DynamicMin);
			ThisRDb *= 1.5f * Alpha96Db;
			Alpha96Db = (PrevRDb - FVM_SonaQ::DynamicMin) / (FVM_SonaQ::DynamicMax - FVM_SonaQ::DynamicMin);
			PrevRDb *= 1.5f * Alpha96Db;

			const float Diff = FMath::Abs(ThisRDb - PrevRDb);
			ThisRDb *= 4.f * FMath::Pow(Diff, .5f);

			ResponseAvgDb += ThisRDb;
			PrevResponse = ThisResponse;
			Response[I] = ThisResponse;
		}
	}
	else if (Type == EEQBandType::AttLow || Type == EEQBandType::AttHigh)
	{
		for (int32 I = 0; I < FVM_SonaQ::Resolution + 1; ++I)
		{
			float F = FMath::Exp(I * FVM_SonaQ::FLS + FVM_SonaQ::FLMin);
			float ThisResponse = DtftImpl(F);
			float ThisRDb = MathLogTool::LinearToVigesibel(ThisResponse);

			float Alpha96Db = (ThisRDb - FVM_SonaQ::DynamicMin) / (FVM_SonaQ::DynamicMax - FVM_SonaQ::DynamicMin);
			ThisRDb *= 2.f * Alpha96Db;

			ResponseAvgDb += ThisRDb;
			Response[I] = ThisResponse;
		}
	}
	else
	{
		for (int32 I = 0; I < FVM_SonaQ::Resolution + 1; ++I)
		{
			float F = FMath::Exp(I * FVM_SonaQ::FLS + FVM_SonaQ::FLMin);
			float ThisResponse = DtftImpl(F);
			float ThisResponseDb = MathLogTool::LinearToVigesibel(ThisResponse);

			ResponseAvgDb += ThisResponseDb;
			Response[I] = ThisResponse;
		}
	}

	if (bIsEnabled)
	{
		for (int32 I = 0; I < FVM_SonaQ::Resolution + 1; ++I)
		{
			Response[I] *= MakeupCoeff;
		}
	}

	ResponseAvgDb /= FVM_SonaQ::Resolution + 1;
}

float FVM_SonaQBand::DtftImpl(float InFrequency) const
{
	if (bIsEnabled)
	{
		auto InOmega = 2.f * PI * InFrequency / SampleRate;
		FEcn Angle0 = FEcn(InOmega)^-0;
		FEcn Angle1 = FEcn(InOmega)^-1;
		FEcn Angle2 = FEcn(InOmega)^-2;

		return
		(
			(B0 * Angle0 + B1 * Angle1 + B2 * Angle2) /
			(A0 * Angle0 + A1 * Angle1 + A2 * Angle2)
		)
		.Radius;
	}

	return 1.f;
}

