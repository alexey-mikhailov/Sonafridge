// Sonafridge 2022

#include "ClathrispaceSettings.h"
#include "Sonafridge/Tools/MathTools.h"
#include "Sonafridge/SonafridgeCommon.h"

UClathrispaceSettings::UClathrispaceSettings()
{
	EarData.EarPins.Add
	({
		FVector::RightVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPins.Add
	({
		FVector::LeftVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPins.Add
	({
		FVector::UpVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPins.Add
	({
		FVector::DownVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPins.Add
	({
		FVector::ForwardVector,
		true, 100.f, 0.f, 1.f, 0.f,
		true, 667.f, 0.f, 1.f, 0.f,
		true, 4444.f, 0.f, 1.f, 0.f,
	});
	EarData.EarPins.Add
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

void UClathrispaceSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ExternallyChanged.Broadcast();
}

