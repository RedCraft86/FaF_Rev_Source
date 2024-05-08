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

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		void ReloadLevel();

protected:

	bool bRanFirstLoads = false;

	virtual void Init() override;
	virtual void OnWorldBeginPlay(UWorld* InWorld) override;
};
