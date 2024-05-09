// Copyright (C) RedCraft86. All Rights Reserved.

#include "PhotoModeActor.h"
#include "HighResScreenshot.h"
#include "Slate/SceneViewport.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Libraries/GTLoadUtilsLibrary.h"
#include "Libraries/GTTextureUtilsLibrary.h"
#include "Menus/PhotoModeWidget.h"
#include "FRPlayerController.h"
#include "FRPlayer.h"

APhotoModeActor::APhotoModeActor()
	: WatermarkClass(nullptr), RotationRate(25.0f), TurnInput(nullptr), CaptureInput(nullptr)
	, ResolutionScale(1.0f), bUseWatermark(true), bActive(false), bCapturing(false), Rotation(FRotator::ZeroRotator)
	, Viewport(nullptr), Watermark(nullptr), ScreenshotResult(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	CineCamera = CreateDefaultSubobject<UCineCameraComponent>("CineCamera");
	CineCamera->SetConstraintAspectRatio(false);
	CineCamera->SetCurrentFocalLength(11.88f);
	CineCamera->SetCurrentAperture(4.0f);
	CineCamera->FocusSettings.FocusMethod = ECameraFocusMethod::Manual;
	CineCamera->FocusSettings.ManualFocusDistance = 2000.0f;
	CineCamera->PostProcessSettings.bOverride_SceneFringeIntensity = true;
	CineCamera->PostProcessSettings.SceneFringeIntensity = 0.0f;
	CineCamera->PostProcessSettings.bOverride_VignetteIntensity = true;
	CineCamera->PostProcessSettings.VignetteIntensity = 0.4f;
	CineCamera->PostProcessSettings.bOverride_LensFlareIntensity = true;
	CineCamera->PostProcessSettings.LensFlareIntensity = 0.0f;
	CineCamera->SetupAttachment(SceneRoot);
}

void APhotoModeActor::ExitMode()
{
	if (!bActive) return;
	DisableInput(Controller);
	SetActorTickEnabled(false);
	ScreenshotResult = nullptr;
	Controller->SetViewTargetWithBlend(Player, 1.0f, VTBlend_EaseIn, 0.75f);
	UGTLoadUtilsLibrary::ForceGarbageCollection();
	bActive = false;
}

void APhotoModeActor::EnterMode()
{
	if (bActive || !IsValid(PhotoModeWidget)) return;
	
	bActive = true;
	UGTLoadUtilsLibrary::ForceGarbageCollection();
	SetActorTickEnabled(true);
	
	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	SetActorLocationAndRotation(ViewLocation, ViewRotation);
	CineCamera->SetRelativeRotation(FRotator::ZeroRotator);
	CineCamera->SetFieldOfView(Player->PlayerCamera->FieldOfView);
	
	EnableInput(Controller);
	Controller->SetViewTargetWithBlend(this, 0.25f);
}

void APhotoModeActor::TakeScreenshot()
{
	ScreenshotResult = nullptr;
	if (Viewport)
	{
		bCapturing = true;
		FVector2D ViewportSize;
		Viewport->GetViewportSize(ViewportSize);
		GetHighResScreenshotConfig().SetResolution(
			FMath::Clamp(ViewportSize.X * ResolutionScale, 2048, GetMax2DTextureDimension()), 
			FMath::Clamp(ViewportSize.Y * ResolutionScale, 2048, GetMax2DTextureDimension()),
			1.0f);
		
		if (!Viewport->GetGameViewport()->TakeHighResScreenShot())
		{
			OnCaptured.Broadcast(nullptr);
			bCapturing = false;
		}
	}
}

void APhotoModeActor::InputBinding_Turn(const FInputActionValue& InValue)
{
	if (!bCapturing)
	{
		const FVector Value = InValue.Get<FVector>() * GetWorld()->DeltaRealTimeSeconds * RotationRate;
		Rotation = {
			FMath::Clamp(Rotation.Pitch + Value.X, -25.0f, 25.0f),
			FMath::Clamp(Rotation.Yaw + Value.Y, -25.0f, 25.0f),
			FMath::Clamp(Rotation.Roll + Value.Z, -45.0f, 45.0f)
		};
		
		CineCamera->SetRelativeRotation(Rotation);
	}
}

void APhotoModeActor::InputBinding_Capture(const FInputActionValue& InValue)
{
	if (!bCapturing && PhotoModeWidget)
	{
		bCapturing = true;
		PhotoModeWidget->BeginCapture();
	}
}

void APhotoModeActor::OnScreenCaptured(int32 SizeX, int32 SizeY, const TArray<FColor>& ColorData)
{
	UTexture2D* OutTex = UGTTextureUtilsLibrary::CreateTextureFromData({ColorData, SizeX, SizeY});
	if (bUseWatermark)
	{
		ScreenshotResult = Watermark->AddWatermark(OutTex);
	}
	else
	{
		ScreenshotResult = OutTex;
	}
	
	OutTex = nullptr;
	Watermark->ClearImage();
	UGTTextureUtilsLibrary::SaveTextureToFile(ScreenshotResult, GetScreenshotPath() / FDateTime::Now().ToString());
	OnCaptured.Broadcast(ScreenshotResult);
	bCapturing = false;
}

void APhotoModeActor::BeginPlay()
{
	Super::BeginPlay();
	Player = FRPlayer(this);
	Controller = FRPlayerController(this);
	Viewport = GetWorld()->GetGameViewport();
	Watermark = CreateWidget<UWatermarkWidgetBase>(GetWorld(), WatermarkClass);
	if (Viewport) Viewport->OnScreenshotCaptured().AddUObject(this, &APhotoModeActor::OnScreenCaptured);

	CreateInputComponent(UInputSettings::GetDefaultInputComponentClass());
	if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		InputComp->BindAction(TurnInput, ETriggerEvent::Triggered, this, &APhotoModeActor::InputBinding_Turn);
		InputComp->BindAction(CaptureInput, ETriggerEvent::Started, this, &APhotoModeActor::InputBinding_Capture);
	}
}

FString APhotoModeActor::GetScreenshotPath()
{
#if WITH_EDITOR
	return FPaths::ProjectSavedDir() / TEXT("Screenshots/");
#else
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("Screenshots/"));
#endif
}
