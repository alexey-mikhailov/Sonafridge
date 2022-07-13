// Sonafridge 2022

#include "EW_SonaQFrequencyResponse.h"
#include "EW_SonaQ.h"
#include "Model/VM_SonaQ.h"
#include "Sonafridge/ColorManagement/HSRColor.h"
#include "Sonafridge/MathTools.h"
#include "Input/HittestGrid.h"
#include "AudioDevice.h"

UEW_SonaQFrequencyResponse::UEW_SonaQFrequencyResponse()
{
	SizeChanged.AddUObject(this, &UEW_SonaQFrequencyResponse::OnSizeChanged);
}

void UEW_SonaQFrequencyResponse::Init(UEW_SonaQ* InRootWidget, TSharedPtr<FVM_SonaQ> InViewModel)
{
	RootWidget = InRootWidget;
	ViewModel = InViewModel;
	SampleRate = ViewModel->GetSampleRate();
	ViewModel->GetEvent_BandChanging().AddUObject(this, &UEW_SonaQFrequencyResponse::OnBandChanging);
	ViewModel->GetEvent_BandChanged().AddUObject(this, &UEW_SonaQFrequencyResponse::OnBandChanged);
	GridPointPairs.SetNumZeroed((7 + 10 + 10) + ((48.f + 42.f) / 6.f));
	ResponsePoints.SetNumZeroed(Resolution);
	BakeGrid();
	BakeResponse();
}

void UEW_SonaQFrequencyResponse::OnBandChanging(TSharedPtr<FVM_SonaQBand> InBand)
{
	BakeResponse();
}

void UEW_SonaQFrequencyResponse::OnBandChanged(TSharedPtr<FVM_SonaQBand> InBand)
{
	BakeResponse();
}

void UEW_SonaQFrequencyResponse::OnSizeChanged(const FVector2D& OldSize,
                                               const FVector2D& NewSize)
{
	LastSize = NewSize;
	BakeGrid();
	BakeResponse();
}

void UEW_SonaQFrequencyResponse::NativeConstruct()
{
	Super::NativeConstruct();
	SetClipping(EWidgetClipping::ClipToBoundsAlways);
}

FReply UEW_SonaQFrequencyResponse::NativeOnMouseButtonDown(const FGeometry& InGeometry, 
														   const FPointerEvent& InMouseEvent)
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
	for (auto Band : ViewModel->GetBands())
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
			PresstimeNAmountDb = (UEW_SonaQ::DynamicMax - Band->GetAmountDb())
			                   / (UEW_SonaQ::DynamicMax - UEW_SonaQ::DynamicMin);
			Reply = Reply.CaptureMouse(TakeWidget());
			break;
		}

		++Index;
	}

	ViewModel->SetSelectedBandIndex(PossessedBandIndex);

	return Reply;
}

FReply UEW_SonaQFrequencyResponse::NativeOnMouseButtonUp(const FGeometry& InGeometry, 
														 const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	Reply = Reply.ReleaseMouseCapture();
	PossessedBand = {};
	PossessedBandIndex = -1;
	ViewModel->GetEvent_BandChanged().Broadcast(PossessedBand);

	return Reply;
}

FReply UEW_SonaQFrequencyResponse::NativeOnMouseButtonDoubleClick(const FGeometry&     InGeometry,
                                                                  const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	float Frequency = MathLogTool::TribelToTwentieths(MousePos.X / LastSize.X);

	TSharedPtr<FVM_SonaQBand> Band = MakeShared<FVM_SonaQBand>();
	Band->Init(SampleRate);
	Band->SetType(EEQBandType::AttBand);
	Band->SetFrequency(Frequency);
	Band->SetQuality(1.f);
	Band->SetAmountDb(0.f);
	Band->SetMakeupDb(0.f);
	ViewModel->AddBand(Band);

	return Reply;
}

FReply UEW_SonaQFrequencyResponse::NativeOnMouseMove(const FGeometry&     InGeometry,
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
	for (auto Band : ViewModel->GetBands())
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
			ViewModel->GetEvent_BandChanging().Broadcast(PossessedBand);
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
			PossessedBand->SetAmountDb(UEW_SonaQ::DynamicMax - NNewY * (UEW_SonaQ::DynamicMax - UEW_SonaQ::DynamicMin));
			ViewModel->GetEvent_BandChanging().Broadcast(PossessedBand);
		}
	}

	bWasLeftMouseButtonPressed = bLeft;
	LastMousePos = MousePos;
	return Reply;
}

