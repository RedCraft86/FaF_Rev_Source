// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GCSequenceTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "GCSequenceManager.generated.h"

UCLASS(BlueprintType, DisplayName = "Game Sequence Manager")
class GAMECORE_API UGCSequenceManager final : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UGCSequenceManager();
	
	static UGCSequenceManager* Get(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "GameSequence|Manager", meta = (Keywords = "Load Sequence"))
		void LoadGameSequence();
	
	UFUNCTION(BlueprintCallable, Category = "GameSequence|Manager", meta = (Keywords = "Set Sequence"))
		void SetGameSequence(const FGCSequenceID InSequenceID);

	UFUNCTION(BlueprintCallable, Category = "GameSequence|Manager", meta = (Keywords = "Set Subsequence"))
		void SetSubsequenceIndex(const uint8 Index);

	UFUNCTION(BlueprintPure, Category = "GameSequence|Manager")
		FORCEINLINE uint8 GetSubsequenceIndex() const { return SubsequenceIndex; }

	UFUNCTION(BlueprintPure, Category = "GameSequence|Manager")
		FORCEINLINE FGCSequenceID GetCurrentSequence() const { return ThisSequenceID; }

	UFUNCTION(BlueprintPure, Category = "GameSequence|Manager")
		FORCEINLINE FGCSequenceID GetLastSequence() const { return LastSequenceID; }
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSequenceChange, const FName& ID)
	FOnSequenceChange OnSequenceChangeStart;
	FOnSequenceChange OnSequenceChangeFinish;
	
private:
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	
	UPROPERTY(Transient)
		class AGCPlayerCharacter* PlayerCharacter;

	UPROPERTY(Transient)
		class AGCPlayerController* PlayerController;

	int32 NextUUID;
	bool bInProgress;
	FName ThisSequenceID;
	FName LastSequenceID;
	uint8 SubsequenceIndex;

	FORCEINLINE int32 GetNextUUID()
	{
		if (NextUUID > 10000) { NextUUID = 0; }
		return NextUUID++;
	}
	
	void BeginUnloadWorld();
	void BeginLoadWorld();

	UFUNCTION()
		void OnWorldUnloaded();

	UFUNCTION()
		void OnWorldLoaded();

public: // Static Functions

	UFUNCTION(BlueprintPure, Category = "GameSequence")
		static bool IsValidGameSequenceID(const FGCSequenceID InSequenceID);
	
	UFUNCTION(BlueprintPure, Category = "GameSequence")
		static FGCSequenceData GetGameSequenceData(const FGCSequenceID InSequenceID);

	UFUNCTION(BlueprintPure, Category = "GameSequence")
		static bool IsValidGameSequenceData(const FGCSequenceData& InSequenceData);

	UFUNCTION(BlueprintPure, Category = "GameSequence")
		static FGCSequenceID GetLastGameSequenceID(const FGCSequenceID InSequenceID);
	
	UFUNCTION(BlueprintPure, Category = "GameSequence")
		static FGCSequenceID GetNextGameSequenceID(const FGCSequenceID InSequenceID);

	UFUNCTION(BlueprintPure, Category = "GameSequence")
		static int32 GetGameSequenceAsPercent(const FGCSequenceID InSequenceID);

	UFUNCTION(BlueprintPure, Category = "GameSequence", meta = (BlueprintAutocast))
		static FGCSequenceID NameToGameSequence(const FName InName);
};
