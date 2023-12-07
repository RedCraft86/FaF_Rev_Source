// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "Core/CoreLogging.h"
#include "Components/ActorComponent.h"
#include "GCLightFunctionComponent.generated.h"

UCLASS(NotBlueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), DisplayName = "Light Function Component")
class GAMECORE_API UGCLightFunctionComponent final : public UActorComponent
{
	GENERATED_BODY()

public:

	UGCLightFunctionComponent();
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		UMaterialInterface* LightFunction;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ReadOnlyKeys))
		TMap<FName, float> ScalarParams;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ReadOnlyKeys))
		TMap<FName, FLinearColor> VectorParams;
	
	UFUNCTION(CallInEditor, Category = "Settings")
		void Update();

private:

	UPROPERTY(Transient)
		UMaterialInstanceDynamic* MID;

	bool bFailed;

	virtual void BeginPlay() override;
};
