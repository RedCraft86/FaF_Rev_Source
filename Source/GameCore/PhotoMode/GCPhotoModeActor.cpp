// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCPhotoModeActor.h"
#include "GCWatermarkWidget.h"
#include "Player/GCPlayerCharacter.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "GameFramework/InputSettings.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "Slate/SceneViewport.h"
#include "HighResScreenshot.h"
#include "Misc/FileHelper.h"
#include "Async/Async.h"
#include "ImageUtils.h"

AGCPhotoModeActor::AGCPhotoModeActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
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

#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
#endif

	WatermarkWidgetClass = NULL;
	RotationRate = 25.0f;
	CaptureInput = nullptr;
	TurnInput = nullptr;
	
	ScreenshotImage = nullptr;
	WatermarkWidget = nullptr;
	
	bActive = false;
	bWaitForUI = false;
	bCapturing = false;
	Rotation = FRotator::ZeroRotator;
	bWatermarkImage = true;
	ResolutionScale = 1.0f;
	CaptureTimer = 0.0f;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AGCPhotoModeActor::InputBinding_Capture(const FInputActionValue& InValue)
{
	if (!bWaitForUI && !bCapturing)
	{
		bCapturing = true;
		BeginCapture();
	}
}

void AGCPhotoModeActor::InputBinding_Turn(const FInputActionValue& InValue)
{
	if (!bWaitForUI && !bCapturing)
	{
		const FVector Value = InValue.Get<FVector>();
		const float Delta = GetWorld()->DeltaRealTimeSeconds * RotationRate;

		Rotation = {
			FMath::Clamp(Rotation.Pitch + (Value.X * Delta), -25.0f, 25.0f),
			FMath::Clamp(Rotation.Yaw + (Value.Y * Delta), -25.0f, 25.0f),
			FMath::Clamp(Rotation.Roll + (Value.Z * Delta), -45.0f, 45.0f)
		};
		
		CineCamera->SetRelativeRotation(Rotation);
	}
}

FString AGCPhotoModeActor::GetScreenshotPath()
{
#if WITH_EDITOR
	return FPaths::ProjectSavedDir() / TEXT("Screenshots/");
#else
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("Screenshots/"));
#endif
}

void AGCPhotoModeActor::CopyDataToTexture2D(UTexture2D* Texture, const int32 SizeX, const int32 SizeY, const TArray<FColor>& ColorData)
{
	if (Texture && !ColorData.IsEmpty())
	{
		const int32 _SizeX = SizeX == 0 ? Texture->GetSizeX() : SizeX;
		const int32 _SizeY = SizeX == 0 ? Texture->GetSizeY() : SizeY;

		FTexturePlatformData* PlatformData = Texture->GetPlatformData();
		void* TextureData = PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, ColorData.GetData(), 4 * _SizeX * _SizeY);
		PlatformData->Mips[0].BulkData.Unlock();
		Texture->SetPlatformData(PlatformData);
		Texture->UpdateResource();
	}
}

void AGCPhotoModeActor::StartPhotoMode(AGCPlayerController* Controller)
{
	if (bActive)
		return;

	bActive = true;
	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	SetActorLocationAndRotation(ViewLocation, ViewRotation);
	
	CineCamera->SetRelativeRotation(FRotator(0.0f));
	CineCamera->SetFieldOfView(Controller->GetPlayer()->PlayerCamera->FieldOfView);
	
	EnableInput(Controller);
	Controller->SetViewTargetWithBlend(this, 0.25f);
}

void AGCPhotoModeActor::FinishPhotoMode(AGCPlayerController* Controller)
{
	if (!bActive)
		return;
	
	Rotation = FRotator(0.0f);
	if (ScreenshotImage)
	{
		ScreenshotImage->MarkAsGarbage();
		ScreenshotImage = nullptr;
	}

	DisableInput(Controller);
	Controller->SetViewTargetWithBlend(Controller->GetPlayer(), 1.0f, VTBlend_EaseIn, 0.75);
	bActive = false;

	SetActorTickEnabled(false);
}

void AGCPhotoModeActor::BeginCapture()
{
	SetActorTickEnabled(true);
	ScreenshotImage = nullptr;
	OnCaptureStarted.Broadcast();
	CaptureTimer = 0.4f;
}

void AGCPhotoModeActor::OnScreenshotCaptured(int32 SizeX, int32 SizeY, const TArray<FColor>& ColorData)
{
	SetActorTickEnabled(false);
	TArray<FColor> PixelData = ColorData;
	
	ScreenshotImage = UTexture2D::CreateTransient(SizeX, SizeY, PF_B8G8R8A8);
	if (ScreenshotImage)
	{
		CopyDataToTexture2D(ScreenshotImage, SizeX, SizeY, PixelData);
		if (WatermarkWidget && bWatermarkImage)
		{
			if (UCanvasRenderTarget2D* RT = WatermarkWidget->ProcessImage(ScreenshotImage))
			{
				RT->GameThread_GetRenderTargetResource()->ReadPixels(PixelData);
				CopyDataToTexture2D(ScreenshotImage, SizeX, SizeY, PixelData);
				RT->MarkAsGarbage();
			}
		}
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [SizeX, SizeY, PixelData]() {
		TArray64<uint8> ImageBinary;
		FImageUtils::PNGCompressImageArray(SizeX, SizeY, PixelData, ImageBinary);
		FFileHelper::SaveArrayToFile(ImageBinary, *(GetScreenshotPath() / FDateTime::Now().ToString() + TEXT(".png")));
	});

	OnCaptureFinished.Broadcast(ScreenshotImage);
	bWaitForUI = true;
	bCapturing = false;
}

void AGCPhotoModeActor::BeginPlay()
{
	Super::BeginPlay();
	
	WatermarkWidget = CreateWidget<UGCWatermarkWidget>(GetWorld(), WatermarkWidgetClass);
	
	CreateInputComponent(UInputSettings::GetDefaultInputComponentClass());
	if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		InputComp->BindAction(CaptureInput, ETriggerEvent::Started, this, &AGCPhotoModeActor::InputBinding_Capture);
		InputComp->BindAction(TurnInput, ETriggerEvent::Triggered, this, &AGCPhotoModeActor::InputBinding_Turn);
	}
}

void AGCPhotoModeActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CaptureTimer > 0.0f)
	{
		CaptureTimer -= DeltaSeconds;
		if (CaptureTimer <= 0.0f)
		{
			UGameViewportClient* Viewport = GetWorld()->GetGameViewport();
	
			Viewport->OnScreenshotCaptured().RemoveAll(this);
			Viewport->OnScreenshotCaptured().AddUObject(this, &AGCPhotoModeActor::OnScreenshotCaptured);
	
			FVector2D ViewportSize;
			Viewport->GetViewportSize(ViewportSize);
			GetHighResScreenshotConfig().SetResolution(
				FMath::Clamp(ViewportSize.X * ResolutionScale, 1024, GetMax2DTextureDimension()), 
				FMath::Clamp(ViewportSize.Y * ResolutionScale, 1024, GetMax2DTextureDimension()),
				1.0f);

			if (!Viewport->GetGameViewport()->TakeHighResScreenShot())
			{
				OnCaptureFinished.Broadcast(nullptr);
				bCapturing = false;
			}
		}
	}
}
