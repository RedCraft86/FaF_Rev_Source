// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCInspectionActor.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/InputSettings.h"
#include "Components/SpotLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Player/GCCameraSmoother.h"
#include "GCInventoryManager.h"
#include "Core/GCSettings.h"
#if WITH_EDITOR
#include "UObject/ConstructorHelpers.h"
#endif

AGCInspectionActor::AGCInspectionActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
#endif
	
	MeshSmoother = CreateDefaultSubobject<UGCCameraSmoother>("MeshSmoother");
	MeshSmoother->SetupAttachment(GetRootComponent());
	
	PreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>("PreviewMesh");
	PreviewMesh->SetupAttachment(MeshSmoother, UGCCameraSmoother::SocketName);

	PreviewLight = CreateDefaultSubobject<USpotLightComponent>("PreviewLight");
	PreviewLight->SetRelativeLocation(FVector(-150.0f, 100.0f, 80.0f));
	PreviewLight->SetRelativeRotation(FRotator(-35.0f, 30.0f, 0.0f));
	PreviewLight->SetupAttachment(GetRootComponent());
	PreviewLight->SetAttenuationRadius(350.0f);

	CaptureCamera = CreateDefaultSubobject<USceneCaptureComponent2D>("CaptureCamera");
	CaptureCamera->SetRelativeLocation(FVector(-150.0f, 0.0f, 0.0f));
	CaptureCamera->SetupAttachment(GetRootComponent());
	CaptureCamera->bAlwaysPersistRenderingState = true;
	CaptureCamera->FOVAngle = 60.0f;
	
	CaptureCamera->ShowFlags.SetAntiAliasing(true);
	CaptureCamera->ShowFlags.SetAtmosphere(false);
	CaptureCamera->ShowFlags.SetBSP(false);
	CaptureCamera->ShowFlags.SetCloud(false);
	CaptureCamera->ShowFlags.SetDecals(false);
	CaptureCamera->ShowFlags.SetFog(false);
	CaptureCamera->ShowFlags.SetLandscape(false);
	CaptureCamera->ShowFlags.SetParticles(false);
	CaptureCamera->ShowFlags.SetSkeletalMeshes(false);
	CaptureCamera->ShowFlags.SetStaticMeshes(true);
	CaptureCamera->ShowFlags.SetTranslucency(true);
	CaptureCamera->ShowFlags.SetDeferredLighting(true);
	CaptureCamera->ShowFlags.SetInstancedFoliage(false);
	CaptureCamera->ShowFlags.SetInstancedGrass(false);
	CaptureCamera->ShowFlags.SetInstancedStaticMeshes(false);
	CaptureCamera->ShowFlags.SetNaniteMeshes(true);
	CaptureCamera->ShowFlags.SetPaper2DSprites(false);
	CaptureCamera->ShowFlags.SetTextRender(false);
	CaptureCamera->ShowFlags.SetTemporalAA(true);
	CaptureCamera->ShowFlags.SetBloom(true);
	CaptureCamera->ShowFlags.SetEyeAdaptation(true);
	CaptureCamera->ShowFlags.SetLocalExposure(true);
	CaptureCamera->ShowFlags.SetMotionBlur(false);
	CaptureCamera->ShowFlags.SetToneCurve(true);
	CaptureCamera->ShowFlags.SetSkyLighting(true);
	CaptureCamera->ShowFlags.SetAmbientOcclusion(false);
	CaptureCamera->ShowFlags.SetDynamicShadows(true);
	CaptureCamera->ShowFlags.SetAmbientCubemap(true);
	CaptureCamera->ShowFlags.SetDistanceFieldAO(true);
	CaptureCamera->ShowFlags.SetLightFunctions(false);
	CaptureCamera->ShowFlags.SetLightShafts(false);
	CaptureCamera->ShowFlags.SetReflectionEnvironment(true);
	CaptureCamera->ShowFlags.SetScreenSpaceReflections(true);
	CaptureCamera->ShowFlags.SetTexturedLightProfiles(false);
	CaptureCamera->ShowFlags.SetVolumetricFog(false);
	CaptureCamera->ShowFlags.SetGame(true);
	CaptureCamera->ShowFlags.SetLighting(true);
	CaptureCamera->ShowFlags.SetPathTracing(false);
	CaptureCamera->ShowFlags.SetPostProcessing(true);

#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
#endif
#if WITH_EDITOR
	DefaultProperties.bCastShadows = false;
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshFinder.Succeeded())
	{
		DefaultProperties.Mesh = MeshFinder.Object;
		PreviewMesh->SetStaticMesh(MeshFinder.Object);
	}
	const ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MaterialFinder.Succeeded())
	{
		DefaultProperties.Materials.Add(MaterialFinder.Object);
		PreviewMesh->SetMaterial(0, MaterialFinder.Object);
	}
#else
	DefaultProperties = { nullptr, {}, false };
