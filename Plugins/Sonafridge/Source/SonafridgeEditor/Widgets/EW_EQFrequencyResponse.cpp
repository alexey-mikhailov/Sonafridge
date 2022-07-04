// Sonafridge 2022

#include "EW_EQFrequencyResponse.h"
#include "AudioDevice.h"
#include "EW_EQ.h"
#include "Sonafridge/MathTools.h"
#include "Sonafridge/Settings_EQ/EQSettingsMock.h"
#include "Input/HittestGrid.h"

UEW_EQFrequencyResponse::UEW_EQFrequencyResponse()
{
	SizeChanged.AddUObject(this, &UEW_EQFrequencyResponse::OnSizeChanged);
}

void UEW_EQFrequencyResponse::Init(UEW_EQ* InRootWidget, TSharedPtr<IEQSettings> InSettings)
{
	RootWidget = InRootWidget;
	Settings = InSettings;
	SampleRate = Settings->GetSampleRate();
	RootWidget->GetEvent_BandChanging().AddUObject(this, &UEW_EQFrequencyResponse::OnBandChanging);
	RootWidget->GetEvent_BandChanged().AddUObject(this, &UEW_EQFrequencyResponse::OnBandChanged);
	GridPointPairs.SetNumZeroed((7 + 10 + 10) + ((48.f + 42.f) / 6.f));
	ResponsePoints.SetNumZeroed(Resolution);
	BakeGrid();
	BakeResponse();
}

void UEW_EQFrequencyResponse::OnBandChanging(TSharedPtr<FEQBand> InBand)
{
	BakeResponse();
}

void UEW_EQFrequencyResponse::OnBandChanged(TSharedPtr<FEQBand> InBand)
{
	BakeResponse();
}

void UEW_EQFrequencyResponse::OnSizeChanged(const FVector2D& OldSize,
                                            const FVector2D& NewSize)
{
	LastSize = NewSize;
	BakeGrid();
	BakeResponse();
}

void UEW_EQFrequencyResponse::NativeConstruct()
{
	Super::NativeConstruct();
	SetClipping(EWidgetClipping::ClipToBoundsAlways);
}

FReply UEW_EQFrequencyResponse::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	MousePos =
	{
		FMath::Clamp(MousePos.X, 0.f, LastSize.X),
		FMath::Clamp(MousePos.Y, 0.f, LastSize.Y),
	};

	PossessedBandIndex = -1;

	int32 Index = 0;
	for (auto Band : Settings->GetBands())
	{
		FVector2D       WBandPos = GetBandWPos(Band);
		constexpr float ScreenSpaceTolerance = 10.f;

		if (FMath::Abs(MousePos.X - WBandPos.X) < ScreenSpaceTolerance && 
			FMath::Abs(MousePos.Y - WBandPos.Y) < ScreenSpaceTolerance)
		{
			PossessedBand = Band;
			PossessedBandIndex = Index;
			PresstimeMousePos = MousePos;
			PresstimeFrequency = Band->GetFrequency();
			PresstimeNAmountDb = (UEW_EQ::DynamicMax - Band->GetAmountDb())
			                   / (UEW_EQ::DynamicMax - UEW_EQ::DynamicMin);
			Reply = Reply.CaptureMouse(TakeWidget());
			break;
		}

		++Index;
	}

	RootWidget->SetSelectedBandIndex(PossessedBandIndex);

	return Reply;
}

FReply UEW_EQFrequencyResponse::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	Reply = Reply.ReleaseMouseCapture();
	PossessedBand = {};
	PossessedBandIndex = -1;
	RootWidget->GetEvent_BandChanging().Broadcast(PossessedBand);

	return Reply;
}

FReply UEW_EQFrequencyResponse::NativeOnMouseButtonDoubleClick(const FGeometry&     InGeometry,
                                                               const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	float Frequency = MathLogTool::TribelToTwentieths(MousePos.X / LastSize.X);

	TSharedPtr<FEQBand> Band = MakeShared<FEQBand>();
	Band->Init(SampleRate);
	Band->SetType(EBandType::BandCut);
	Band->SetFrequency(Frequency);
	Band->SetQuality(1.f);
	Band->SetAmountDb(0.f);
	Band->SetLoudCompDb(0.f);
	Settings->AddBand(Band);

	return Reply;
}

