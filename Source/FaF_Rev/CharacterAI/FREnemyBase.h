// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FRCharacter.h"
#include "FREnemyBase.generated.h"

UENUM(BlueprintType, DisplayName = "Enemy AI Mode")
enum class EEnemyState : uint8
{
	None,
	Roam,
	Alert,
	Chase,
	Search
};
ENUM_RANGE_BY_FIRST_AND_LAST(EEnemyState, EEnemyState::None, EEnemyState::Search);

UCLASS(Abstract, DisplayName = "Enemy Character Base")
class FAF_REV_API AFREnemyBase : public AFRCharacter
{
	GENERATED_BODY()

public:

	AFREnemyBase();

	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bStartRoaming;

	UFUNCTION(BlueprintCallable, Category = "EnemyAI")
		void SetEnemyState(const EEnemyState InNewState);

	UFUNCTION(BlueprintPure, Category = "EnemyAI")
		EEnemyState GetEnemyState() const { return EnemyState; }

protected:
	
	UPROPERTY() EEnemyState EnemyState;

	virtual void BeginPlay() override;

public: // Statics

	UFUNCTION(BlueprintPure, Category = "EnemyAI")
		static EEnemyState GetHighestEnemyState(const TArray<AFREnemyBase*>& InEnemies);
};
