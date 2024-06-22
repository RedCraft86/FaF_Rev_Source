// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MusicManagerBase.generated.h"

UCLASS(Abstract)
class GTRUNTIME_API AMusicManagerBase : public AActor
{
	GENERATED_BODY()

public:

	AMusicManagerBase();

	static inline uint8 MaxChannels = 4;

protected:
	
	virtual void BeginPlay() override;
};
