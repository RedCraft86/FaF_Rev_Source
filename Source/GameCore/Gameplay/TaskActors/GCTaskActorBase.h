// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GCTaskActorBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskCompleteBP, class AGCTaskActorBase*, TaskActor);

UCLASS(Abstract, DisplayName = "Task Actor Base")
class GAMECORE_API AGCTaskActorBase : public AActor
{
	GENERATED_BODY()

public:

	AGCTaskActorBase();

	UPROPERTY(BlueprintAssignable, DisplayName = "On Task Completed")
		FOnTaskCompleteBP OnTaskCompletedBP;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultObjects")
		USceneComponent* SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultObjects")
		class UBillboardComponent* IconBillboard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor")
		bool bStartLocked;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor")
		bool bSingleUse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor|Narrative")
		class UNarrativeDataTask* NarrativeTask;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor|Narrative", meta = (MultiLine = false, EditCondition = "NarrativeTask != nullptr"))
		FString TaskArgument;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor|Narrative", meta = (ClampMin = 1, UIMin = 1, EditCondition = "NarrativeTask != nullptr"))
		uint8 TaskAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor|Rendering")
		UTexture2D* IconImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor|Rendering", meta = (MakeEditWidget = true, EditCondition = "IconImage != nullptr"))
		FVector IconLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaskActor|Rendering", meta = (ClampMin = 0.1f, UIMin = 0.1f, EditCondition = "IconImage != nullptr"))
		float IconScale;

	UFUNCTION(BlueprintImplementableEvent, Category = "TaskActor", DisplayName = "Completed")
		void OnCompletedEvent();

	UFUNCTION(BlueprintCallable, Category = "TaskActor")
		virtual void LockTask();
	
	UFUNCTION(BlueprintCallable, Category = "TaskActor")
		virtual void UnlockTask();

	UFUNCTION(BlueprintCallable, Category = "TaskActor")
		virtual void ResetTask();
	
	UFUNCTION(BlueprintCallable, Category = "TaskActor")
		virtual void CompleteTask();

	UFUNCTION(BlueprintPure, Category = "TaskActor")
		FORCEINLINE bool IsCompleted() const { return bCompleted; }

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnTaskComplete, AGCTaskActorBase*);
	FOnTaskComplete OnTaskCompleted;
	
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	
	bool bCompleted;
	bool bTickEnabled;
	
	virtual void BeginPlay() override;
};
