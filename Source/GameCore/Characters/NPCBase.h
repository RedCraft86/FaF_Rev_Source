// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/GCCharacterInterface.h"
#include "Interfaces/GCInteractionInterface.h"
#include "NPCBase.generated.h"

UCLASS(DisplayName = "Roaming NPC Base")
class GAMECORE_API ANPCBase final : public ACharacter, public IGCInteractionInterface, public IGCCharacterInterface
{
	GENERATED_BODY()

public:
	
	ANPCBase();

	UPROPERTY(VisibleDefaultsOnly, Category = "DefaultSubobjects")
		UArrowComponent* EyePosition;

	UPROPERTY(BlueprintReadOnly, Category = "NPC")
		USceneComponent* LookAtComponent;

	UPROPERTY(EditAnywhere, Category = "NPC", AdvancedDisplay, meta = (GetOptions = "SocketNames"))
		FName HeadSocketName;
	
	UPROPERTY(EditAnywhere, Category = "NPC", AdvancedDisplay, meta = (MakeEditWidget = true))
		FVector LookAtLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
		FText CharacterName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
		bool bCanInteract;

	UFUNCTION(BlueprintPure, Category = "NPC")
		void GetPlayerCameraInfo(float& AngleTo, FVector& Location) const;

	virtual bool GetInteractionInfo_Implementation(FText& DisplayName) override;
	virtual FVector GetEyeWorldLocation_Implementation() override { return EyePosition->GetComponentLocation(); }
	virtual FVector GetEyeForwardVector_Implementation() override { return EyePosition->GetForwardVector(); }
	virtual USceneComponent* GetLookAtComponent_Implementation() override { return LookAtComponent; } 
	
private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(meta = (TransientToolProperty)) TArray<FName> SocketNames = {};
#endif

	virtual void OnConstruction(const FTransform& Transform) override;
};
