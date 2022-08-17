// Sonafridge 2022

#pragma once
#include "ClathrispaceSettings.h"

class FClathrispace;
class FClathriEarPreviewScene;

struct FClathriEarDataProxy
{
	FVector EarPositionL = { 0.f, -8.f, 0.f };
	FVector EarPositionR = { 0.f, +8.f, 0.f };

	TArray<FEarPin> EarPinsL;
	TArray<FEarPin> EarPinsR;
};

class SONAFRIDGE_API FClathrispaceSettingsProxy final : public TSharedFromThis<FClathrispaceSettingsProxy>
{
public:
	void Init(UClathrispaceSettings* InSettings);

	FClathriEarDataProxy& GetEarData() { return EarData; }
	FClathriMazeData&     GetMazeData() { return MazeData; }
	FClathriumData&       GetRoomData() { return RoomData; }

	DECLARE_EVENT(UClathrispaceSettings, FExternallyChanged);
	FExternallyChanged& GetEvent_ExternallyChanged() { return ExternallyChanged; }

	DECLARE_EVENT(UClathrispaceSettings, FInternallyChanged);
	FInternallyChanged& GetEvent_InternallyChanged() { return InternallyChanged; }

private:
	void OnExternallyChanged();
	void OnInternallyChanged();
	void TransferAssetToThis();

	// UClathrispaceSettings instance held by FAssetEditor_Clathrispace class. 
	TWeakObjectPtr<UClathrispaceSettings> Settings;

	friend FClathrispace;
	friend FClathriEarPreviewScene;
	FClathriEarDataProxy EarData;

	friend FClathrispace;
	friend FClathriEarPreviewScene;
	FClathriMazeData MazeData;

	friend FClathrispace;
	friend FClathriEarPreviewScene;
	FClathriumData RoomData;

	FExternallyChanged ExternallyChanged;
	FInternallyChanged InternallyChanged;
};
