// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "FRPlayer.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlayerFlags : uint8
{
	Locked,
};
ENUM_CLASS_FLAGS(EPlayerFlags);

UCLASS()
class FAF_REV_API AFRPlayer : public ACharacter
{
	GENERATED_BODY()

public:

	AFRPlayer();
	
protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public: // Statics

	template <typename T = AFRPlayer>
	static T* Get(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure, Category = "Game", DisplayName = "Get Player (Smart)", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", WorldContext = "WorldContextObject"))
		static ACharacter* GetPlayerPawnSmart(const UObject* WorldContextObject, const TSubclassOf<ACharacter> Class);
};
