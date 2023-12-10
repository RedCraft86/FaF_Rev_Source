// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "InputActionValue.h"
#include "RCPrimitiveTypes.h"
#include "GameFramework/Actor.h"
#include "GCInspectionActor.generated.h"

class UInputAction;

UCLASS(NotBlueprintable, DisplayName = "Inspection Actor")
class GAMECORE_API AGCInspectionActor final : public AActor
{
	GENERATED_BODY()

public:

	AGCInspectionActor();
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultObjects")
		USceneComponent* SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UGCCameraSmoother* MeshSmoother;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultObjects")
		class UStaticMeshComponent* PreviewMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultObjects")
		class USpotLightComponent* PreviewLight;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultObjects")
		class USceneCaptureComponent2D* CaptureCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		FRCStaticMeshProperties DefaultProperties;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (MakeEditWidget = true))
		FVector LightLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ClampMin = 10.0f, UIMin = 10.0f))
		float CameraDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		UInputAction* TurnInput;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		UInputAction* ZoomInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		UTextureRenderTarget2D* RenderTarget;

	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	void InputBinding_Turn(const FInputActionValue& InValue);
	void InputBinding_Zoom(const FInputActionValue& InValue);
	void SetPreviewItemID(const FName& InItemID);
	void SetInspectionPaused(const bool bPaused) const;
	void BeginInspection();
	void EndInspection();
	
private:
	
	FName ItemID;
	FVector2D ZoomRange;
	float TargetZoom;
	float CurrentZoom;

	virtual void BeginPlay() override;
};
