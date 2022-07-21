// Sonafridge 2022

#include "MathToolsTest.h"
#include "CoreMinimal.h"
#include "MathTools.h"
#include "Misc/AutomationTest.h"

#if (WITH_DEV_AUTOMATION_TESTS || WITH_PERF_AUTOMATION_TESTS)

enum class ETestType
{
	Functional,
	Performance,
};

auto TestType = ETestType::Functional;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRcnAddTest,
	"Sonafridge.ComplexNumbers.01 Add rectangular numbers",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ClientContext |
	EAutomationTestFlags::ServerContext |
	EAutomationTestFlags::PerfFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEcnAddTest,
	"Sonafridge.ComplexNumbers.02 Add exponential numbers",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ClientContext |
	EAutomationTestFlags::ServerContext |
	EAutomationTestFlags::PerfFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRcnMultiplyTest,
	"Sonafridge.ComplexNumbers.03 Multiply rectangular numbers",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ClientContext |
	EAutomationTestFlags::ServerContext |
	EAutomationTestFlags::PerfFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEcnMultiplyTest,
	"Sonafridge.ComplexNumbers.04 Multiply exponential numbers",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ClientContext |
	EAutomationTestFlags::ServerContext |
	EAutomationTestFlags::PerfFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRcnMultiplyToScalarTest,
	"Sonafridge.ComplexNumbers.05 Multiply rectangular numbers to scalar",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ClientContext |
	EAutomationTestFlags::ServerContext |
	EAutomationTestFlags::PerfFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEcnMultiplyToScalarTest,
	"Sonafridge.ComplexNumbers.06 Multiply exponential numbers to scalar",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ClientContext |
	EAutomationTestFlags::ServerContext |
	EAutomationTestFlags::PerfFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRcnDivideTest,
	"Sonafridge.ComplexNumbers.07 Divide rectangular numbers",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ClientContext |
	EAutomationTestFlags::ServerContext |
	EAutomationTestFlags::PerfFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEcnDivideTest,
	"Sonafridge.ComplexNumbers.08 Divide exponential numbers",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ClientContext |
	EAutomationTestFlags::ServerContext |
	EAutomationTestFlags::PerfFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRcnDtftTest,
	"Sonafridge.ComplexNumbers.90 DTFT rectangular numbers",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ClientContext |
	EAutomationTestFlags::ServerContext |
	EAutomationTestFlags::PerfFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEcnDtftTest,
	"Sonafridge.ComplexNumbers.91 DTFT exponential numbers",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ClientContext |
	EAutomationTestFlags::ServerContext |
	EAutomationTestFlags::PerfFilter)

bool FRcnAddTest::RunTest(const FString& Parameters)
{
	FRcn N1, N2, Sum;

	for (float I = 0.f; I < 1000.f; ++I)
	{
		N1.Real = I + 0.f;
		N1.Imag = I + 1.f;
		N2.Real = I + 2.f;
		N2.Imag = I + 3.f;
		Sum = N1 + N2;

		if (TestType == ETestType::Functional)
		{
			TestEqual("Sum of real parts", Sum.Real, I + I + 2.f);
			TestEqual("Sum of imag parts", Sum.Imag, I + I + 4.f);
		}
	}

	return true;
}

bool FEcnAddTest::RunTest(const FString& Parameters)
{
	FEcn N1, N2, Sum;
	float Angle;
	constexpr float Tolerance = 0.001f;

	for (float I = 0.f; I < 1000.f; ++I)
	{
		Angle = I;
		while (Angle > PI)
		{
			Angle -= PI;
		}

		N1.Radius = I + 0.f;
		N1.Angle = Angle;
		N2.Radius = I + 2.f;
		N2.Angle = Angle;
		Sum = N1 + N2;

		if (TestType == ETestType::Functional)
		{
			TestEqual("Radius of sum", Sum.Radius, I + I + 2.f, Tolerance);
			TestEqual("Angle of sum", Sum.Angle, Angle);
		}
	}

	return true;
}

bool FRcnMultiplyTest::RunTest(const FString& Parameters)
{
	FRcn N1, N2, Prd;

	for (float I = 0.f; I < 1000.f; ++I)
	{
		N1.Real = I + 0.f;
		N1.Imag = I + 1.f;
		N2.Real = I + 2.f;
		N2.Imag = I + 3.f;
		Prd = N1 * N2;

		if (TestType == ETestType::Functional)
		{
			TestEqual("Real part of product", Prd.Real, N1.Real * N2.Real - N1.Imag * N2.Imag);
			TestEqual("Imag part of product", Prd.Imag, N1.Real * N2.Imag - N1.Imag * N2.Real);
		}
	}

	return true;
}

bool FEcnMultiplyTest::RunTest(const FString& Parameters)
{
	FEcn N1, N2, Sum;
	float Angle;
	constexpr float Tolerance = 0.001f;

	for (float I = 0.f; I < 1000.f; ++I)
	{
		Angle = I * PI;
		while (Angle > PI)
		{
			Angle -= PI;
		}

		N1.Radius = I + 0.f;
		N1.Angle = Angle;
		N2.Radius = I + 2.f;
		N2.Angle = Angle;
		Sum = N1 * N2;

		if (TestType == ETestType::Functional)
		{
			TestEqual("Radius of product", Sum.Radius, I * (I + 2.f), Tolerance);
			TestEqual("Angle of product", Sum.Angle, Angle + Angle);
		}
	}

	return true;
}

