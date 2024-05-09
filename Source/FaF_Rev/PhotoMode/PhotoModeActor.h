// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "WatermarkWidget.h"
#include "CineCameraComponent.h"
#include "GameFramework/Actor.h"
#include "PhotoModeActor.generated.h"

class UInputAction;

UCLASS()
class FAF_REV_API APhotoModeActor final : public AActor
{
	GENERATED_BODY()

public:

	APhotoModeActor();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultObjects")
		USceneComponent* SceneRoot;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultObjects")
		UCineCameraComponent* CineCamera;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TSubclassOf<UWatermarkWidgetBase> WatermarkClass;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		float RotationRate;
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayThumbnail = false))
		UInputAction* TurnInput;
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayThumbnail = false))
		UInputAction* CaptureInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PhotoMode")
		class UPhotoModeWidgetBase* PhotoModeWidget;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnScreenshotCaptured, UTexture*);
	FOnScreenshotCaptured OnCaptured;
	
	void ExitMode();
	void EnterMode();
	void Capture(const float InScale, const bool bInWatermark);

protected:

	float ResScale;
	bool bWatermark;

	bool bActive;
	bool bCapturing;
	FRotator Rotation;
	UPROPERTY(Transient) class AFRPlayerBase* Player;
	UPROPERTY(Transient) class AFRPlayerController* Controller;
	UPROPERTY(Transient) UGameViewportClient* Viewport;
	UPROPERTY(Transient) UWatermarkWidgetBase* Watermark;
	UPROPERTY(Transient) UTexture2D* ScreenshotResult;
	
	void InputBinding_Turn(const FInputActionValue& InValue);
	void InputBinding_Capture(const FInputActionValue& InValue);
	void OnScreenCaptured(int32 SizeX, int32 SizeY, const TArray<FColor>& ColorData);
	virtual void BeginPlay() override;

public: // Statics

	static FString GetScreenshotPath();
};