FReply UEW_EQFrequencyResponse::NativeOnMouseMove(const FGeometry& InGeometry, 
                                                  const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	MousePos =
	{
		FMath::Clamp(MousePos.X, 0.f, LastSize.X),
		FMath::Clamp(MousePos.Y, 0.f, LastSize.Y),
	};

	FVector2D WDelta = MousePos - LastMousePos;
	HoverBandIndex = -1;

	bool bLeft = InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton);
	bool bCtrl = InMouseEvent.IsControlDown();

	int32 Index = 0;
	for (auto Band : Settings->GetBands())
	{
		FVector2D       WBandPos = GetBandWPos(Band);
		constexpr float ScreenSpaceTolerance = 10.f;

		if (FMath::Abs(MousePos.X - WBandPos.X) < ScreenSpaceTolerance && 
			FMath::Abs(MousePos.Y - WBandPos.Y) < ScreenSpaceTolerance)
		{
			HoverBandIndex = Index;
			break;
		}

		++Index;
	}

	if (PossessedBand)
	{
		if (bCtrl)
		{
			//
			// Drag Quality
			//
			
			float QDelta = MathLogTool::HexabelToLinear(WDelta.Y / LastSize.Y);
			PossessedBand->SetQuality(PossessedBand->GetQuality() * QDelta);
			RootWidget->GetEvent_BandChanged().Broadcast(PossessedBand);
		}
		else
		{
			//
			// Drag Frequency & Amount
			//

			FVector2D WDiff = MousePos - PresstimeMousePos;
			FVector2D NDiff = WDiff / LastSize;
			float FCoef = MathLogTool::TribelToLinear(NDiff.X);
			float F = PresstimeFrequency * FCoef;
			float NNewY = PresstimeNAmountDb + NDiff.Y;

			PossessedBand->SetFrequency(F);
			PossessedBand->SetAmountDb(UEW_EQ::DynamicMax - NNewY * (UEW_EQ::DynamicMax - UEW_EQ::DynamicMin));
			RootWidget->GetEvent_BandChanged().Broadcast(PossessedBand);
		}
	}

	bWasLeftMouseButtonPressed = bLeft;
	LastMousePos = MousePos;
	return Reply;
}

int32 UEW_EQFrequencyResponse::NativePaint(const FPaintArgs&        Args,
                                           const FGeometry&         AllottedGeometry,
                                           const FSlateRect&        MyCullingRect,
                                           FSlateWindowElementList& OutDrawElements,
                                           int32                    LayerId,
                                           const FWidgetStyle&      InWidgetStyle,
                                           bool                     bParentEnabled) const
{
	int32 Result = Super::NativePaint(Args,
	                                  AllottedGeometry,
	                                  MyCullingRect,
	                                  OutDrawElements,
	                                  LayerId,
	                                  InWidgetStyle,
	                                  bParentEnabled);

	if (!Settings) return Result;

	FVector2D Size = AllottedGeometry.GetLocalSize();
	if (Size != LastSize)
	{
		SizeChanged.Broadcast(LastSize, Size);
	}

	constexpr FLinearColor GridColor1 = FLinearColor(1.f, 1.f, 1.f, .25f);
	constexpr FLinearColor GridColor2 = FLinearColor(1.f, 1.f, 1.f, .75f);
	for (int32 Index = 0; Index < GridPointPairs.Num(); ++Index)
	{
		FLinearColor Color = 
			Index == 8 || 
			Index == 17 || 
			Index == 26 ||
			Index == 34
		? GridColor2
		: GridColor1;

		TArray<FVector2D> Pair = GridPointPairs[Index];
		FSlateDrawElement::MakeLines(OutDrawElements,
		                             LayerId,
		                             AllottedGeometry.ToPaintGeometry(),
		                             Pair,
		                             ESlateDrawEffect::None,
		                             Color,
		                             true,
		                             .5f);
	}

	constexpr FLinearColor RespColor = FLinearColor(0.f, .25f, 1.f, 1.f);
	FSlateDrawElement::MakeLines(OutDrawElements,
	                             LayerId,
	                             AllottedGeometry.ToPaintGeometry(),
	                             ResponsePoints,
	                             ESlateDrawEffect::None,
	                             RespColor,
	                             true,
	                             3.f);

	constexpr FLinearColor BandPointNormalColor = FLinearColor(.25f, .625f, 1.f, 1.f);
	constexpr FLinearColor BandPointHoveredColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
	constexpr FLinearColor BandPointSelectedColor = FLinearColor(.75f, .875f, 1.f, 1.f);
	FLinearColor PointColor = BandPointNormalColor;

	for (int32 Index = 0; Index < BandPoints.Num(); ++Index)
	{
		auto P1 = BandPoints[Index] + FVector2D(0.f, -1.f);
		auto P2 = BandPoints[Index] + FVector2D(0.f, +1.f);

		if (Index == HoverBandIndex)
		{
			PointColor = BandPointHoveredColor;
		}
		else if (RootWidget->GetSelectedBandIndex() == Index)
		{
			PointColor = BandPointSelectedColor;
		}
		else
		{
			PointColor = BandPointNormalColor;
		}

		FSlateDrawElement::MakeLines(OutDrawElements,
		                             LayerId,
		                             AllottedGeometry.ToPaintGeometry(),
									 { P1, P2 },
		                             ESlateDrawEffect::None,
		                             PointColor,
		                             true,
		                             12.f);
	}

	return Result;
}

