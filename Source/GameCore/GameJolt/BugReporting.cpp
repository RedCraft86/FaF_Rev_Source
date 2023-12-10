// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "BugReporting.h"
#include "GamejoltAPI.h"
#include "GameSequence/GCSequenceManager.h"
#include "Narrative/GCNarrativeComponent.h"
#include "Kismet/GameplayStatics.h"

UGJBanUserID* UGJBanUserID::BanUserID(const FString UserToBan)
{
	UGJBanUserID* Task = NewObject<UGJBanUserID>();
	Task->UserID = UserToBan;
	return Task;
}

void UGJBanUserID::Activate()
{
	if (bRunning) return;
	bRunning = true;
	
	GamejoltAPI::BanUser(UserID, [this](const bool bSuccess, const FString& Response)
	{
		if (bSuccess) OnSuccess.Broadcast(TEXT("")); else OnFail.Broadcast(Response);
		bRunning = false;
	});
}

UGJUnbanUserID* UGJUnbanUserID::UnbanUserID(const FString UserToUnban)
{
	UGJUnbanUserID* Task = NewObject<UGJUnbanUserID>();
	Task->UserID = UserToUnban;
	return Task;
}

void UGJUnbanUserID::Activate()
{
	if (bRunning) return;
	bRunning = true;
	
	GamejoltAPI::UnbanUser(UserID, [this](const bool bSuccess, const FString& Response)
	{
		if (bSuccess) OnSuccess.Broadcast(TEXT("")); else OnFail.Broadcast(Response);
		bRunning = false;
	});
}

UGJGetBannedUserIDs* UGJGetBannedUserIDs::GetBannedUserIDs()
{
	UGJGetBannedUserIDs* Task = NewObject<UGJGetBannedUserIDs>();
	return Task;
}

void UGJGetBannedUserIDs::Activate()
{
	if (bRunning) return;
	bRunning = true;
	
	GamejoltAPI::GetData(BAN_LIST_KEY, [this](const bool bSuccess, const FString& Response, const FString& Data)
	{
		TArray<FString> UserIDs; Data.ParseIntoArray(UserIDs, TEXT(","), true);
		OnFinish.Broadcast(UserIDs);
		bRunning = false;
	});
}

UReportBug* UReportBug::ReportBug(const FJsonObjectWrapper& ReportData)
{
	UReportBug* Task = NewObject<UReportBug>();
	Task->Data = ReportData;
	return Task;
}

FJsonObjectWrapper UReportBug::CreateBugReportData(const UObject* WorldContextObject, const FString& Title, const FString& Description, const bool bLevel, const bool bQuest, const bool bPosition)
{
	FJsonObjectWrapper JsonData;
	if (!Title.IsEmpty()) JsonData.JsonObject->SetStringField("title", Title);
	if (!Description.IsEmpty()) JsonData.JsonObject->SetStringField("description", Description);
	if (bLevel)
	{
		const UGCSequenceManager* SM = UGCSequenceManager::Get(WorldContextObject);
		JsonData.JsonObject->SetStringField("sequence", SM ? *SM->GetCurrentSequence().ToString() : TEXT(""));
	}
	if (bQuest)
	{
		const UGCNarrativeComponent* Narrative = UGCNarrativeComponent::Get(WorldContextObject);
		JsonData.JsonObject->SetStringField("quest", Narrative->GetLastQuestData());
	}
	if (bPosition)
	{
		const APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0);
		JsonData.JsonObject->SetStringField("position", PCM ? PCM->GetCameraLocation().ToString() : TEXT(""));
	}

	return JsonData;
}

void UReportBug::Activate()
{
	if (bRunning) return;
	bRunning = true;
	
	GamejoltAPI::CreateBugReport(Data, [this](const bool bSuccess, const bool bBanned, const FString& Response)
	{
		if (bSuccess) OnSuccess.Broadcast(TEXT(""), false); else OnFail.Broadcast(Response, bBanned);
		bRunning = false;
	});
}

UGetBugReport* UGetBugReport::GetBugReport(const FString InKey)
{
	UGetBugReport* Task = NewObject<UGetBugReport>();
	Task->Key = InKey;
	return Task;
}

void UGetBugReport::Activate()
{
	if (bRunning) return;
	bRunning = true;
	
	GamejoltAPI::GetBugReport(Key, [this](const bool bSuccess, const FJsonObjectWrapper& Data)
	{
		OnFinish.Broadcast(Data);
		bRunning = false;
	});
}
