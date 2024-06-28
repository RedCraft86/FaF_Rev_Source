// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GamejoltAsyncAction.h"
#include "AsyncGetUserData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGamejoltUserDataDelegate, const FGamejoltUserData&, UserData, const FString&, Error);

UCLASS()
class GAMEJOLTAPI_API UAsyncGetUserData final : public UExtendedAsyncAction
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get User Data (Gamejolt)", Keywords = "get user data info gamejolt"))
		static UAsyncGetUserData* GetUserData();

	UPROPERTY(BlueprintAssignable)
		FGamejoltUserDataDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltUserDataDelegate OnFail;
	
private:

	virtual void Activate() override;
};
