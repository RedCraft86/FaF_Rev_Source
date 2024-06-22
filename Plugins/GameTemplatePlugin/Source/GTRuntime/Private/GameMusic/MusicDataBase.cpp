// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameMusic/MusicDataBase.h"
#include "GameMusic/MusicManagerBase.h"

#if WITH_EDITOR
void UMusicDataBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	while (BaseChannels.Num() < AMusicManagerBase::MaxChannels)
	{
		BaseChannels.Add({});
	}

	while (BaseChannels.Num() > AMusicManagerBase::MaxChannels)
	{
		if (BaseChannels.IsEmpty()) break;
		BaseChannels.RemoveAt(BaseChannels.Num() - 1);
	}
}
#endif
