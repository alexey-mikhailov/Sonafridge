// Sonafridge 2022

#include "ClathrispaceSettingsProxy.h"
#include "Sonafridge/SonafridgeCommon.h"
#include "Sonafridge/Tools/MathTools.h"

void FClathrispaceSettingsProxy::Init(UClathrispaceSettings* InSettings)
{
	Settings = InSettings;
	TransferAssetToThis();
	Settings->GetEvent_ExternallyChanged().AddSP(this, &FClathrispaceSettingsProxy::OnExternallyChanged);
	Settings->GetEvent_InternallyChanged().AddSP(this, &FClathrispaceSettingsProxy::OnInternallyChanged);
}

void FClathrispaceSettingsProxy::OnExternallyChanged()
{
	TransferAssetToThis();
	ExternallyChanged.Broadcast();
}

void FClathrispaceSettingsProxy::OnInternallyChanged()
{
	TransferAssetToThis();
	InternallyChanged.Broadcast();
}

void FClathrispaceSettingsProxy::TransferAssetToThis()
{
	if (Settings.IsValid())
	{
		FClathriEarData& SettingsEarData = Settings->GetEarData();
		EarData.EarPositionL = -SettingsEarData.EarPosition;
		EarData.EarPositionR = SettingsEarData.EarPosition;

		int32 PinNum = SettingsEarData.EarPins.Num();
		EarData.EarPinsL.SetNum(PinNum);
		EarData.EarPinsR.SetNum(PinNum);

		for (int32 I = 0; I < PinNum; ++I)
		{
			FEarPin& ThisPinL = EarData.EarPinsL[I] = SettingsEarData.EarPins[I];
			FEarPin& ThisPinR = EarData.EarPinsR[I] = SettingsEarData.EarPins[I];
			MathTool::ReflectVectorY(ThisPinL.Direction);
		}
	}
}
