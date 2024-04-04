// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "FRPlayer.generated.h"

UCLASS()
class FAF_REV_API AFRPlayer : public ACharacter
{
	GENERATED_BODY()

public:

	AFRPlayer();
	
protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public: // Statics

	UFUNCTION(BlueprintPure, Category = "Game", DisplayName = "Get Player (Smart)", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", WorldContext = "WorldContextObject"))
		static APawn* GetPlayerPawnSmart(const UObject* WorldContextObject, const TSubclassOf<APawn> Class);
};
