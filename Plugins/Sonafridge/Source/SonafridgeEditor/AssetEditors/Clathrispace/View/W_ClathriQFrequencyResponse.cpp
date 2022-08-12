// Sonafridge 2022

#include "W_ClathriQFrequencyResponse.h"
#include "W_ClathriQ.h"
#include "SonafridgeEditor/AssetEditors/Clathrispace/ViewModel/VM_ClathriQ.h"
#include "Sonafridge/Attenuator/Clathrispace.h"
#include "Sonafridge/Tools/HSRColor.h"
#include "Sonafridge/Tools/MathTools.h"
#include "Input/HittestGrid.h"
#include "AudioDevice.h"

UW_ClathriQFrequencyResponse::UW_ClathriQFrequencyResponse()
{
	BandPoints.SetNumZeroed(UClathrispaceSettings::BandCount);
	SizeChanged.AddUObject(this, &UW_ClathriQFrequencyResponse::OnSizeChanged);
}

void UW_ClathriQFrequencyResponse::Init(UW_ClathriQ* InRootWidget, TSharedPtr<FVM_ClathriQ> InViewModel)
{
	RootWidget = InRootWidget;
	ViewModel = InViewModel;
	SampleRate = ViewModel->GetSampleRate();
	ViewModel->GetEvent_PinIndexChanged().AddUObject(this, &UW_ClathriQFrequencyResponse::OnPinIndexChanged);
	ViewModel->GetEvent_BandChanging().AddUObject(this, &UW_ClathriQFrequencyResponse::OnBandChanging);
	ViewModel->GetEvent_BandChanged().AddUObject(this, &UW_ClathriQFrequencyResponse::OnBandChanged);
	GridPointPairs.SetNumZeroed((7 + 10 + 10) + ((48.f + 42.f) / 6.f));
	ResponsePoints.SetNumZeroed(FVM_ClathriQ::Resolution);
	BakeGrid();
	BakeResponse();
}

void UW_ClathriQFrequencyResponse::OnPinIndexChanged(int32 NewPinIndex)
{
	PinIndex = NewPinIndex;
	BakeResponse();
}

void UW_ClathriQFrequencyResponse::OnBandChanging(int32 BandIndex)
{
	BakeResponse();
}

void UW_ClathriQFrequencyResponse::OnBandChanged(int32 BandIndex)
{
	BakeResponse();
}

void UW_ClathriQFrequencyResponse::OnSizeChanged(const FVector2D& OldSize,
                                              const FVector2D& NewSize)
{
	LastSize = NewSize;
	BakeGrid();
	BakeResponse();
}

void UW_ClathriQFrequencyResponse::NativeConstruct()
{
	Super::NativeConstruct();
	SetClipping(EWidgetClipping::ClipToBoundsAlways);
}

FReply UW_ClathriQFrequencyResponse::NativeOnMouseButtonDown(const FGeometry&     InGeometry,
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

	for (int32 Index = 0; Index < UClathrispaceSettings::BandCount; ++Index)
	{
		FVector2D       WBandPos = GetBandWPos(Index);
		constexpr float ScreenSpaceTolerance = 10.f;

		if (FMath::Abs(MousePos.X - WBandPos.X) < ScreenSpaceTolerance && 
			FMath::Abs(MousePos.Y - WBandPos.Y) < ScreenSpaceTolerance)
		{
			PossessedBandIndex = Index;
			PresstimeMousePos = MousePos;
			PresstimeFrequency = ViewModel->GetFrequency(Index);
			PresstimeMakeupDb = ViewModel->GetMakeupDb(Index);
			PresstimeAvgDb = ViewModel->GetResponseAvgDb(Index);
			PresstimeNAmount = (FVM_ClathriQ::DynamicMax - ViewModel->GetAmountDb(Index))
			                 / (FVM_ClathriQ::DynamicMax - FVM_ClathriQ::DynamicMin);
			PresstimeNMakeup = (FVM_ClathriQ::DynamicMax - ViewModel->GetMakeupDb(Index))
			                 / (FVM_ClathriQ::DynamicMax - FVM_ClathriQ::DynamicMin);
			Reply = Reply.CaptureMouse(TakeWidget());
			break;
		}
	}

	ViewModel->SetSelectedBandIndex(PossessedBandIndex);

	return Reply;
}

