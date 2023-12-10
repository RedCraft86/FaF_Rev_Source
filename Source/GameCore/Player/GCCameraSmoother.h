// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "GCCameraSmoother.generated.h"

UCLASS(DisplayName = "Camera Smoother", NotBlueprintable, ClassGroup = (Gameplay), meta = (BlueprintSpawnableComponent))
class GAMECORE_API UGCCameraSmoother final : public USceneComponent
{
	GENERATED_BODY()

	friend class AGCPlayerCharacter;
	
public:

	UGCCameraSmoother();

	static const FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraSmoother")
		bool bEnabled;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSmoother", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float LagSpeed;

	UFUNCTION(BlueprintCallable, Category = "CameraSmoother")
		void SetSmootherEnabled(const bool bInEnabled);
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool HasAnySockets() const override { return true; }
	virtual void QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const override;
	virtual FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace = RTS_World) const override;

	FRotator GetDesiredRotation() const;

	UFUNCTION(BlueprintCallable, Category = "CameraSmoother")
		void OverrideRelativeRotation(const FRotator NewRotation);

	UFUNCTION(BlueprintCallable, Category = "CameraSmoother")
		void OverrideWorldRotation(const FRotator NewRotation);
	
private:

	bool bIsLockOn;

	FRotator PreviousDesiredRotation;
	FRotator RelativeSocketRotation;
};