bool FRcnMultiplyToScalarTest::RunTest(const FString& Parameters)
{
	FRcn N1, Prd;
	float N2;

	for (float I = 0.f; I < 1000.f; ++I)
	{
		N1.Real = I + 0.f;
		N1.Imag = I + 1.f;
		N2 = I + 2.f;
		Prd = N1 * N2;

		if (TestType == ETestType::Functional)
		{
			TestEqual("Real part of product", Prd.Real, N1.Real * N2);
			TestEqual("Imag part of product", Prd.Imag, N1.Imag * N2);
		}
	}

	return true;
}

bool FEcnMultiplyToScalarTest::RunTest(const FString& Parameters)
{
	FEcn N1, Sum;
	float N2;
	float Angle;
	constexpr float Tolerance = 0.001f;

	for (float I = 0.f; I < 1000.f; ++I)
	{
		Angle = I * PI;
		while (Angle > PI)
		{
			Angle -= PI;
		}

		N1.Radius = I + 0.f;
		N1.Angle = Angle;
		N2 = I + 2.f;
		Sum = N1 * N2;

		if (TestType == ETestType::Functional)
		{
			TestEqual("Radius of product", Sum.Radius, I * (I + 2.f), Tolerance);
			TestEqual("Angle of product", Sum.Angle, Angle);
		}
	}

	return true;
}

bool FRcnDivideTest::RunTest(const FString& Parameters)
{
	FRcn N1, N2, Prd;

	for (float I = 0.f; I < 1000.f; ++I)
	{
		N1.Real = I + 0.f;
		N1.Imag = I + 1.f;
		N2.Real = I + 2.f;
		N2.Imag = I + 3.f;
		Prd = N1 / N2;

		if (TestType == ETestType::Functional)
		{
			TestEqual("Real part of fraction", Prd.Real, (N1.Real * N2.Real + N1.Imag * N2.Imag) / (N2.Real * N2.Real + N2.Imag * N2.Imag));
			TestEqual("Imag part of fraction", Prd.Imag, (N1.Imag * N2.Real - N1.Real * N2.Imag) / (N2.Real * N2.Real + N2.Imag * N2.Imag));
		}
	}

	return true;
}

bool FEcnDivideTest::RunTest(const FString& Parameters)
{
	FEcn N1, N2, Sum;
	float Angle;
	constexpr float Tolerance = 0.001f;

	for (float I = 0.f; I < 1000.f; ++I)
	{
		Angle = I * PI;
		while (Angle > PI)
		{
			Angle -= PI;
		}

		N1.Radius = I + 0.f;
		N1.Angle = 3 * Angle;
		N2.Radius = I + 2.f;
		N2.Angle = Angle;
		Sum = N1 / N2;

		if (TestType == ETestType::Functional)
		{
			TestEqual("Radius of fraction", Sum.Radius, I / (I + 2.f), Tolerance);
			TestEqual("Angle of fraction", Sum.Angle, Angle + Angle);
		}
	}

	return true;
}

