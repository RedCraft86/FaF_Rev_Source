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
		TObjectPtr<class UNarrativeComponent> Narrative;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		TObjectPtr<class UInventoryComponent> Inventory;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "GameMode")
		TObjectPtr<class AInspectionActor> InspectionActor;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "GameMode")
		TObjectPtr<class AFRPlayerController> PlayerController;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "GameMode")
		TObjectPtr<class AFRPlayerBase> PlayerCharacter;
};
