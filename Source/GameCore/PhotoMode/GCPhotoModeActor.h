// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "InputActionValue.h"
#include "CineCameraComponent.h"
#include "GameFramework/Actor.h"
#include "Player/GCPlayerController.h"
#include "GCPhotoModeActor.generated.h"

class UInputAction;
class UCanvasRenderTarget2D;

UCLASS(NotBlueprintable, DisplayName = "Photo Mode Actor")
class GAMECORE_API AGCPhotoModeActor final : public AActor
{
	GENERATED_BODY()

	friend class UGCPhotoModeWidget;

public:

	AGCPhotoModeActor();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultObjects")
		USceneComponent* SceneRoot;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultObjects")
		UCineCameraComponent* CineCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowAbstract = false))
		TSubclassOf<class UGCWatermarkWidget> WatermarkWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		float RotationRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		UInputAction* CaptureInput;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		UInputAction* TurnInput;

	UFUNCTION(BlueprintCallable, Category = "PhotoModeActor")
		FPostProcessSettings& GetPostProcessing() const { return CineCamera->PostProcessSettings; }

	UFUNCTION(BlueprintPure, Category = "PhotoModeActor")
		UTexture2D* GetScreenshotImage() const { return ScreenshotImage; }
	
	DECLARE_MULTICAST_DELEGATE(FOnCaptureStarted)
	FOnCaptureStarted OnCaptureStarted;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCaptureFinished, UTexture2D*)
	FOnCaptureFinished OnCaptureFinished;
	
	void InputBinding_Capture(const FInputActionValue& InValue);
	void InputBinding_Turn(const FInputActionValue& InValue);

	static FString GetScreenshotPath();
	static void CopyDataToTexture2D(UTexture2D* Texture, const int32 SizeX, const int32 SizeY, const TArray<FColor>& ColorData);
	
private:

	UPROPERTY(Transient)
		UTexture2D* ScreenshotImage;
	
	UPROPERTY(Transient)
		UGCWatermarkWidget* WatermarkWidget;

	bool bActive;
	bool bWaitForUI;
	bool bCapturing;
	FRotator Rotation;
	bool bWatermarkImage;
	float ResolutionScale;
	float CaptureTimer;

	void StartPhotoMode(AGCPlayerController* Controller);
	void FinishPhotoMode(AGCPlayerController* Controller);

	void BeginCapture();
	void OnScreenshotCaptured(int32 SizeX, int32 SizeY, const TArray<FColor>& ColorData);
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
