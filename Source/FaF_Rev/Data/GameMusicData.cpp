// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameMusicData.h"
#include "FRSettings.h"

FGameMusicData FGameMusicID::GetMusicData() const
{
	FGameMusicData Data;
	const UDataTable* Table = FRSettings->MusicTable.LoadSynchronous();
	if (const FGameMusicData* DataPtr = Table ? Table->FindRow<FGameMusicData>(SelectedValue, ToString()) : nullptr)
	{
		Data = *DataPtr;
	}
		
	return Data;
}
