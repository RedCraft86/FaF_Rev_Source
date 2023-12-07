// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "Engine/TargetPoint.h"
#include "GameplayTagContainer.h"
#include "GCTeleportTarget.generated.h"

UCLASS(NotBlueprintable, DisplayName = "Teleport Target")
class GAMECORE_API AGCTeleportTarget final : public ATargetPoint
{
	GENERATED_BODY()

public:

	AGCTeleportTarget();

	static AGCTeleportTarget* Get(const UObject* WorldContext, const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, Category = "TeleportTarget")
		FGameplayTagContainer GetTeleporterTags() const { return TeleporterTag; }

	UFUNCTION(BlueprintCallable, Category = "TeleportTarget")
		void TeleportPlayer() const;
	
	UFUNCTION(BlueprintCallable, Category = "TeleportTarget")
		void TeleportActor(AActor* ActorToTeleport) const;

private:
	
	UPROPERTY(EditAnywhere, Category = "TeleportTarget")
		FGameplayTagContainer TeleporterTag;
};
