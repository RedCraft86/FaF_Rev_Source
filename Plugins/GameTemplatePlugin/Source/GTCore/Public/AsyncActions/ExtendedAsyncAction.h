// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Helpers/LoggingHelpers.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ExtendedAsyncAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncActionDelegate);

#define CREATE_TASK ThisClass* Task = NewObject<ThisClass>();
#define GET_WORLD const UWorld* World = GetWorld(); if (!IsValid(World)) { bTaskRunning = false; return; }
#define CHECK_RUNNING if (CheckRunning()) return;
#define FINISH_RUNNING bTaskRunning = false;

UCLASS(Abstract)
class GTCORE_API UExtendedAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UExtendedAsyncAction();
	virtual UWorld* GetWorld() const override;
	void SetWorldContext(UObject* InContext) { WorldContext = InContext; }
	UObject* GetWorldContext() const { return WorldContext; }

protected:

	bool CheckRunning();
	bool bTaskRunning = false;

private:

	UPROPERTY(Transient) TObjectPtr<UObject> WorldContext;
};
