// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExecEnums.h"
#include "Components/ActorComponent.h"
#include "SmartCulling.generated.h"

UCLASS(NotBlueprintable, ClassGroup = (Game), meta = (BlueprintSpawnableComponent))
class GTRUNTIME_API USmartCullingComponent final : public UActorComponent
{
	GENERATED_BODY()

public:

	USmartCullingComponent() : bAffectTicking(false), NullCheckInterval(0.5f), bDisableComponent(false)
		, bCachedHiddenState(false), bInitialTickState(false)
	{
		PrimaryComponentTick.bCanEverTick = false;
		PrimaryComponentTick.bStartWithTickEnabled = false;
	}

	UFUNCTION(BlueprintCallable, Category = "SmartCulling")
		void AddRenderRequest(const UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "SmartCulling")
		void RemoveRenderRequest(const UObject* Object);

	UFUNCTION(BlueprintPure, Category = "SmartCulling")
		bool HasRenderRequest(const UObject* Object) const;
	
	bool IsDisabled() const { return bDisableComponent; }

protected:

	UPROPERTY(EditAnywhere, Category = "Rendering|SmartCulling")
		bool bAffectTicking;

	UPROPERTY(EditAnywhere, Category = "Rendering|SmartCulling")
		float NullCheckInterval;

	UPROPERTY(EditAnywhere, Category = "Rendering|SmartCulling")
		TArray<TSoftObjectPtr<const UObject>> RenderRequests;

	bool bDisableComponent;
	bool bCachedHiddenState;
	bool bInitialTickState;
	FTimerHandle CheckTimer;
	void CheckRenderRequests();
	void UpdateRenderingState();
	virtual void BeginPlay() override;

public: // Static

	UFUNCTION(BlueprintCallable, Category = "SmartCulling", meta = (DefaultToSelf = "Target", ExpandEnumAsExecs = "OutStatus"))
		static USmartCullingComponent* GetSmartCulling(const AActor* Target, EGTFoundPins& OutStatus);
	static USmartCullingComponent* GetSmartCulling(const AActor* Target);
};
