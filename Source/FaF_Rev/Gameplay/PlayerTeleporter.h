// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FRPlayer.h"
#include "Engine/TargetPoint.h"
#include "GameplayTagContainer.h"
#include "PlayerTeleporter.generated.h"

UCLASS(NotBlueprintable)
class FAF_REV_API APlayerTeleporter final : public ATargetPoint
{
	GENERATED_BODY()

public:

	APlayerTeleporter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		FGameplayTagContainer TeleporterIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TSoftObjectPtr<AFRPlayerBase> PlayerReference;

	UFUNCTION(BlueprintCallable, Category = "PlayerTeleporter")
		void TeleportPlayer() const;

private:
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override
	{
		Super::OnConstruction(Transform);
		PlayerReference = FRPlayer(this);
		SetActorScale3D(FVector::OneVector);
		if (FRotator Rotation = GetActorRotation(); !FMath::IsNearlyZero(Rotation.Roll))
		{
			Rotation.Roll = 0.0f;
			SetActorRotation(Rotation);
		}
	}
#endif
public: // Statics
	
	UFUNCTION(BlueprintPure, Category = "Game", meta = (WorldContext = "WorldContextObject"))
		static APlayerTeleporter* FindTeleporter(const UObject* WorldContextObject, const FGameplayTag InFilter);
};
