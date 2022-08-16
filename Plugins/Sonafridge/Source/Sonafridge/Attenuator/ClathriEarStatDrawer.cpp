// Sonafridge 2022

#include "ClathriEarStatDrawer.h"

#include "CanvasItem.h"
#include "Clathrispace.h"
#include "Sonafridge/Tools/HSRColor.h"

FClathriEarStatDrawer::FClathriEarStatDrawer(FClathrispace* InClathrispace)
	: Context(InClathrispace)
{
}

void FClathriEarStatDrawer::Initialize()
{
	if (GEngine)
	{
		GEngine->AddEngineStat(TEXT("STAT_Clathrispace"),
							   TEXT("STATCAT_Sonafridge"),
							   NSLOCTEXT("Sonafridge", "ClathrispaceStatDesc", "Displays stats for Clathrispace."),
							   UEngine::FEngineStatRender::CreateSP(this, &FClathriEarStatDrawer::RenderClathriEarStat),
							   UEngine::FEngineStatToggle::CreateSP(this, &FClathriEarStatDrawer::ToggleClathriEarStat),
							   false);
	}
}

void FClathriEarStatDrawer::Shutdown()
{
	if (GEngine)
	{
		GEngine->RemoveEngineStat(TEXT("STAT_Clathrispace"));
	}
}

int32 FClathriEarStatDrawer::RenderClathriEarStat(UWorld*         InWorld,
                                                  FViewport*      InViewport,
                                                  FCanvas*        Canvas,
                                                  int32           X,
                                                  int32           Y,
                                                  const FVector*  Vector,
                                                  const FRotator* Rotator)
{
	if (!Context) return 0;

	FIntPoint ViewportSize = InViewport->GetSizeXY();
	float SW = ViewportSize.X;
	float SH = ViewportSize.Y;
	float SWeightL1 = SH * Context->ClathriEarStatWeightsL.X;
	float SWeightL2 = SH * Context->ClathriEarStatWeightsL.Y;
	float SWeightL3 = SH * Context->ClathriEarStatWeightsL.Z;
	float SWeightR1 = SH * Context->ClathriEarStatWeightsR.X;
	float SWeightR2 = SH * Context->ClathriEarStatWeightsR.Y;
	float SWeightR3 = SH * Context->ClathriEarStatWeightsR.Z;

	constexpr float SThickness = 40.f;
	constexpr float SMargin = 20.f;

	FCanvasTileItem TileL1 = FCanvasTileItem({SMargin,                   0.f},                   { SThickness, SWeightL1 }, HSR(0.617f, .50f, .05f));
	FCanvasTileItem TileL2 = FCanvasTileItem({SMargin,                   SWeightL1},             { SThickness, SWeightL2 }, HSR(0.617f, .75f, .10f));
	FCanvasTileItem TileL3 = FCanvasTileItem({SMargin,                   SWeightL1 + SWeightL2}, { SThickness, SWeightL3 }, HSR(0.617f, 1.0f, .40f));
	FCanvasTileItem TileR1 = FCanvasTileItem({SW - SMargin - SThickness, 0.f},                   { SThickness, SWeightR1 }, HSR(0.617f, .50f, .05f));
	FCanvasTileItem TileR2 = FCanvasTileItem({SW - SMargin - SThickness, SWeightR1},             { SThickness, SWeightR2 }, HSR(0.617f, .75f, .10f));
	FCanvasTileItem TileR3 = FCanvasTileItem({SW - SMargin - SThickness, SWeightR1 + SWeightR2}, { SThickness, SWeightR3 }, HSR(0.617f, 1.0f, .40f));

	FCanvasTextItem TextL1 = FCanvasTextItem({SMargin + .5f * SThickness, 0.f + .5f * SWeightL1},
	                                         FText::FromString(FString::Printf(TEXT("%i"), Context->ClathriEarStatIdxL1)),
	                                         GEngine->GetLargeFont(),
	                                         FLinearColor::White);

	FCanvasTextItem TextL2 = FCanvasTextItem({SMargin + .5f * SThickness, SWeightL1 + .5f * SWeightL2},
	                                         FText::FromString(FString::Printf(TEXT("%i"), Context->ClathriEarStatIdxL2)),
	                                         GEngine->GetSubtitleFont(),
	                                         FLinearColor::White);

	FCanvasTextItem TextL3 = FCanvasTextItem({SMargin + .5f * SThickness, SWeightL1 + SWeightL2 + .5f * SWeightL3},
	                                         FText::FromString(FString::Printf(TEXT("%i"), Context->ClathriEarStatIdxL3)),
	                                         GEngine->GetLargeFont(),
	                                         FLinearColor::White);

	FCanvasTextItem TextR1 = FCanvasTextItem({SW - SMargin - .5f * SThickness, 0.f + .5f * SWeightR1},
	                                         FText::FromString(FString::Printf(TEXT("%i"), Context->ClathriEarStatIdxR1)),
	                                         GEngine->GetMediumFont(),
	                                         FLinearColor::White);

	FCanvasTextItem TextR2 = FCanvasTextItem({SW - SMargin - .5f * SThickness, SWeightR1 + .5f * SWeightR2},
	                                         FText::FromString(FString::Printf(TEXT("%i"), Context->ClathriEarStatIdxR2)),
	                                         GEngine->GetSmallFont(),
	                                         FLinearColor::White);

	FCanvasTextItem TextR3 = FCanvasTextItem({SW - SMargin - .5f * SThickness, SWeightR1 + SWeightR2 + .5f * SWeightR3},
	                                         FText::FromString(FString::Printf(TEXT("%i"), Context->ClathriEarStatIdxR3)),
	                                         GEngine->GetTinyFont(),
	                                         FLinearColor::White);

	TileL1.MaterialRenderProxy = GEngine->EmissiveMeshMaterial->GetRenderProxy();
	TileL2.MaterialRenderProxy = GEngine->EmissiveMeshMaterial->GetRenderProxy();
	TileL3.MaterialRenderProxy = GEngine->EmissiveMeshMaterial->GetRenderProxy();

	Canvas->DrawItem(TileL1);
	Canvas->DrawItem(TileL2);
	Canvas->DrawItem(TileL3);
	Canvas->DrawItem(TileR1);
	Canvas->DrawItem(TileR2);
	Canvas->DrawItem(TileR3);
	Canvas->DrawItem(TextL1);
	Canvas->DrawItem(TextL2);
	Canvas->DrawItem(TextL3);
	Canvas->DrawItem(TextR1);
	Canvas->DrawItem(TextR2);
	Canvas->DrawItem(TextR3);

	return 0;
}

bool FClathriEarStatDrawer::ToggleClathriEarStat(UWorld*                InWorld,
                                                 FCommonViewportClient* InClient,
                                                 const TCHAR*           Stream)
{
	return false;
}