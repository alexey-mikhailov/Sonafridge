// Sonafridge 2022

#include "Clathrispace.h"

#include "AudioDevice.h"
#include "ClathriEarStatDrawer.h"
#include "Sonafridge/Tools/MathTools.h"

UClathrispaceSettings::UClathrispaceSettings()
{
	EarData.EarPinsL.Add
	({
		FVector::LeftVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsL.Add
	({
		FVector::RightVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsL.Add
	({
		FVector::UpVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsL.Add
	({
		FVector::DownVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsL.Add
	({
		FVector::ForwardVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsL.Add
	({
		FVector::BackwardVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::RightVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::LeftVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::UpVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::DownVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::ForwardVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPinsR.Add
	({
		FVector::BackwardVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
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

	if (!MarkPackageDirty())
	{
		UE_LOG(LogSonafridge, Error, TEXT("UClathrispaceSettings::CopyLeftToRight: Could not mark package dirty. "));
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

	if (!MarkPackageDirty())
	{
		UE_LOG(LogSonafridge, Error, TEXT("UClathrispaceSettings::CopyRightToLeft: Could not mark package dirty. "));
	}
}

void UClathrispaceSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ExternallyChanged.Broadcast();
}

TAudioSpatializationPtr FClathrispaceFactory::CreateNewSpatializationPlugin(FAudioDevice* OwningDevice)
{
	return MakeShared<FClathrispace, ESPMode::ThreadSafe>();
}

FClathrispace::FClathrispace()
	: StatDrawer{ MakeShared<FClathriEarStatDrawer>(this) }
{
}

FClathrispace::~FClathrispace()
{
	if (Settings.IsValid())
	{
		Settings->GetEvent_ExternallyChanged().Remove(ExternallyChangedDelegateHandle);
		Settings->GetEvent_InternallyChanged().Remove(InternallyChangedDelegateHandle);
	}
}

void FClathrispace::Initialize(const FAudioPluginInitializationParams Params)
{
	bInitialized = true;

	// It turned out no easy way to get audio channel configuration in UE. 
	// - Params.NumOutputChannels is coming as initialized from ctor as 0.
	// - Params.AudioDevicePtr->GetMaxChannels() is about 32 or similar, but not 2 in my case.
	// Leave it 2 as declared in ctor. 

	SampleRate = Params.SampleRate;
	NumSamples = Params.BufferLength;

	for (int32 Index = 0; Index < UClathrispaceSettings::BandCount; ++Index)
	{
		FilterL[Index].Init(SampleRate);
		FilterL[Index].SetNumDstChannels(NumDstChannels);
		FilterL[Index].SetDstChannelIndex(0);
		FilterR[Index].Init(SampleRate);
		FilterR[Index].SetNumDstChannels(NumDstChannels);
		FilterR[Index].SetDstChannelIndex(1);
	}

	StatDrawer->Initialize();
}

void FClathrispace::Shutdown()
{
	if (StatDrawer)
	{
		StatDrawer->Shutdown();
	}
}

void FClathrispace::OnInitSource(const uint32                             SourceId,
                                 const FName&                             AudioComponentUserId,
                                 USpatializationPluginSourceSettingsBase* InSettings)
{
	Settings = Cast<UClathrispaceSettings>(InSettings);

	if (Settings.IsValid())
	{
		ExternallyChangedDelegateHandle = Settings->GetEvent_ExternallyChanged().AddRaw
		(
			this, &FClathrispace::OnAssetExternallyChanged
		);
		InternallyChangedDelegateHandle = Settings->GetEvent_InternallyChanged().AddRaw
		(
			this, &FClathrispace::OnAssetInternallyChanged
		);
	}
	else
	{
		UE_LOG(LogSonafridge,
		       Error,
		       TEXT("FClathrispace::OnInitSource: "
			        "Argument InSettings is not an instance of UClathrispaceSettings class. "));
	}
}

void FClathrispace::OnReleaseSource(const uint32 SourceId)
{
}

bool FClathrispace::IsSpatializationEffectInitialized() const
{
	return bInitialized;
}

void FClathrispace::ProcessAudio(const FAudioPluginSourceInputData& Src,
                                       FAudioPluginSourceOutputData& Dst)
{
	FVector EmitterDirection = Src.SpatializationParams->ListenerOrientation.Inverse()
	                         *    (Src.SpatializationParams->EmitterWorldPosition 
	                             - Src.SpatializationParams->ListenerPosition);

	if (PreviousEmitterDirection.IsNearlyZero())
	{
		PreviousEmitterDirection = EmitterDirection;
	}

	RecalculateEar(EmitterDirection.GetUnsafeNormal());

	for (int32 Index = 0; Index < UClathrispaceSettings::BandCount; ++Index)
	{
		FilterL[Index].SetNumSrcChannels(Src.NumChannels);
		FilterL[Index].SetMode
		(
			Src.NumChannels == 1
			? EBiquadMixerMode::BroadcastMono
			: Src.NumChannels == 2
			  ? EBiquadMixerMode::BroadcastMonoFromStereo
			  : EBiquadMixerMode::BroadcastMonoFromMultichannel
		);

		FilterR[Index].SetNumSrcChannels(Src.NumChannels);
		FilterR[Index].SetMode
		(
			Src.NumChannels == 1
			? EBiquadMixerMode::BroadcastMono
			: Src.NumChannels == 2
			  ? EBiquadMixerMode::BroadcastMonoFromStereo
			  : EBiquadMixerMode::BroadcastMonoFromMultichannel
		);
	}

	if (Src.AudioBuffer)
	{
		float* SrcBuffer = Src.AudioBuffer->GetData();
		float* DstBuffer = Dst.AudioBuffer.GetData();

		FilterL[0].ProcessInterlaced(SrcBuffer, DstBuffer, FilterL[0].GetMixerMode(), NumSamples, Src.NumChannels, NumDstChannels, 0);
		FilterL[1].ProcessInterlaced(DstBuffer, DstBuffer, EBiquadMixerMode::NoBlending, NumSamples, NumDstChannels, NumDstChannels, 0);
		FilterL[2].ProcessInterlaced(DstBuffer, DstBuffer, EBiquadMixerMode::NoBlending, NumSamples, NumDstChannels, NumDstChannels, 0);
		FilterR[0].ProcessInterlaced(SrcBuffer, DstBuffer, FilterR[0].GetMixerMode(), NumSamples, Src.NumChannels, NumDstChannels, 1);
		FilterR[1].ProcessInterlaced(DstBuffer, DstBuffer, EBiquadMixerMode::NoBlending, NumSamples, NumDstChannels, NumDstChannels, 1);
		FilterR[2].ProcessInterlaced(DstBuffer, DstBuffer, EBiquadMixerMode::NoBlending, NumSamples, NumDstChannels, NumDstChannels, 1);
	}

	PreviousEmitterDirection = EmitterDirection;
}

void FClathrispace::OnAssetExternallyChanged()
{
	// TODO: Do not create M filters, where M is number of bands.
	// TODO: Create NxM filters, where N is number of pins. 
	// TODO: Pass settings here. 
}

void FClathrispace::OnAssetInternallyChanged()
{
	// TODO: Do not create M filters, where M is number of bands.
	// TODO: Create NxM filters, where N is number of pins. 
	// TODO: Pass settings here. 
}

void FClathrispace::RecalculateEar(const FVector& InEmitterNormal)
{
	if (!Settings.IsValid())
	{
		return;
	}

	if (FMath::IsNaN(InEmitterNormal.X) || 
		FMath::IsNaN(InEmitterNormal.Y) || 
		FMath::IsNaN(InEmitterNormal.Z) || 
		InEmitterNormal.Size() < TNumericLimits<float>::Min())
	{
		return;
	}

	struct FSortResult
	{
		int32    Index;
		float    DotProduct;
		FEarPin* Pin;

		FSortResult(int32 InIndex, float InDotProduct, FEarPin* InPin) : Index(InIndex),
		                                                                 DotProduct(InDotProduct),
		                                                                 Pin(InPin)
		{
		}
	};

	// Left pins. 
	{
		TArray<FSortResult> Results;
		for (int32 Index = 0; Index < Settings->GetEarData().EarPinsL.Num(); ++Index)
		{
			FEarPin& Pin = Settings->GetEarData().EarPinsL[Index];
			float DotProduct = FVector::DotProduct(InEmitterNormal, Pin.Direction);
			Results.Add({ Index, DotProduct, &Pin });
		}

		TArray<float> Keys;
		Results.Sort([](const FSortResult& R1, const FSortResult& R2) { return R1.DotProduct > R2.DotProduct; });

		FEarPin& Pin1 = *Results[0].Pin;
		FEarPin& Pin2 = *Results[1].Pin;
		FEarPin& Pin3 = *Results[2].Pin;

		FQuat Q1 = FQuat::FindBetweenNormals(InEmitterNormal, Pin1.Direction.GetSafeNormal());
		FQuat Q2 = FQuat::FindBetweenNormals(InEmitterNormal, Pin2.Direction.GetSafeNormal());
		FQuat Q3 = FQuat::FindBetweenNormals(InEmitterNormal, Pin3.Direction.GetSafeNormal());

		ClathriEarStatWeightsL = FMath::ComputeBaryCentric2D(FVector::ZeroVector, Q1.Euler(), Q2.Euler(), Q3.Euler());
		ClathriEarStatIdxL1 = Results[0].Index;
		ClathriEarStatIdxL2 = Results[1].Index;
		ClathriEarStatIdxL3 = Results[2].Index;

		float Band1Frequency = Pin1.Band1Frequency * ClathriEarStatWeightsL[0] +
							   Pin2.Band1Frequency * ClathriEarStatWeightsL[1] +
							   Pin3.Band1Frequency * ClathriEarStatWeightsL[2];
		float Band1AmountDb = Pin1.Band1AmountDb * ClathriEarStatWeightsL[0] +
							  Pin2.Band1AmountDb * ClathriEarStatWeightsL[1] +
							  Pin3.Band1AmountDb * ClathriEarStatWeightsL[2];
		float Band1Quality = Pin1.Band1Quality * ClathriEarStatWeightsL[0] +
							 Pin2.Band1Quality * ClathriEarStatWeightsL[1] +
							 Pin3.Band1Quality * ClathriEarStatWeightsL[2];
		float Band1MakeupDb = Pin1.Band1MakeupDb * ClathriEarStatWeightsL[0] +
							  Pin2.Band1MakeupDb * ClathriEarStatWeightsL[1] +
							  Pin3.Band1MakeupDb * ClathriEarStatWeightsL[2];

		float Band2Frequency = Pin1.Band2Frequency * ClathriEarStatWeightsL[0] +
							   Pin2.Band2Frequency * ClathriEarStatWeightsL[1] +
							   Pin3.Band2Frequency * ClathriEarStatWeightsL[2];
		float Band2AmountDb = Pin1.Band2AmountDb * ClathriEarStatWeightsL[0] +
							  Pin2.Band2AmountDb * ClathriEarStatWeightsL[1] +
							  Pin3.Band2AmountDb * ClathriEarStatWeightsL[2];
		float Band2Quality = Pin1.Band2Quality * ClathriEarStatWeightsL[0] +
							 Pin2.Band2Quality * ClathriEarStatWeightsL[1] +
							 Pin3.Band2Quality * ClathriEarStatWeightsL[2];
		float Band2MakeupDb = Pin1.Band2MakeupDb * ClathriEarStatWeightsL[0] +
							  Pin2.Band2MakeupDb * ClathriEarStatWeightsL[1] +
							  Pin3.Band2MakeupDb * ClathriEarStatWeightsL[2];

		float Band3Frequency = Pin1.Band3Frequency * ClathriEarStatWeightsL[0] +
							   Pin2.Band3Frequency * ClathriEarStatWeightsL[1] +
							   Pin3.Band3Frequency * ClathriEarStatWeightsL[2];
		float Band3AmountDb = Pin1.Band3AmountDb * ClathriEarStatWeightsL[0] +
							  Pin2.Band3AmountDb * ClathriEarStatWeightsL[1] +
							  Pin3.Band3AmountDb * ClathriEarStatWeightsL[2];
		float Band3Quality = Pin1.Band3Quality * ClathriEarStatWeightsL[0] +
							 Pin2.Band3Quality * ClathriEarStatWeightsL[1] +
							 Pin3.Band3Quality * ClathriEarStatWeightsL[2];
		float Band3MakeupDb = Pin1.Band3MakeupDb * ClathriEarStatWeightsL[0] +
							  Pin2.Band3MakeupDb * ClathriEarStatWeightsL[1] +
							  Pin3.Band3MakeupDb * ClathriEarStatWeightsL[2];

		FilterL[0].NotifyBufferChanged();
		FilterL[1].NotifyBufferChanged();
		FilterL[2].NotifyBufferChanged();
		FilterL[0].SetParams(EEQBandType::AttLow, Band1Frequency, Band1AmountDb, Band1Quality, Band1MakeupDb);
		FilterL[1].SetParams(EEQBandType::AttBand, Band2Frequency, Band2AmountDb, Band2Quality, Band2MakeupDb);
		FilterL[2].SetParams(EEQBandType::AttHigh, Band3Frequency, Band3AmountDb, Band3Quality, Band3MakeupDb);
	}

	// Right pins. 
	{
		TArray<FSortResult> Results;
		for (int32 Index = 0; Index < Settings->GetEarData().EarPinsR.Num(); ++Index)
		{
			FEarPin& Pin = Settings->GetEarData().EarPinsR[Index];
			float DotProduct = FVector::DotProduct(InEmitterNormal, Pin.Direction);
			Results.Add({ Index, DotProduct, &Pin });
		}

		TArray<float> Keys;
		Results.Sort([](const FSortResult& R1, const FSortResult& R2) { return R1.DotProduct > R2.DotProduct; });

		FEarPin& Pin1 = *Results[0].Pin;
		FEarPin& Pin2 = *Results[1].Pin;
		FEarPin& Pin3 = *Results[2].Pin;

		FQuat Q1 = FQuat::FindBetweenNormals(InEmitterNormal, Pin1.Direction.GetSafeNormal());
		FQuat Q2 = FQuat::FindBetweenNormals(InEmitterNormal, Pin2.Direction.GetSafeNormal());
		FQuat Q3 = FQuat::FindBetweenNormals(InEmitterNormal, Pin3.Direction.GetSafeNormal());

		ClathriEarStatWeightsR = FMath::ComputeBaryCentric2D(FVector::ZeroVector, Q1.Euler(), Q2.Euler(), Q3.Euler());
		ClathriEarStatIdxR1 = Results[0].Index;
		ClathriEarStatIdxR2 = Results[1].Index;
		ClathriEarStatIdxR3 = Results[2].Index;

		float Band1Frequency = Pin1.Band1Frequency * ClathriEarStatWeightsR[0] +
							   Pin2.Band1Frequency * ClathriEarStatWeightsR[1] +
							   Pin3.Band1Frequency * ClathriEarStatWeightsR[2];
		float Band1AmountDb = Pin1.Band1AmountDb * ClathriEarStatWeightsR[0] +
							  Pin2.Band1AmountDb * ClathriEarStatWeightsR[1] +
							  Pin3.Band1AmountDb * ClathriEarStatWeightsR[2];
		float Band1Quality = Pin1.Band1Quality * ClathriEarStatWeightsR[0] +
							 Pin2.Band1Quality * ClathriEarStatWeightsR[1] +
							 Pin3.Band1Quality * ClathriEarStatWeightsR[2];
		float Band1MakeupDb = Pin1.Band1MakeupDb * ClathriEarStatWeightsR[0] +
							  Pin2.Band1MakeupDb * ClathriEarStatWeightsR[1] +
							  Pin3.Band1MakeupDb * ClathriEarStatWeightsR[2];

		float Band2Frequency = Pin1.Band2Frequency * ClathriEarStatWeightsR[0] +
							   Pin2.Band2Frequency * ClathriEarStatWeightsR[1] +
							   Pin3.Band2Frequency * ClathriEarStatWeightsR[2];
		float Band2AmountDb = Pin1.Band2AmountDb * ClathriEarStatWeightsR[0] +
							  Pin2.Band2AmountDb * ClathriEarStatWeightsR[1] +
							  Pin3.Band2AmountDb * ClathriEarStatWeightsR[2];
		float Band2Quality = Pin1.Band2Quality * ClathriEarStatWeightsR[0] +
							 Pin2.Band2Quality * ClathriEarStatWeightsR[1] +
							 Pin3.Band2Quality * ClathriEarStatWeightsR[2];
		float Band2MakeupDb = Pin1.Band2MakeupDb * ClathriEarStatWeightsR[0] +
							  Pin2.Band2MakeupDb * ClathriEarStatWeightsR[1] +
							  Pin3.Band2MakeupDb * ClathriEarStatWeightsR[2];

		float Band3Frequency = Pin1.Band3Frequency * ClathriEarStatWeightsR[0] +
							   Pin2.Band3Frequency * ClathriEarStatWeightsR[1] +
							   Pin3.Band3Frequency * ClathriEarStatWeightsR[2];
		float Band3AmountDb = Pin1.Band3AmountDb * ClathriEarStatWeightsR[0] +
							  Pin2.Band3AmountDb * ClathriEarStatWeightsR[1] +
							  Pin3.Band3AmountDb * ClathriEarStatWeightsR[2];
		float Band3Quality = Pin1.Band3Quality * ClathriEarStatWeightsR[0] +
							 Pin2.Band3Quality * ClathriEarStatWeightsR[1] +
							 Pin3.Band3Quality * ClathriEarStatWeightsR[2];
		float Band3MakeupDb = Pin1.Band3MakeupDb * ClathriEarStatWeightsR[0] +
							  Pin2.Band3MakeupDb * ClathriEarStatWeightsR[1] +
							  Pin3.Band3MakeupDb * ClathriEarStatWeightsR[2];

		FilterR[0].NotifyBufferChanged();
		FilterR[1].NotifyBufferChanged();
		FilterR[2].NotifyBufferChanged();
		FilterR[0].SetParams(EEQBandType::AttLow, Band1Frequency, Band1AmountDb, Band1Quality, Band1MakeupDb);
		FilterR[1].SetParams(EEQBandType::AttBand, Band2Frequency, Band2AmountDb, Band2Quality, Band2MakeupDb);
		FilterR[2].SetParams(EEQBandType::AttHigh, Band3Frequency, Band3AmountDb, Band3Quality, Band3MakeupDb);
	}
}
