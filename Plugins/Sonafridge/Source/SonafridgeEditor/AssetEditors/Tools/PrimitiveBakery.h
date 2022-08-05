// Sonafridge 2022

#pragma once

#include "CoreMinimal.h"


struct FPrimitiveCake
{
	TArray<FDynamicMeshVertex> Vertices;
	TArray<uint32> Indices;
};


class PrimitiveBakery
{
public:
	static FPrimitiveCake BuildSphere(float Radius,
	                                  int32 TesselationOrder);

	static void DrawSphere(FPrimitiveDrawInterface*  PDI,
	                       const FPrimitiveCake&     Cake,
	                       const FMatrix&            Matrix,
	                       const UMaterialInterface* Material);
};