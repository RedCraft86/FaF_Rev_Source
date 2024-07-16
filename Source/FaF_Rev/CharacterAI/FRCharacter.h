// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/InlineCurves.h"
#include "SMStateMachineComponent.h"
#include "GameFramework/Character.h"
#include "FRCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterAudioEvent, const class AFRCharacter*, Character, const UAudioComponent*, Component);

UCLASS(Abstract, DisplayName = "Character Base")
class FAF_REV_API AFRCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AFRCharacter();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UAudioComponent> FootstepAudio;

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
		FOnCharacterAudioEvent OnAudioPlayed;

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
		USMStateMachineComponent* GetLogicComponent() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterAI")
		void PlaySmartAudio(UAudioComponent* InComponent);

	UFUNCTION(BlueprintCallable, Category = "CharacterAI")
		void DisableAI();

protected:

	virtual void OnConstruction(const FTransform& Transform) override;
};
