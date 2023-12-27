// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "GCGameMusicManager.generated.h"

UCLASS(Blueprintable, DisplayName = "Game Music Manager")
class GAMECORE_API UGCGameMusicManager final : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UGCGameMusicManager();

	static UGCGameMusicManager* Get(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "GameMusic|Manager")
		void SetMainMusic(USoundBase* InSound);

	UFUNCTION(BlueprintCallable, Category = "GameMusic|Manager")
		void SetMainMusicPaused(const bool bPaused) const;
	
	UFUNCTION(BlueprintCallable, Category = "GameMusic|Manager")
		void SetChaseMusic(USoundBase* InSound);

	UFUNCTION(BlueprintCallable, Category = "GameMusic|Manager")
		void SetChaseMusicPaused(const bool bPaused) const;
	
	UFUNCTION(BlueprintCallable, Category = "GameMusic|Manager")
		void SetChaseMode(const bool bIsInChase);

	UFUNCTION(BlueprintCallable, Category = "GameMusic|Manager")
		FORCEINLINE bool IsChaseMode() const { return bUseChaseMusic; }

	UFUNCTION(BlueprintCallable, Category = "GameMusic|Manager")
		void StopAll();

	UFUNCTION(BlueprintPure, Category = "GameMusic|Manager")
		FORCEINLINE UAudioComponent* GetMainMusic() const { return MainMusicComponent; }

	UFUNCTION(BlueprintPure, Category = "GameMusic|Manager")
		FORCEINLINE UAudioComponent* GetChaseMusic() const { return ChaseMusicComponent; }
		
private:
	
	virtual void OnWorldBeginPlay(UWorld& World) override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		UAudioComponent* MainMusicComponent;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		UAudioComponent* ChaseMusicComponent;

	UPROPERTY(Transient)
		USoundBase* MainMusicSound;

	UPROPERTY(Transient)
		USoundBase* ChaseMusicSound;

	UPROPERTY(Transient)
		class UGCSettings* GameSettings;
	
	bool bUseChaseMusic;

	UAudioComponent* CreateSound(USoundBase* InSound) const;
	void PauseMusicInternal(UAudioComponent* Component) const;
	void UnpauseMusicInternal(UAudioComponent* Component) const;
};
