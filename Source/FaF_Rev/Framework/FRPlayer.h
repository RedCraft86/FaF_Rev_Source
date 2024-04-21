// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "FRPlayer.generated.h"

#define GET_PLAYER(Context) AFRPlayer::Get<AFRPlayer>(Context)

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlayerFlags : uint8
{
	Locked,
};
ENUM_CLASS_FLAGS(EPlayerFlags);

USTRUCT(BlueprintType)
struct FPlayerSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerFlags"))
		uint8 PlayerFlags;

	FPlayerSettings() : PlayerFlags(0) {}
};

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
	
	UFUNCTION(BlueprintPure, Category = "Game", DisplayName = "Get Player (Smart)", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", WorldContext = "WorldContextObject"))
		static AFRPlayer* GetPlayerPawnSmart(const UObject* WorldContextObject, const TSubclassOf<AFRPlayer> Class);
	template <typename T = AFRPlayer>
	static T* Get(const UObject* WorldContextObject)
	{
		return Cast<T>(GetPlayerPawnSmart(WorldContextObject, T::StaticClass()));
	}
};
