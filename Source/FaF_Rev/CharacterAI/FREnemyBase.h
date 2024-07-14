// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/InlineCurves.h"
#include "GameFramework/Character.h"
#include "FREnemyBase.generated.h"

UENUM(BlueprintType, DisplayName = "Enemy AI Mode")
enum class EEnemyState : uint8
{
	None,
	Roam,
	Alert,
	Chase,
	Search
};
ENUM_RANGE_BY_FIRST_AND_LAST(EEnemyState, EEnemyState::None, EEnemyState::Search);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyAudioEvent, const class AFREnemyBase*, Enemy, const UAudioComponent*, Component);

UCLASS(Abstract, DisplayName = "Enemy Character Base")
class FAF_REV_API AFREnemyBase : public ACharacter
{
	GENERATED_BODY()

public:

	AFREnemyBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UAudioComponent> FootstepAudio;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bStartRoaming;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FInlineFloatCurve AudioVolumeCurve;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		bool bDebugAudio = false;
	
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		FColor DebugColor = FColor::MakeRandomColor();
#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay)
		TSoftObjectPtr<class AFRPlayerBase> PlayerChar;

	UPROPERTY(BlueprintAssignable)
		FOnEnemyAudioEvent OnAudioPlayed;

	UFUNCTION(BlueprintCallable, Category = "EnemyAI")
		void PlayFootstep();

	UFUNCTION(BlueprintCallable, Category = "EnemyAI")
		void SetEnemyState(const EEnemyState InNewState);

	UFUNCTION(BlueprintPure, Category = "EnemyAI")
		EEnemyState GetEnemyState() const { return EnemyState; }

	UFUNCTION(BlueprintCallable, Category = "EnemyAI")
		void PlaySmartAudio(UAudioComponent* InComponent);

protected:
	
	UPROPERTY() EEnemyState EnemyState;

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public: // Statics

	UFUNCTION(BlueprintPure, Category = "EnemyAI")
		static EEnemyState GetHighestEnemyState(const TArray<AFREnemyBase*>& InEnemies);
};
