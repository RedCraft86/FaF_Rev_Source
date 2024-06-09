// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GamejoltAsyncAction.h"
#include "AsyncAuthLogin.generated.h"

UCLASS()
class GAMEJOLTAPI_API UAsyncAuthLogin final : public UGamejoltAsyncAction
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Auth Login (Gamejolt)", Keywords = "auth log in login gamejolt"))
		static UAsyncAuthLogin* AuthLogin(const FString& UserName, const FString& UserToken, const bool bRemember = true);

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FGamejoltAsyncActionDelegate OnFail;
	
private:

	virtual void Activate() override;
	
	FString _UserName = TEXT("");
	FString _UserToken = TEXT("");
	bool _Remember = true;

public:

	UFUNCTION(BlueprintCallable, Category = "GamejoltAPI", meta = (DisplayName = "Logout (Gamejolt)", Keywords = "log out logout gamejolt"))
		static void Logout(const bool bForget = true);
};
