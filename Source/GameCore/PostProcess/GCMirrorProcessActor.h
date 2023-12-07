// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GCMirrorProcessActor.generated.h"

UCLASS(NotBlueprintable, DisplayName = "Mirror Process Actor", HideCategories = (Rendering, Collision, Physics, Input))
class GAMECORE_API AGCMirrorProcessActor final : public AActor
{
	GENERATED_BODY()

public:

	AGCMirrorProcessActor();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UBoxComponent* SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UPostProcessComponent* PostProcess;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float Priority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TArray<class APlanarReflection*> PlanarReflections;

private:
#if WITH_EDITOR
	UBillboardComponent* VisualIcon;
#endif

	UPROPERTY(Transient)
		class UGCUserSettings* GameSettings;

	bool bPointInBox;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void ApplyState();
	void OnSettingChanged(UGCUserSettings* Settings);
};
