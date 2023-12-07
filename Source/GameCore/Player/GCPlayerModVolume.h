// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "GameFramework/Volume.h"
#include "GCPlayerModVolume.generated.h"

UCLASS(Abstract, HideCategories = "HLOD, Path Tracing, Navigation, Cooking, Replication, Networking, Mobile, Ray Tracing")
class GAMECORE_API AGCPlayerModVolume : public AVolume
{
	GENERATED_BODY()

public:

	AGCPlayerModVolume();
};

UCLASS(DisplayName = "Player Volume (Speed)")
class GAMECORE_API AGCPlayerVolume_Speed final : public AGCPlayerModVolume
{
	GENERATED_BODY()

public:

	AGCPlayerVolume_Speed();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerSettings")
		float SpeedMultiplier;

protected:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};