#endif
	
	LightLocation = FVector(-150.0f, 100.0f, 80.0f);
	CameraDistance = 200.0f;
	TurnInput = nullptr;
	ZoomInput = nullptr;
	RenderTarget = nullptr;
	
	ItemID = NAME_None;
	ZoomRange = { 0.25f, 2.0f };
	TargetZoom = 1.0f;
	CurrentZoom = 1.0f;
}

void AGCInspectionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetZoom != CurrentZoom)
	{
		CurrentZoom = FMath::FInterpTo(CurrentZoom, TargetZoom, DeltaTime, 10.0f);
		MeshSmoother->SetRelativeScale3D(FVector(CurrentZoom));
	}
}

void AGCInspectionActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	PreviewLight->SetRelativeLocation(LightLocation);
	PreviewLight->SetRelativeRotation(UKismetMathLibrary::FindLookAtRotation(LightLocation, FVector::ZeroVector));
	CaptureCamera->SetRelativeLocation(FVector(-CameraDistance, 0.0f, 0.0f));
	CaptureCamera->ShowOnlyActors.AddUnique(this);
	CaptureCamera->ShowOnlyComponent(PreviewMesh);
	CaptureCamera->TextureTarget = RenderTarget;

	if (const UGCSettings* Settings = UGCSettings::Get())
	{
		PreviewMesh->SetLightingChannels(
			Settings->InventoryLightingChannel.bChannel0,
			Settings->InventoryLightingChannel.bChannel1,
			Settings->InventoryLightingChannel.bChannel2);
		
		PreviewLight->SetLightingChannels(
			Settings->InventoryLightingChannel.bChannel0,
			Settings->InventoryLightingChannel.bChannel1,
			Settings->InventoryLightingChannel.bChannel2);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AGCInspectionActor::InputBinding_Turn(const FInputActionValue& InValue)
{
	const FVector2D Value = InValue.Get<FVector2D>();
	if (CaptureCamera->bCaptureEveryFrame && Value.Size() != 0.0f)
	{
		MeshSmoother->AddWorldRotation(FRotator(Value.Y, Value.X, 0.0f));
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AGCInspectionActor::InputBinding_Zoom(const FInputActionValue& InValue)
{
	const float Value = InValue.Get<float>();
	if (CaptureCamera->bCaptureEveryFrame && Value != 0.0f)
	{
		TargetZoom = FMath::Clamp(TargetZoom + (Value * 0.1f), ZoomRange.X, ZoomRange.Y);
	}
}

void AGCInspectionActor::SetPreviewItemID(const FName& InItemID)
{
	if (ItemID != InItemID)
	{
		ItemID = InItemID;
		ZoomRange = FVector2D(0.25f, 2.0f);
		FRCStaticMeshProperties Data = DefaultProperties;
		const FGCInvItemData ItemData = UGCInventoryManager::GetItemData(ItemID);
		if (ItemData.IsValidData())
		{
			Data.bCastShadows = ItemData.bPreviewCastShadows;
			Data.Mesh = ItemData.PreviewMesh.LoadSynchronous();
			Data.Materials.Empty(ItemData.PreviewMaterials.Num());
			for (TSoftObjectPtr<UMaterialInterface> Material : ItemData.PreviewMaterials)
			{
				Data.Materials.Add(Material.LoadSynchronous());
			}

			PreviewMesh->SetRelativeTransform(ItemData.PreviewTransform);
			ZoomRange = ItemData.PreviewZoomRange;
		}

		PreviewMesh->SetStaticMesh(Data.Mesh);
		PreviewMesh->SetMaterial(0, !Data.Materials.IsEmpty() ? Data.Materials[0] : nullptr);
		PreviewMesh->SetCastShadow(Data.bCastShadows);

		TargetZoom = 1.0f;
		MeshSmoother->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

void AGCInspectionActor::SetInspectionPaused(const bool bPaused) const
{
	if (IsActorTickEnabled())
	{
		CaptureCamera->bCaptureEveryFrame = !bPaused;
	}
}

void AGCInspectionActor::BeginInspection()
{
	CaptureCamera->bCaptureEveryFrame = true;
	EnableInput(GetWorld()->GetFirstPlayerController());
	SetActorTickEnabled(true);
}

void AGCInspectionActor::EndInspection()
{
	CaptureCamera->bCaptureEveryFrame = false;
	DisableInput(GetWorld()->GetFirstPlayerController());
	SetActorTickEnabled(false);
}

void AGCInspectionActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorEnableCollision(false);
	PreviewMesh->SetVisibleInSceneCaptureOnly(true);
	CaptureCamera->bCaptureEveryFrame = false;
	
	CreateInputComponent(UInputSettings::GetDefaultInputComponentClass());
	if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		InputComp->BindAction(TurnInput, ETriggerEvent::Triggered, this, &AGCInspectionActor::InputBinding_Turn);
		InputComp->BindAction(ZoomInput, ETriggerEvent::Triggered, this, &AGCInspectionActor::InputBinding_Zoom);
	}
}
