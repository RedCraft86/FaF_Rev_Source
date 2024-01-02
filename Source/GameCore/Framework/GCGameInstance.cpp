// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCGameInstance.h"
#include "Player/GCPlayerCharacter.h"
#include "UserSettings/GCUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "JsonObjectWrapper.h"
#include "RCCVarLibrary.h"

UGCGameInstance::UGCGameInstance()
{
	PlayerCharacter = nullptr;
	bInvinciblePlayer = false;
	bViewModeUnlit = false;
}

UGCGameInstance* UGCGameInstance::Get(const UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	return World ? World->GetGameInstance<UGCGameInstance>() : nullptr;
}

void UGCGameInstance::UpdatePlayerInvincible() const
{
	OnInvincibleStateChanged.Broadcast(IsPlayerInvincible());
}

void UGCGameInstance::SetPlayerInvincible(const bool bInvincible)
{
	bInvinciblePlayer = bInvincible;
	UpdatePlayerInvincible();
}

bool UGCGameInstance::IsPlayerInvincible() const
{
	return bInvinciblePlayer || (PlayerCharacter ? PlayerCharacter->IsInInvincibleState() : true);
}

void UGCGameInstance::SetViewModeUnlit(const bool bUnlit)
{
	if (IConsoleVariable* VMCVar = URCCVarLibrary::FindCVar(TEXT("r.ForceDebugViewModes")))
	{
		VMCVar->Set(bUnlit ? 1 : 0);
		UKismetSystemLibrary::ExecuteConsoleCommand(this,
			FString::Printf(TEXT("Viewmode %s"), bUnlit ? TEXT("unlit") : TEXT("lit")));

		bViewModeUnlit = bUnlit;
	}
}

void UGCGameInstance::Init()
{
	Super::Init();
	UGCUserSettings::Get()->GameInstance = this;
	
	FString JsonStr;
	FFileHelper::LoadFileToString(JsonStr, *(FPaths::ProjectSavedDir() / TEXT("Global.json")));
	FJsonObjectWrapper GlobalJson;
	GlobalJson.JsonObjectFromString(JsonStr);
	if (!GlobalJson.JsonObject->GetBoolField(TEXT("Finished_First_Launch")))
	{
#if !WITH_EDITOR
		UGCUserSettings* Settings = UGCUserSettings::Get();
		Settings->SetToDefaults();
		Settings->SetOverallQuality(3);
		Settings->SetResScalePercent(50.0f);
		Settings->ApplyNonResolutionSettings();
#endif
		GlobalJson.JsonObject->SetBoolField(TEXT("Finished_First_Launch"), true);

		const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonStr, 0);
		FJsonSerializer::Serialize(GlobalJson.JsonObject.ToSharedRef(), JsonWriter, true);
		
		FFileHelper::SaveStringToFile(JsonStr, *(FPaths::ProjectSavedDir() / TEXT("Global.json")));
	}
}

void UGCGameInstance::WorldBeginPlay()
{
	EventWorldBeginPlay();
	UGCUserSettings::Get()->GameInstance = this;
}

void UGCGameInstance::WorldTick(const float DeltaTime)
{
	EventWorldTick(DeltaTime);
}