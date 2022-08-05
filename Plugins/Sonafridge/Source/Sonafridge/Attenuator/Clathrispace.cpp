#include "Clathrispace.h"

#include "Sonafridge/Tools/MathTools.h"

UClathrispaceSettings::UClathrispaceSettings()
{
	EarData.EarPinsL.Add
	({
		FVector::LeftVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsL.Add
	({
		FVector::RightVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsL.Add
	({
		FVector::UpVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsL.Add
	({
		FVector::DownVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsL.Add
	({
		FVector::ForwardVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsL.Add
	({
		FVector::BackwardVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::RightVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::LeftVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::UpVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::DownVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::ForwardVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::BackwardVector,
		100.f, 0.f, 1.f, 0.f,
		667.f, 0.f, 1.f, 0.f,
		4444.f, 0.f, 1.f, 0.f,
	});
}

bool UClathrispaceSettings::CanEditChange(const FProperty* InProperty) const
{
	return Super::CanEditChange(InProperty);
}

void UClathrispaceSettings::CopyLeftToRight()
{
	EarData.EarPositionR = EarData.EarPositionL;
	MathTool::ReflectVectorY(EarData.EarPositionR);

	EarData.EarPinsR.SetNum(EarData.EarPinsL.Num());
	for (int32 Index = 0; Index < EarData.EarPinsL.Num(); ++Index)
	{
		FEarPin& LeftPin  = EarData.EarPinsL[Index];
		FEarPin& RightPin = EarData.EarPinsR[Index];

		RightPin = LeftPin;
		MathTool::ReflectVectorY(RightPin.Direction);
	}
}

void UClathrispaceSettings::CopyRightToLeft()
{
	EarData.EarPositionL = EarData.EarPositionR;
	MathTool::ReflectVectorY(EarData.EarPositionL);

	EarData.EarPinsL.SetNum(EarData.EarPinsR.Num());
	for (int32 Index = 0; Index < EarData.EarPinsR.Num(); ++Index)
	{
		FEarPin& LeftPin = EarData.EarPinsL[Index];
		FEarPin& RightPin = EarData.EarPinsR[Index];

		LeftPin = RightPin;
		MathTool::ReflectVectorY(LeftPin.Direction);
	}
}

TAudioSpatializationPtr FClathrispaceFactory::CreateNewSpatializationPlugin(FAudioDevice* OwningDevice)
{
	return MakeShared<FClathrispace, ESPMode::ThreadSafe>();
}

FClathrispace::FClathrispace()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("FClathrispace::FClathrispace"));
}

void FClathrispace::Initialize(const FAudioPluginInitializationParams Params)
{
	bInitialized = true;
	NumberOfOutputs = Params.NumOutputChannels;
	SampleRate = Params.SampleRate;
	BufferLength = Params.BufferLength;

	FilterL.Init(SampleRate, 1, 0.f, EEQBandType::CutHighButterworth, 0.f);
	FilterR.Init(SampleRate, 1, 0.f, EEQBandType::CutHighButterworth, 0.f);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("FClathrispace::Initialize"));
}

void FClathrispace::OnInitSource(const uint32                             SourceId,
                                 const FName&                             AudioComponentUserId,
                                 USpatializationPluginSourceSettingsBase* InSettings)
{
	GEngine->AddOnScreenDebugMessage(-1,
	                                 1.f,
	                                 FColor::Green,
	                                 FString::Printf(
		                                 TEXT("FClathrispace::OnInitSource. Source: %i, AudioComponent: %s"),
		                                 SourceId,
		                                 *AudioComponentUserId.ToString()));

	Settings = Cast<UClathrispaceSettings>(InSettings);
}

void FClathrispace::OnReleaseSource(const uint32 SourceId)
{
	GEngine->AddOnScreenDebugMessage(-1,
									1.f,
									FColor::Green,
									FString::Printf(
										TEXT("FClathrispace::OnReleaseSource. Source: %i"),
										SourceId));
}

bool FClathrispace::IsSpatializationEffectInitialized() const
{
	GEngine->AddOnScreenDebugMessage(-1,
									1.f,
									FColor::Green,
									FString::Printf(
										TEXT("FClathrispace::IsSpatializationEffectInitialized with Result %i"),
										bInitialized));
	return bInitialized;
}

void FClathrispace::ProcessAudio(const FAudioPluginSourceInputData& InputData,
                                       FAudioPluginSourceOutputData& OutputData)
{
	FVector EmitterPos = InputData.SpatializationParams->EmitterPosition;
	FVector LeftEar = FVector::UpVector.RotateAngleAxis(-45.f, FVector::ForwardVector);
	FVector RightEar = FVector::UpVector.RotateAngleAxis(45.f, FVector::ForwardVector);
	float IsBackFromLeft = FVector::DotProduct(EmitterPos, LeftEar);
	float IsBackFromRight = FVector::DotProduct(EmitterPos, RightEar);
	
	float LeftGain;
	float RightGain;
	float IsRight = InputData.SpatializationParams->EmitterPosition.Y;
	FMath::SinCos(&RightGain, &LeftGain, .25f * PI * (IsRight + 1.f));

	IsBackFromLeft = .5f * (IsBackFromLeft + 1.f);
	IsBackFromRight = .5f * (IsBackFromRight + 1.f);
	IsBackFromLeft = FMath::Pow(IsBackFromLeft, 2.f);
	IsBackFromRight = FMath::Pow(IsBackFromRight, 2.f);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("IsBackFromLeft: %f"), IsBackFromLeft));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("IsBackFromRight: %f"), IsBackFromRight));

	FilterL.SetFrequency(FMath::Lerp(20000.f, 4000.f, IsBackFromLeft));
	FilterR.SetFrequency(FMath::Lerp(20000.f, 4000.f, IsBackFromRight));

	LeftGain = FMath::Lerp(LeftGain, 1.f + IsBackFromLeft, .25f);
	RightGain = FMath::Lerp(RightGain, 1.f + IsBackFromRight, .25f);

	float* InputPtr = InputData.AudioBuffer->GetData();
	float* OutputPtr = OutputData.AudioBuffer.GetData();
	for (uint32 I = {}; I < BufferLength; ++I)
	{
		//*OutputPtr = *InputPtr;
		*OutputPtr = *InputPtr * LeftGain;
		FilterL.ProcessAudio(OutputPtr, 1, OutputPtr);
		++OutputPtr;

		//*OutputPtr = *InputPtr;
		*OutputPtr = *InputPtr * RightGain;
		FilterR.ProcessAudio(OutputPtr, 1, OutputPtr);
		++OutputPtr;

		++InputPtr;
	}
}
