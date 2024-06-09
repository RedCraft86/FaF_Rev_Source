// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "JsonObjectWrapper.h"
#include "GamejoltData.generated.h"

UENUM(BlueprintInternalUseOnly)
enum class EGamejoltTrophyFilter : uint8
{
	AllTrophies,
	AchievedTrophies,
	UnachievedTrophies
};

USTRUCT(BlueprintInternalUseOnly)
struct GAMEJOLTAPI_API FGamejoltResponse
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GamejoltResponse")
		bool bSuccess;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GamejoltResponse")
		FString ErrorMsg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GamejoltResponse")
		FJsonObjectWrapper Json;

	FGamejoltResponse(): bSuccess(false), ErrorMsg(TEXT("")) {}
	FGamejoltResponse(const bool bInSuccess, const FString& InErrorMsg) : bSuccess(bInSuccess), ErrorMsg(InErrorMsg) {}
	FGamejoltResponse(const FJsonObjectWrapper& InJson) : Json(InJson)
	{
		InJson.JsonObject->TryGetBoolField(TEXT("success"), bSuccess);
		InJson.JsonObject->TryGetStringField(TEXT("message"), ErrorMsg);
	}
};
namespace GamejoltResponse
{
	inline FGamejoltResponse Unknown{false, TEXT("Unknown error.")};
	inline FGamejoltResponse NotLoggedIn{false, TEXT("The user is not logged in.")};
	inline FGamejoltResponse InvalidURL{false, TEXT("The url requested is invalid or connection has timed out.")};
	inline FGamejoltResponse InvalidParams{false, TEXT("Some or all of the necessary parameters were not provided.")};
}

USTRUCT(BlueprintType)
struct GAMEJOLTAPI_API FGamejoltUserData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		int32 ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		FString UserName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		FString AvatarURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		FString SignedUpRelativeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		FDateTime SignedUpExactTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		FString LoggedInRelativeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		FDateTime LoggedInExactTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		FString Status;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		FString DeveloperName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		FString DevWebsiteURL;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserData")
		FString DevDescription;

	FGamejoltUserData() : ID(0) {}
	void SetTimestamps(const int32 SignedUpSeconds, const int32 LoggedInSeconds)
	{
		SignedUpExactTime = FDateTime::FromUnixTimestamp(SignedUpSeconds);
		LoggedInExactTime = FDateTime::FromUnixTimestamp(LoggedInSeconds);
	}
};

USTRUCT(BlueprintType)
struct GAMEJOLTAPI_API FGamejoltTrophyData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrophyData")
		int32 ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrophyData")
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrophyData")
		FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrophyData")
		FString Difficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrophyData")
		FString ImageURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrophyData")
		bool bAchieved;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrophyData")
		FString Timestamp;

	FGamejoltTrophyData()
		: ID(0), Name(TEXT("")), Description(TEXT("")), Difficulty(TEXT("Bronze"))
		, ImageURL(TEXT("")), bAchieved(false), Timestamp(TEXT(""))
	{}
};