FReply UW_ClathriQFrequencyResponse::NativeOnMouseButtonUp(const FGeometry&     InGeometry,
                                                           const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	Reply = Reply.ReleaseMouseCapture();
	PossessedBandIndex = INDEX_NONE;
	ViewModel->GetEvent_BandChanged().Broadcast(PossessedBandIndex);

	return Reply;
}

FReply UW_ClathriQFrequencyResponse::NativeOnMouseMove(const FGeometry&     InGeometry,
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

	bool bLeft =  InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton);
	bool bShift = InMouseEvent.IsShiftDown();
	bool bCtrl =  InMouseEvent.IsControlDown();
	bool bAlt =   InMouseEvent.IsAltDown();

	// You cannot drag over several instances of this widget class.
	// So, static context is justified. 
	static bool bWasShiftPressed = bShift;
	static bool bWasCtrlPressed = bCtrl;
	static bool bWasAltPressed = bAlt;

	if (PossessedBandIndex != INDEX_NONE)
	{
		if (!bWasShiftPressed && bShift || bWasShiftPressed && !bShift ||
			!bWasCtrlPressed && bCtrl || bWasCtrlPressed && !bCtrl ||
			!bWasAltPressed && bAlt || bWasAltPressed && !bAlt)
		{
			PresstimeMousePos = MousePos;
			PresstimeFrequency = ViewModel->GetFrequency(PossessedBandIndex);
			PresstimeMakeupDb = ViewModel->GetMakeupDb(PossessedBandIndex);
			PresstimeAvgDb = ViewModel->GetResponseAvgDb(PossessedBandIndex);
			PresstimeNAmount = (FVM_ClathriQ::DynamicMax - ViewModel->GetAmountDb(PossessedBandIndex))
			                 / (FVM_ClathriQ::DynamicMax - FVM_ClathriQ::DynamicMin);
			PresstimeNMakeup = (FVM_ClathriQ::DynamicMax - ViewModel->GetMakeupDb(PossessedBandIndex))
			                 / (FVM_ClathriQ::DynamicMax - FVM_ClathriQ::DynamicMin);
		}
	}

	bWasShiftPressed = bShift;
	bWasCtrlPressed = bCtrl;
	bWasAltPressed = bAlt;

	for (int32 Index = 0; Index < UClathrispaceSettings::BandCount; ++Index)
	{
		FVector2D       WBandPos = GetBandWPos(Index);
		constexpr float ScreenSpaceTolerance = 10.f;

		if (FMath::Abs(MousePos.X - WBandPos.X) < ScreenSpaceTolerance && 
			FMath::Abs(MousePos.Y - WBandPos.Y) < ScreenSpaceTolerance)
		{
			HoverBandIndex = Index;
			break;
		}

		++Index;
	}

	if (PossessedBandIndex != INDEX_NONE && 
		ViewModel->GetIsEnabled(PossessedBandIndex))
	{
		if (bShift)
		{
			//
			// Drag Makeup
			//
			
			FVector2D WDiff = MousePos - PresstimeMousePos;
			FVector2D NDiff = WDiff / LastSize;
			float NNewY = PresstimeNMakeup + NDiff.Y;

			ViewModel->SetMakeupDb
			(
				PossessedBandIndex, 
				FVM_ClathriQ::DynamicMax - NNewY * (FVM_ClathriQ::DynamicMax - FVM_ClathriQ::DynamicMin)
			);
		}
		else if (bCtrl)
		{
			//
			// Drag Quality
			//
			
			float QDelta = MathLogTool::HexabelToLinear(WDelta.Y / LastSize.Y);
			float Quality = ViewModel->GetQuality(PossessedBandIndex);
			ViewModel->SetQuality(PossessedBandIndex, Quality * QDelta);

			if (!bAlt)
			{
				//
				// Auto makeup
				// 

				bool bIsBell = PossessedBandIndex == 1;
				bool bIsShelf = PossessedBandIndex == 0 || PossessedBandIndex == 2;
				if (bIsBell)
				{
					float ResponseAvgDb = ViewModel->GetResponseAvgDb(PossessedBandIndex);
					ViewModel->SetMakeupDb(PossessedBandIndex,
					                       PresstimeMakeupDb + 1.5f * (PresstimeAvgDb - ResponseAvgDb));
				}
				else if (bIsShelf)
				{
					float ResponseAvgDb = ViewModel->GetResponseAvgDb(PossessedBandIndex);
					ViewModel->SetMakeupDb(PossessedBandIndex, 
										   PresstimeMakeupDb + 1.0f * (PresstimeAvgDb - ResponseAvgDb));
				}
			}
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
			float NNewY = PresstimeNAmount + NDiff.Y;

			ViewModel->SetFrequency(PossessedBandIndex, F);
			ViewModel->SetAmountDb
			(
				PossessedBandIndex, 
				FVM_ClathriQ::DynamicMax - NNewY * (FVM_ClathriQ::DynamicMax - FVM_ClathriQ::DynamicMin)
			);

			if (!bAlt)
			{
				//
				// Auto makeup
				// 

				bool bIsBell = PossessedBandIndex == 1;
				bool bIsShelf = PossessedBandIndex == 0 || PossessedBandIndex == 2;
				if (bIsBell)
				{
					float ResponseAvgDb = ViewModel->GetResponseAvgDb(PossessedBandIndex);
					ViewModel->SetMakeupDb
					(
						PossessedBandIndex, 
						PresstimeMakeupDb + 1.5f * (PresstimeAvgDb - ResponseAvgDb)
					);
				}
				else if (bIsShelf)
				{
					float ResponseAverageDb = ViewModel->GetResponseAvgDb(PossessedBandIndex);
					ViewModel->SetMakeupDb
					(
						PossessedBandIndex,
						PresstimeMakeupDb + 1.0f * (PresstimeAvgDb - ResponseAverageDb)
					);
				}
			}
		}

		ViewModel->GetEvent_BandChanging().Broadcast(PossessedBandIndex);
	}

	bWasLeftMouseButtonPressed = bLeft;
	LastMousePos = MousePos;
	return Reply;
}