int32 UEW_SonaQFrequencyResponse::NativePaint(const FPaintArgs&        Args,
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

	if (!ViewModel) return Result;

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

	static const FLinearColor BandPointEnabledHoveredColor = HSR(.617f, 1.f, 1.f);
	static const FLinearColor BandPointEnabledSelectedColor = HSR(.617f, 1.f, .833f);
	static const FLinearColor BandPointEnabledNormalColor = HSR(.617f, 1.f, .667f);
	static const FLinearColor BandPointDisabledHoveredColor = HSR(.617f, 1.f, .5f);
	static const FLinearColor BandPointDisabledSelectedColor = HSR(.617f, 1.f, .333f);
	static const FLinearColor BandPointDisabledNormalColor = HSR(.617f, 1.f, .167f);

	FLinearColor PointColor = BandPointEnabledNormalColor;

	for (int32 Index = 0; Index < BandPoints.Num(); ++Index)
	{
		TSharedPtr<FVM_SonaQBand> CurrentBand;
		if (ViewModel->GetBands().Num() > Index)
		{
			CurrentBand = ViewModel->GetBands()[Index];
		}

		FVector2D P1 = BandPoints[Index] + FVector2D(0.f, -1.f);
		FVector2D P2 = BandPoints[Index] + FVector2D(0.f, +1.f);

		if (CurrentBand)
		{
			if (CurrentBand->GetIsEnabled())
			{
				if (Index == HoverBandIndex)
				{
					PointColor = BandPointEnabledHoveredColor;
				}
				else if (ViewModel->GetSelectedBandIndex() == Index)
				{
					PointColor = BandPointEnabledSelectedColor;
				}
				else
				{
					PointColor = BandPointEnabledNormalColor;
				}
			}
			else
			{
				if (Index == HoverBandIndex)
				{
					PointColor = BandPointDisabledHoveredColor;
				}
				else if (ViewModel->GetSelectedBandIndex() == Index)
				{
					PointColor = BandPointDisabledSelectedColor;
				}
				else
				{
					PointColor = BandPointDisabledNormalColor;
				}
			}
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

void UEW_SonaQFrequencyResponse::BakeGrid()
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

void UEW_SonaQFrequencyResponse::BakeResponse()
{
	if (!ViewModel) return;

	const float W = LastSize.X;
	const float H = LastSize.Y;

	for (int32 I = 0; I < Resolution; ++I)
	{
		auto Frequency = FMath::Exp(I * FLS + FLMin);
		float Response = ViewModel->DtftDb(Frequency);

		float Y = (Response              - UEW_SonaQ::DynamicMin)
		        / (UEW_SonaQ::DynamicMax - UEW_SonaQ::DynamicMin);

		ResponsePoints[I] = 
		{
			W * I * ResolutionStep,
			H * (1.f - Y)
		};
	}

	BandPoints.SetNumZeroed(ViewModel->GetBands().Num());

	int32 Index = 0;
	for (const auto& Band : ViewModel->GetBands())
	{
		float F = Band->GetFrequency();
		float X = MathLogTool::TwentiethsToTribel(F);
		float Response = ViewModel->DtftDb(F);
		float Y = (Response - UEW_SonaQ::DynamicMin) / (UEW_SonaQ::DynamicMax - UEW_SonaQ::DynamicMin);

		if (BandPoints.Num() > Index)
		{
			BandPoints[Index] = { W * X, H * (1.f - Y) };
		}

		++Index;
	}
}

FVector2D UEW_SonaQFrequencyResponse::GetBandWPos(TSharedPtr<FVM_SonaQBand> InBand)
{
	if (InBand)
	{
		float F = InBand->GetFrequency();
		float NX = MathLogTool::TwentiethsToTribel(F);
		float NY = (UEW_SonaQ::DynamicMax - ViewModel->DtftDb(F))
		         / (UEW_SonaQ::DynamicMax - UEW_SonaQ::DynamicMin);
		float WX = NX * LastSize.X;
		float WY = NY * LastSize.Y;
		return { WX, WY };
	}

	return {};
}
