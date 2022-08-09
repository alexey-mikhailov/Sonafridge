// Sonafridge 2022

#include "PrimitiveBakery.h"

#include "MeshPassProcessor.h"
#include "Sonafridge/SonafridgeCommon.h"

TArray<FVector> TesselateQuad(const FVector& V1,
                              const FVector& V2,
                              const FVector& V3,
                              const FVector& V4,
                              int32          Order)
{
	if (Order < 0)
	{
		UE_LOG(LogSonafridgeEditor, Error, TEXT("TesselateQuad: Argument 'Order' is less than 0. "));
		return {};
	}

	if (Order == 0)
	{
		return { V1, V2, V3, V3, V2, V4 };
	}

	TArray<FVector> Result;
	Result.Reserve(3 * FMath::Pow(4, Order));

	FVector V12 = FMath::Lerp(V1, V2, .5f);
	FVector V13 = FMath::Lerp(V1, V3, .5f);
	FVector V24 = FMath::Lerp(V2, V4, .5f);
	FVector V34 = FMath::Lerp(V3, V4, .5f);
	FVector V14 = FMath::Lerp(V1, V4, .5f);

	Result.Append(TesselateQuad(V1, V12, V13, V14, Order - 1));
	Result.Append(TesselateQuad(V12, V2, V14, V24, Order - 1));
	Result.Append(TesselateQuad(V13, V14, V3, V34, Order - 1));
	Result.Append(TesselateQuad(V14, V24, V34, V4, Order - 1));

	return Result;
}

FPrimitiveCake PrimitiveBakery::BuildSphere(float Radius,
                                            int32 TesselationOrder)
{
	constexpr float Cs45 = 0.7071f;
	FPrimitiveCake Result;

	Result.Vertices.SetNum(36 * FMath::Pow(4, TesselationOrder));
	Result.Indices.SetNum(36 * FMath::Pow(4, TesselationOrder));

	FVector V1 = { -Cs45, -Cs45, -Cs45 };
	FVector V2 = { +Cs45, -Cs45, -Cs45 };
	FVector V3 = { -Cs45, +Cs45, -Cs45 };
	FVector V4 = { +Cs45, +Cs45, -Cs45 };
	FVector V5 = { -Cs45, -Cs45, +Cs45 };
	FVector V6 = { +Cs45, -Cs45, +Cs45 };
	FVector V7 = { -Cs45, +Cs45, +Cs45 };
	FVector V8 = { +Cs45, +Cs45, +Cs45 };

	TArray<FVector> Normals;
	Normals.Reserve(24 * FMath::Pow(4, TesselationOrder));
	Normals.Append(TesselateQuad(V1, V2, V3, V4, TesselationOrder));
	Normals.Append(TesselateQuad(V5, V6, V7, V8, TesselationOrder));
	Normals.Append(TesselateQuad(V1, V2, V5, V6, TesselationOrder));
	Normals.Append(TesselateQuad(V3, V4, V7, V8, TesselationOrder));
	Normals.Append(TesselateQuad(V1, V3, V5, V7, TesselationOrder));
	Normals.Append(TesselateQuad(V2, V4, V6, V8, TesselationOrder));

	uint32 Index = 0ui32;
	for (const FVector& N : Normals)
	{
		Result.Vertices[Index] = N.GetUnsafeNormal() * Radius;
		Result.Indices[Index] = Index;
		++Index;
	}

	return Result;
}

void PrimitiveBakery::DrawSphere(FPrimitiveDrawInterface*  PDI,
                                 const FPrimitiveCake&     Cake,
                                 const FMatrix&            Matrix,
                                 const UMaterialInterface* Material,
                                 const FHitProxyId         HitProxyId)
{
	if (!PDI)
	{
		return;
	}

	const ERHIFeatureLevel::Type FeatureLevel = PDI->View->FeatureLevel;

	FMeshBatch          Mesh;
	FDynamicMeshBuilder MeshBuilder(FeatureLevel);
	MeshBuilder.AddVertices(Cake.Vertices);
	MeshBuilder.AddTriangles(Cake.Indices);
	MeshBuilder.Draw(PDI, Matrix, Material->GetRenderProxy(), SDPG_Foreground, false, true, HitProxyId);
}
