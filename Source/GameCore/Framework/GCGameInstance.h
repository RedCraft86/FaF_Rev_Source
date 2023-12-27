// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GCGameInstance.generated.h"

UCLASS(DisplayName = "Main Game Instance")
class GAMECORE_API UGCGameInstance : public UGameInstance
{
	GENERATED_BODY()

	friend class AGCPlayerController;
	friend class AGCPlayerCharacter;

public:

	UGCGameInstance();

	static UGCGameInstance* Get(const UObject* WorldContext);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "World Begin Play")
		void EventWorldBeginPlay();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "World Tick")
		void EventWorldTick(float DeltaTime);
	
	UFUNCTION(BlueprintPure, Category = "GameInstance")
		AGCPlayerCharacter* GetPlayerCharacter() const { return PlayerCharacter; }
	
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		void UpdatePlayerInvincible() const;
	
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		void SetPlayerInvincible(const bool bInvincible);
	
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		bool IsPlayerInvincible() const;

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		void SetViewModeUnlit(const bool bUnlit);
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerInvincibleStateChanged, bool);
	FPlayerInvincibleStateChanged OnInvincibleStateChanged;
	
	virtual void Init() override;
	bool GetPlayerInvincible() const { return bInvinciblePlayer; }
	bool GetViewModeUnlit() const { return bViewModeUnlit; }
	
protected:
	
	UPROPERTY(Transient)
		AGCPlayerCharacter* PlayerCharacter;
	
	bool bInvinciblePlayer;
	bool bViewModeUnlit;

	virtual void WorldBeginPlay();
	virtual void WorldTick(const float DeltaTime);
};
