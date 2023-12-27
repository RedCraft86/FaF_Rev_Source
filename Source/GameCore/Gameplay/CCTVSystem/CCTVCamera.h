// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "CCTVCamera.generated.h"

UCLASS(Abstract, DisplayName = "CCTV Camera")
class GAMECORE_API ACCTVCamera final : public AActor
{
	GENERATED_BODY()

	friend class ACCTVMonitor;

public:

	ACCTVCamera();

	UPROPERTY(/*VisibleDefaultsOnly, */BlueprintReadOnly, Category = "DefaultSubobjects")
		USceneComponent* SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(EditAnywhere, Category = "CCTV", meta = (MakeEditWidget = true))
		FVector LookTarget;
	
	UPROPERTY(EditAnywhere, Category = "CCTV")
		FVector2D MinXY;

	UPROPERTY(EditAnywhere, Category = "CCTV")
		FVector2D MaxXY;

	UFUNCTION(BlueprintImplementableEvent)
		USceneComponent* GetAttachmentRoot();

	UFUNCTION(BlueprintNativeEvent)
		void SetCameraRotation(const FRotator& Rotation);
	void SetCameraRotation_Implementation(const FRotator& Rotation);

private:

	FRotator BaseOffset;
	FVector2D CurrentOffset;

	void SetState(const bool bActive);
	bool IsActorOnCamera(const AActor* Actor);
	void AddTurnInput(const float X, const float Y);

	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
