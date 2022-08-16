// Sonafridge 2022

#pragma once
#include "CoreMinimal.h"

class FClathrispace;


class FClathriEarStatDrawer final : public TSharedFromThis<FClathriEarStatDrawer>
{
public:
	FClathriEarStatDrawer(FClathrispace* InClathrispace);

	void Initialize();
	void Shutdown();

private:
	int32 RenderClathriEarStat(UWorld*         InWorld,
	                           FViewport*      InViewport,
	                           FCanvas*        Canvas,
	                           int32           X,
	                           int32           Y,
	                           const FVector*  Vector,
	                           const FRotator* Rotator);

	bool ToggleClathriEarStat(UWorld* InWorld, 
							  FCommonViewportClient* InClient, 
							  const TCHAR* Stream);

	FClathrispace*            Context = nullptr;
};

