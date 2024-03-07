// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "GCTimingGameManager.generated.h"

struct FTimingGameStruct
{
	FGuid ID;
	FKey Key;
	
	FTimingGameStruct() : ID({}), Key(FKey{}) {}
	FTimingGameStruct(const FGuid InID, const FKey& InKey) : ID(InID), Key(InKey) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimingGameManagerEvent, const FGuid&, ID);
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
		TSet<FGuid> SucceededKeys;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
		TSet<FGuid> FailedKeys;

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
		FKey GetKeyFromID(const FGuid& InID) const;

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
	TMap<FGuid, TSharedPtr<FTimingGameStruct>> Instances;
	FTimerHandle TickTimer;

	void OnKeySuccess(const FGuid& ID);
	void OnKeyFailed(const FGuid& ID);
	void CreateInstance();
	void RemoveInstance(const FGuid& ID);
	void ConsistentTick();
	void StopGame(const bool bFailed);
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
