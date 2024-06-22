// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MusicDataBase.h"
#include "GameFramework/Actor.h"
#include "MusicManagerBase.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct GTRUNTIME_API FMusicChannelComponents
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "MusicChannelComponents")
		TMap<TObjectPtr<const USoundBase>, TObjectPtr<UAudioComponent>> Components;

	FMusicChannelComponents() : Components({}) {}

	void ClearComponents();
	TArray<TObjectPtr<UAudioComponent>> GetComponents() const;
	UAudioComponent* FindComponent(const USoundBase* InSound);
	void AddComponent(UAudioComponent* InComponent);
};

UCLASS()
class GTRUNTIME_API AMusicManagerBase : public AActor
{
	GENERATED_BODY()

public:

	AMusicManagerBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TMap<EWorldMusicChannel, TObjectPtr<UAudioComponent>> BaseAudioComponent;

	UFUNCTION(BlueprintCallable, Category = "WorldMusicManager")
		void SetBaseMusicData(const UMusicDataBase* InData);

	UFUNCTION(BlueprintCallable, Category = "WorldMusicManager")
		UAudioComponent* FindExternalAudio(const EWorldMusicChannel InChannel, const USoundBase* InSound) const;

	UFUNCTION(BlueprintCallable, Category = "WorldMusicManager")
		void AddExternalAudio(const EWorldMusicChannel InChannel, USoundBase* InSound, const float InVolume, const float InPitch, const float InStartTime, const bool bFade);

	UFUNCTION(BlueprintCallable, Category = "WorldMusicManager")
		void MuteChannel(const EWorldMusicChannel InChannel, const bool bImmediately = false);

	UFUNCTION(BlueprintCallable, Category = "WorldMusicManager")
		void UnmuteChannel(const EWorldMusicChannel InChannel, const bool bImmediately = false);

	UFUNCTION(BlueprintCallable, Category = "WorldMusicManager")
		void MuteAllChannels(const bool bImmediately = true);

protected:
	
	UPROPERTY(Transient) TObjectPtr<const UMusicDataBase> BaseMusicData;
	UPROPERTY(Transient) TMap<EWorldMusicChannel, FMusicChannelComponents> ExternalAudioComponents;
	TMap<EWorldMusicChannel, TPair<FTimerHandle, FTimerHandle>> ChannelTimers;

	virtual void BeginPlay() override;

public: // Statics
	
	UFUNCTION(BlueprintPure, Category = "Game", DisplayName = "Get Music Manager", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "Class"))
		static AMusicManagerBase* GetMusicManager(const UObject* WorldContextObject);

	template <typename T = AMusicManagerBase>
	static T* Get(const UObject* WorldContextObject)
	{
		return Cast<T>(GetMusicManager(WorldContextObject));
	}
};