bool FRcnDtftTest::RunTest(const FString& Parameters)
{
	float SampleRate = 44100.f;
	float Frequency = 1000.f;
	float Quality = 1.f;
	float Gain = .5f;
	float GainSqrt = FMath::Sqrt(Gain);

	float Omega = 2.f * PI * Frequency / SampleRate;
	float Sn = FMath::Sin(Omega);
	float Cs = FMath::Cos(Omega);
	float Alpha = .5f * Sn / Quality;

	float A0 = 1.f + Alpha / GainSqrt;
	float A1 = -2.f * Cs;
	float A2 = 1.f - Alpha / GainSqrt;
	float B0 = 1.f + Alpha * GainSqrt;
	float B1 = -2 * Cs;
	float B2 = 1 - Alpha * GainSqrt;

	for (int32 I = 0; I < 1000; ++I)
	{
		float F = .001f * I * SampleRate;
		float O = 2.f * PI * Frequency / SampleRate;
		FRcn Angle0 = FRcn::FromAngle(O) ^ -0;
		FRcn Angle1 = FRcn::FromAngle(O) ^ -1;
		FRcn Angle2 = FRcn::FromAngle(O) ^ -2;

		(B0 * Angle0 + B1 * Angle1 + B2 * Angle2) /
		(A0 * Angle0 + A1 * Angle1 + A2 * Angle2);
	}

	if (TestType == ETestType::Functional)
	{
		float F1 = Frequency;
		float F2 = .25 * Frequency;
		float F3 = 4 * Frequency;
		float O1 = 2.f * PI * F1 / SampleRate;
		float O2 = 2.f * PI * F2 / SampleRate;
		float O3 = 2.f * PI * F3 / SampleRate;
		FRcn Angle1_0 = FRcn::FromAngle(O1)^-0;
		FRcn Angle1_1 = FRcn::FromAngle(O1)^-1;
		FRcn Angle1_2 = FRcn::FromAngle(O1)^-2;
		FRcn Angle2_0 = FRcn::FromAngle(O2)^-0;
		FRcn Angle2_1 = FRcn::FromAngle(O2)^-1;
		FRcn Angle2_2 = FRcn::FromAngle(O2)^-2;
		FRcn Angle3_0 = FRcn::FromAngle(O3)^-0;
		FRcn Angle3_1 = FRcn::FromAngle(O3)^-1;
		FRcn Angle3_2 = FRcn::FromAngle(O3)^-2;

		FRcn Response1 =
		{
			(B0 * Angle1_0 + B1 * Angle1_1 + B2 * Angle1_2) /
			(A0 * Angle1_0 + A1 * Angle1_1 + A2 * Angle1_2)
		};

		FRcn Response2 =
		{
			(B0 * Angle2_0 + B1 * Angle2_1 + B2 * Angle2_2) /
			(A0 * Angle2_0 + A1 * Angle2_1 + A2 * Angle2_2)
		};

		FRcn Response3 =
		{
			(B0 * Angle3_0 + B1 * Angle3_1 + B2 * Angle3_2) /
			(A0 * Angle3_0 + A1 * Angle3_1 + A2 * Angle3_2)
		};

		TestNotEqual("Radii of central frequency and left frequency", Response1.GetRadius(), Response2.GetRadius());
		TestNotEqual("Radii of central frequency and right frequency", Response1.GetRadius(), Response3.GetRadius());
		TestEqual("Radii of left frequency and right frequency", Response2.GetRadius(), Response3.GetRadius(), .1f);
	}

	return true;
}

bool FEcnDtftTest::RunTest(const FString& Parameters)
{
	float SampleRate = 44100.f;
	float Frequency = 1000.f;
	float Quality = 1.f;
	float Gain = .5f;
	float GainSqrt = FMath::Sqrt(Gain);

	float Omega = 2.f * PI * Frequency / SampleRate;
	float Sn = FMath::Sin(Omega);
	float Cs = FMath::Cos(Omega);
	float Alpha = .5f * Sn / Quality;

	float A0 = 1.f + Alpha / GainSqrt;
	float A1 = -2.f * Cs;
	float A2 = 1.f - Alpha / GainSqrt;
	float B0 = 1.f + Alpha * GainSqrt;
	float B1 = -2 * Cs;
	float B2 = 1 - Alpha * GainSqrt;

	for (int32 I = 0; I < 1000; ++I)
	{
		float F = .001f * I * SampleRate;
		float O = 2.f * PI * F / SampleRate;
		FEcn Angle0 = FEcn(O)^-0;
		FEcn Angle1 = FEcn(O)^-1;
		FEcn Angle2 = FEcn(O)^-2;

		(B0 * Angle0 + B1 * Angle1 + B2 * Angle2) /
		(A0 * Angle0 + A1 * Angle1 + A2 * Angle2);
	}

	if (TestType == ETestType::Functional)
	{
		float F1 = Frequency;
		float F2 = .25 * Frequency;
		float F3 = 4 * Frequency;
		float O1 = 2.f * PI * F1 / SampleRate;
		float O2 = 2.f * PI * F2 / SampleRate;
		float O3 = 2.f * PI * F3 / SampleRate;
		FEcn Angle1_0 = FEcn(O1)^-0;
		FEcn Angle1_1 = FEcn(O1)^-1;
		FEcn Angle1_2 = FEcn(O1)^-2;
		FEcn Angle2_0 = FEcn(O2)^-0;
		FEcn Angle2_1 = FEcn(O2)^-1;
		FEcn Angle2_2 = FEcn(O2)^-2;
		FEcn Angle3_0 = FEcn(O3)^-0;
		FEcn Angle3_1 = FEcn(O3)^-1;
		FEcn Angle3_2 = FEcn(O3)^-2;

		FEcn Response1 =
		{
			(B0 * Angle1_0 + B1 * Angle1_1 + B2 * Angle1_2) /
			(A0 * Angle1_0 + A1 * Angle1_1 + A2 * Angle1_2)
		};

		FEcn Response2 =
		{
			(B0 * Angle2_0 + B1 * Angle2_1 + B2 * Angle2_2) /
			(A0 * Angle2_0 + A1 * Angle2_1 + A2 * Angle2_2)
		};

		FEcn Response3 =
		{
			(B0 * Angle3_0 + B1 * Angle3_1 + B2 * Angle3_2) /
			(A0 * Angle3_0 + A1 * Angle3_1 + A2 * Angle3_2)
		};

		TestNotEqual("Radii of central frequency and left frequency", Response1.Radius, Response2.Radius);
		TestNotEqual("Radii of central frequency and right frequency", Response1.Radius, Response3.Radius);
		TestEqual("Radii of left frequency and right frequency", Response2.Radius, Response3.Radius, .1f);
	}

	return true;
}

#endif