void UEW_EQFrequencyResponse::BakeGrid()
{
	const float W = LastSize.X;
	const float H = LastSize.Y;

	int32 PairIndex = 0;

	// Frequency bars
	int32 Mantissa = 2;
	int32 Radix = 10;
	int32 Power = Radix;
	int32 Frequency = Mantissa * Power;

	while (Frequency < 20000)
	{
		while (Frequency < 20000 && Mantissa < Radix)
		{
			// Normalized x-coordinate. 
			float NxCoord = MathLogTool::TwentiethsToTribel(Frequency);
			GridPointPairs[PairIndex++] = 
			{
				{ W * NxCoord, 0.f },
				{ W * NxCoord, H   }
			};

			++Mantissa;
			Frequency = Mantissa * Power;
		}

		Mantissa = 1;
		Power *= Radix;
		Frequency = Mantissa * Power;
	}

	// Amplitude change bars
	for (int32 Index = 1; Index < 16; ++Index)
	{
		// Normalized y-coordinate
		float NyCoord = 6.f * Index / 96.f;

		GridPointPairs[PairIndex++] =
		{
			{ 0.f, H * NyCoord },
			{ W,   H * NyCoord }
		};
	}
}

void UEW_EQFrequencyResponse::BakeResponse()
{
	if (!Settings) return;

	const float W = LastSize.X;
	const float H = LastSize.Y;

	for (int32 I = 0; I < Resolution; ++I)
	{
		auto Frequency = FMath::Exp(I * FLS + FLMin);
		float Response = Settings->DtftDb(Frequency);

		float Y = (Response           - UEW_EQ::DynamicMin)
		        / (UEW_EQ::DynamicMax - UEW_EQ::DynamicMin);

		ResponsePoints[I] = 
		{
			W * I * ResolutionStep,
			H * (1.f - Y)
		};
	}

	BandPoints.SetNumZeroed(Settings->GetBands().Num());

	int32 Index = 0;
	for (const auto& Band : Settings->GetBands())
	{
		if (Band->GetType() == EBandType::BandCut)
		{
			float F = Band->GetFrequency();
			float X = MathLogTool::TwentiethsToTribel(F);
			float Response = Settings->DtftDb(F);
			float Y = (Response - UEW_EQ::DynamicMin) / (UEW_EQ::DynamicMax - UEW_EQ::DynamicMin);

			if (BandPoints.Num() > Index)
			{
				BandPoints[Index] = { W * X, H * (1.f - Y) };
			}

			++Index;
		}
	}
}

FVector2D UEW_EQFrequencyResponse::GetBandWPos(TSharedPtr<FEQBand> InBand)
{
	if (InBand)
	{
		float F = InBand->GetFrequency();
		float NX = MathLogTool::TwentiethsToTribel(F);
		float NY = (UEW_EQ::DynamicMax - Settings->DtftDb(F))
		         / (UEW_EQ::DynamicMax - UEW_EQ::DynamicMin);
		float WX = NX * LastSize.X;
		float WY = NY * LastSize.Y;
		return { WX, WY };
	}

	return {};
}
