// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GameVersion.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameJoltMisc.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGamejoltDataKeys, const TArray<FString>&, Keys);

UCLASS()
class GAMECORE_API UGJGetDataKeys final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI", meta = (BlueprintInternalUseOnly = "true"))
		static UGJGetDataKeys* GetDataKeys();

	UPROPERTY(BlueprintAssignable)
		FGamejoltDataKeys OnFinish;
	
	virtual void Activate() override;
	
private:
	
	bool bRunning = false;
};

UCLASS()
class GAMECORE_API UGJRemoveData final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI", meta = (BlueprintInternalUseOnly = "true"))
	static UGJRemoveData* RemoveData(const FString Key);

	UPROPERTY(BlueprintAssignable)
		FGamejoltFinish OnFinish;
	
	virtual void Activate() override;
	
private:
	
	bool bRunning = false;
	FString DataKey = TEXT("");
};