int32 UW_ClathriQFrequencyResponse::NativePaint(const FPaintArgs&        Args,
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

	if (PinIndex == INDEX_NONE) return Result;

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

	for (int32 Index = 0; Index < UClathrispaceSettings::BandCount; ++Index)
	{
		FVector2D P1 = BandPoints[Index] + FVector2D(0.f, -1.f);
		FVector2D P2 = BandPoints[Index] + FVector2D(0.f, +1.f);

		if (ViewModel->GetIsEnabled(Index))
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

void UW_ClathriQFrequencyResponse::BakeGrid()
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

void UW_ClathriQFrequencyResponse::BakeResponse()
{
	if (!ViewModel) return;
	if (!ViewModel->PullDataContext()) return;

	const float W = LastSize.X;
	const float H = LastSize.Y;

	for (int32 I = 0; I < FVM_ClathriQ::Resolution; ++I)
	{
		float Frequency = FMath::Exp(I * FVM_ClathriQ::FLS + FVM_ClathriQ::FLMin);
		float Response = ViewModel->DtftDb(Frequency);

		float Y = (FVM_ClathriQ::DynamicMax - Response)
		        / (FVM_ClathriQ::DynamicMax - FVM_ClathriQ::DynamicMin);

		ResponsePoints[I] = 
		{
			W * I * FVM_ClathriQ::ResolutionStep,
			H * Y
		};
	}

	for (int32 Index = 0; Index < UClathrispaceSettings::BandCount; ++Index)
	{
		float F = ViewModel->GetFrequency(Index);
		float X = MathLogTool::TwentiethsToTribel(F);
		float Response = ViewModel->DtftDb(F);
		float Y = (FVM_ClathriQ::DynamicMax - Response)
		        / (FVM_ClathriQ::DynamicMax - FVM_ClathriQ::DynamicMin);

		if (Index < BandPoints.Num())
		{
			BandPoints[Index] = { W * X, H * Y };
		}
	}
}

FVector2D UW_ClathriQFrequencyResponse::GetBandWPos(int32 BandIndex)
{
	if (FEarPin* Pin = ViewModel->PullDataContext())
	{
		float F = BandIndex == 0
		          ? Pin->Band1Frequency
		          : BandIndex == 1
		            ? Pin->Band2Frequency
		            : Pin->Band3Frequency;

		float NX = MathLogTool::TwentiethsToTribel(F);
		float NY = (FVM_ClathriQ::DynamicMax - ViewModel->DtftDb(F)) /
		           (FVM_ClathriQ::DynamicMax - FVM_ClathriQ::DynamicMin);

		float WX = NX * LastSize.X;
		float WY = NY * LastSize.Y;
		return { WX, WY };
	}

	return FVector2D::ZeroVector;
}
