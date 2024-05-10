// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTGameMode.h"
#include "FRGameMode.generated.h"

#define FRGameMode(Context) AGTGameModeBase::Get<AFRGameModeBase>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRGameModeBase final : public AGTGameModeBase
{
	GENERATED_BODY()

public:

	AFRGameModeBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UNarrativeComponent* Narrative;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UInventoryComponent* Inventory;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "GameMode")
		class AInspectionActor* InspectionActor;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "GameMode")
		class AFRPlayerController* PlayerController;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "GameMode")
		class AFRPlayerBase* PlayerCharacter;
};
