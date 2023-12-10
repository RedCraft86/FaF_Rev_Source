// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GCLightAnimator.h"
#include "GCElectricalTypes.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GCElectricalActor.generated.h"

UCLASS(NotBlueprintable, NotBlueprintType, DisplayName = "Electronic Visualizer")
class GAMECORE_API UGCElectronicVisualizer final : public UActorComponent
{
	GENERATED_BODY()

public:

	UGCElectronicVisualizer();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITOR
	TArray<FVector> GetNextTargets() const;
	TArray<FVector> GetLastTargets() const;
#endif
};

UCLASS(Abstract, NotBlueprintable, DisplayName = "Electrical Actor")
class GAMECORE_API AGCElectricalActor : public AActor
{
	GENERATED_BODY()

	friend class UGCElectronicVisualizer;

public:

	AGCElectricalActor();

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, Category = "Electronic", meta = (EditCondition = "!bForceStaticType"))
		EGCElectronicType ElectronicType;
	
	UPROPERTY(EditAnywhere, Category = "Electronic", meta = (EditCondition = "ElectronicType != EGCElectronicType::Connector"))
		bool bLocalState;

	UPROPERTY(EditAnywhere, Category = "Electronic", meta = (EditCondition = "ElectronicType != EGCElectronicType::Connector"))
		bool bIsDamaged;

	UPROPERTY(EditAnywhere, Category = "Electronic")
		FGameplayTag ElectronicTag;

	UPROPERTY(EditAnywhere, Category = "Electronic", meta = (EditCondition = "ElectronicType != EGCElectronicType::Generator"))
		EGCConditionType ConditionType;

	UPROPERTY(EditAnywhere, Category = "Electronic")
		bool bDrawDebugLines;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Electronic")
		bool bForceStaticType = false;
#endif

	UPROPERTY(EditAnywhere, Category = "ElectronicLinks", meta = (EditCondition = "ElectronicType != EGCElectronicType::Appliance"))
		TSet<AGCElectricalActor*> NextLinks;
	
	UPROPERTY(EditAnywhere, Category = "ElectronicLinks", meta = (EditCondition = "ElectronicType != EGCElectronicType::Generator"))
		TSet<AGCElectricalActor*> LastLinks;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "NextLinksFinder", meta = (EditCondition = "ElectronicType != EGCElectronicType::Appliance"))
		TSubclassOf<AGCElectricalActor> LinkClass = NULL;

	UPROPERTY(EditAnywhere, Category = "NextLinksFinder", meta = (EditCondition = "ElectronicType != EGCElectronicType::Appliance"))
		FGameplayTagContainer LinkTags = {};
#endif
	
	UFUNCTION(CallInEditor, Category = "ElectronicLinks")
		void ResyncLinks();

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "NextLinksFinder")
		void FindNextLinks();
#endif

public:
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "State Changed")
		void EventStateChanged(bool bState);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Damaged")
		void EventDamaged(bool bDamaged);

	UFUNCTION(BlueprintCallable, Category = "ElectronicActor")
		void RefreshElectronic();

	UFUNCTION(BlueprintPure, Category = "ElectronicActor")
		FORCEINLINE FGameplayTag GetElectronicTag() const { return ElectronicTag; }

	UFUNCTION(BlueprintPure, Category = "ElectronicActor")
		FORCEINLINE bool GetLocalState() const { return bLocalState; }
		
	UFUNCTION(BlueprintCallable, Category = "ElectronicActor")
		void SetLocalState(const bool bNewState);

	UFUNCTION(BlueprintCallable, Category = "ElectronicActor")
		void ToggleLocalState();

	UFUNCTION(BlueprintPure, Category = "ElectronicActor")
		FORCEINLINE bool GetIsDamaged() const { return bIsDamaged; }
	
	UFUNCTION(BlueprintCallable, Category = "ElectronicActor")
		void SetIsDamaged(const bool bNewDamaged);

	UFUNCTION(BlueprintPure, Category = "ElectronicActor")
		bool EvaluateConditions() const;

	UFUNCTION(BlueprintPure, Category = "ElectronicActor")
		virtual bool GetFinalState() const;
	
protected:
#if WITH_EDITOR
	UGCElectronicVisualizer* DebugVisualizer = nullptr;
#endif

	bool bFirstUpdate;
	bool bLastFinalState;
	
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnStateChanged(const bool bState);
	virtual void OnDamaged(const bool bDamaged);
};

UCLASS(Abstract, Blueprintable, DisplayName = "Electrical Switch")
class GAMECORE_API AGCElectricalSwitch : public AGCElectricalActor
{
	GENERATED_BODY()

public:

	AGCElectricalSwitch()
	{
		PrimaryActorTick.bCanEverTick = true;
		PrimaryActorTick.bStartWithTickEnabled = false;
	
		ElectronicType = EGCElectronicType::Toggle;
#if WITH_EDITOR
		bForceStaticType = true;
#endif
	}
};

UCLASS(Abstract, Blueprintable, DisplayName = "Electrical Wire")
class GAMECORE_API AGCElectricalWire : public AGCElectricalActor
{
	GENERATED_BODY()

public:

	AGCElectricalWire()
	{
		PrimaryActorTick.bCanEverTick = true;
		PrimaryActorTick.bStartWithTickEnabled = false;
	
		ElectronicType = EGCElectronicType::Connector;
#if WITH_EDITOR
		bForceStaticType = true;
#endif
	}
};

UCLASS(Abstract, Blueprintable, DisplayName = "Electrical Light", HideCategories = (ElectronicLinks, NextLinksFinder))
class GAMECORE_API AGCElectricalLight : public AGCElectricalActor
{
	GENERATED_BODY()

public:

	AGCElectricalLight();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Instanced, Category = "Settings|Animation")
		UGCLightAnimator* AnimBroken;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Instanced, Category = "Settings|Animation")
		UGCLightAnimator* AnimEnable;

	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction = true))
		void ApplyProperties(const float IntensityMulti, const FLinearColor& ColorMulti);

	UFUNCTION(BlueprintPure, Category = "ElectronicActor")
		void GetAnimMultipliers(float& OutIntensity, FLinearColor& OutColor) const;
	
	virtual bool GetFinalState() const override
	{
		return bLocalState && EvaluateConditions();
	}

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void ExecuteApply();
	virtual void OnDamaged(const bool bDamaged) override;
	virtual void OnStateChanged(const bool bState) override;
};
	