// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "GCTimingGameManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimingGameManagerEvent, const FString&, ID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimingGameManagerSuccessEvent);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMECORE_API UGCTimingGameManager final : public UActorComponent
{
	GENERATED_BODY()

public:

	UGCTimingGameManager();

	UPROPERTY(EditAnywhere, Category = "Settings")
		TArray<FKey> KeyList;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FVector2D BumpSpeed;

	UPROPERTY(EditAnywhere, Category = "Settings")
		float DrainSpeed;

	UPROPERTY(EditAnywhere, Category = "Settings")
		uint8 MovePerPhase;
	
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		TSet<FString> SucceededKeys;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		TSet<FString> FailedKeys;

	UPROPERTY(BlueprintAssignable)
		FTimingGameManagerEvent OnAdded;

	UPROPERTY(BlueprintAssignable)
		FTimingGameManagerEvent OnRemoved;

	UPROPERTY(BlueprintAssignable)
		FTimingGameManagerSuccessEvent OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FTimingGameManagerSuccessEvent OnFailed;

	UPROPERTY(BlueprintAssignable)
		FTimingGameManagerSuccessEvent OnStarted;

	UFUNCTION(BlueprintPure, Category = "TimingGame")
		float GetProgress() const { return 0.01f + Progress / MaxProgress; }
	
	UFUNCTION(BlueprintPure, Category = "TimingGame")
		FKey GetKeyFromID(const FString& InID) const;

	UFUNCTION(BlueprintCallable, Category = "TimingGame")
		void RegisterKeyPress(const FKey& InKey);

	UFUNCTION(BlueprintCallable, Category = "TimingGame")
		void BeginNewGame(const float InMaxProgress = 100.0f);

	UFUNCTION(BlueprintPure, Category = "TimingGame")
		bool IsInGame() const { return bInGame; }
	
private:

	bool bInGame;
	uint8 Phase;
	int32 NumMoves;
	float Progress;
	float MaxProgress;
	TMap<FString, FKey> Instances;
	FTimerHandle TickTimer;

	void OnKeySuccess(const FString& ID);
	void OnKeyFailed(const FString& ID);
	void CreateInstance();
	void RemoveInstance(const FString& ID);
	void ConsistentTick();
	void StopGame(const bool bFailed);
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
