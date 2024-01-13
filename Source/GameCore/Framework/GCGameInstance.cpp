// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCGameInstance.h"
#include "Player/GCPlayerCharacter.h"
#include "UserSettings/GCUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RCRuntimeLibrary.h"
#include "RCCVarLibrary.h"

UGCGameInstance::UGCGameInstance()
{
	PlayerCharacter = nullptr;
	bRanFirstSettingCheck = false;
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
	FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &UGCGameInstance::OnWindowFocusChanged);
}

void UGCGameInstance::WorldBeginPlay()
{
	EventWorldBeginPlay();
	UGCUserSettings::Get()->GameInstance = this;
	
	if (!bRanFirstSettingCheck)
	{
		bRanFirstSettingCheck = true;
		UGCUserSettings::Get()->InitializeSettings();

		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this,
			&UGCGameInstance::ReloadLevel, 0.5f, false);
	}
}

void UGCGameInstance::WorldTick(const float DeltaTime)
{
	EventWorldTick(DeltaTime);
}

void UGCGameInstance::ReloadLevel() const
{
	URCRuntimeLibrary::RestartLevel(GetWorld());
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UGCGameInstance::OnWindowFocusChanged(bool bFocused) const
{
	if (IConsoleVariable* CVar = URCCVarLibrary::FindCVar(TEXT("t.MaxFPS")))
	{
		if (bFocused)
		{
			CVar->Set(UGCUserSettings::Get()->GetFrameRateLimit());
		}
		else
		{
			CVar->Set(10.0f);
		}
	}
}
