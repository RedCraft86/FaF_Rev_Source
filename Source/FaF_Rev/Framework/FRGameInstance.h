// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTGameInstance.h"
#include "FRGameInstance.generated.h"

#define FRGameInstance(Context) UGTGameInstance::Get<UFRGameInstance>(Context)

UCLASS()
class FAF_REV_API UFRGameInstance : public UGTGameInstance
{
	GENERATED_BODY()

public:

	UFRGameInstance();

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		void ReloadLevel() const;

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		void SetPlayerInvincible(bool bInInvincible);

	UFUNCTION(BlueprintPure, Category = "GameInstance")
		bool IsPlayerInvincible() const { return bPlayerInvincible; }

	DECLARE_MULTICAST_DELEGATE_OneParam(FUpdatePlayerInvincible, const bool);
	FUpdatePlayerInvincible OnInvincibilityUpdated;

protected:
	
	bool bRanFirstLoads;
	bool bPlayerInvincible;

	virtual void Init() override;
	virtual void OnWorldBeginPlay(UWorld* InWorld) override;
};
