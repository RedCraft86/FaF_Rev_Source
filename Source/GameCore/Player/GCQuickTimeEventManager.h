// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "GCQuickTimeEventManager.generated.h"

class UGCQuickTimeEventWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQuickTimeEventSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuickTimeKeyEventSignature, const FString&, ID);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMECORE_API UGCQuickTimeEventManager final : public UActorComponent
{
	GENERATED_BODY()

	friend class AGCPlayerCharacter;

public:

	UGCQuickTimeEventManager();
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		TSubclassOf<UGCQuickTimeEventWidget> WidgetClass;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TArray<FKey> Keylist;

	// BUMP - PENALTY - CONSTANT
	UPROPERTY(EditAnywhere, Category = "Settings")
		FVector Speeds;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 1, UIMin = 1))
		uint8 MovesPerPhase;

	UPROPERTY(BlueprintAssignable)
		FQuickTimeEventSignature OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FQuickTimeEventSignature OnFailed;

	UPROPERTY(BlueprintAssignable)
		FQuickTimeEventSignature OnStarted;

	UPROPERTY(BlueprintAssignable)
		FQuickTimeKeyEventSignature OnKeySuccess;

	UPROPERTY(BlueprintAssignable)
		FQuickTimeKeyEventSignature OnKeyFailed;

	UPROPERTY(BlueprintAssignable)
		FQuickTimeKeyEventSignature OnKeyAdded;
	
	UPROPERTY(BlueprintAssignable)
		FQuickTimeKeyEventSignature OnKeyRemoved;

	UFUNCTION(BlueprintPure, Category = "QuickTimeEvent")
		bool IsPlaying() const { return bPlaying; }

	UFUNCTION(BlueprintPure, Category = "QuickTimeEvent")
		float GetPercent() const { return Progress / MaxProgress; }

	UFUNCTION(BlueprintPure, Category = "QuickTimeEvent")
		FKey GetKeyFromID(const FString& InID) const { return Instances.FindRef(InID); }

	UFUNCTION(BlueprintPure, Category = "QuickTimeEvent")
		bool WasKeySuccess(const FString& InID) const { return SucceededKeys.Contains(InID); }

	UFUNCTION(BlueprintPure, Category = "QuickTimeEvent")
		bool WasKeyFailed(const FString& InID) const { return FailedKeys.Contains(InID); }

	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
		void BeginNewQTE(const float InMaxProgress = 100.0f);

	UFUNCTION(BlueprintCallable, Category = "QuickTimeEvent")
		void RegisterKeyPress(const FKey& InKey);

private:

	UPROPERTY(Transient)
		UGCQuickTimeEventWidget* WidgetObject;

	bool bPlaying;
	bool bOverridePlayer;
	uint8 Phase, NumMoves;
	float Progress, MaxProgress;
	TSet<FString> SucceededKeys, FailedKeys;
	TMap<FString, FKey> Instances;
	TArray<FString> InstanceKeys; // To preserve order
	
	FTimerHandle StartTimer, TickTimer;

	void CreateInstance();
	void RemoveInstance(const FString& InID);
	void MarkKeySuccess(const FString& InID);
	void MarkKeyFailed(const FString& InID);
	void StartQTE(const float InProgress);
	void StopQTE(const bool bFailed);
	void ResetData();
	
	void TimedTick();
	virtual void BeginPlay() override;
};
