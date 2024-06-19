﻿// Copyright (C) RedCraft86. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "InventoryPreview.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/InputSettings.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItemData.h"
#include "Data/PrimitiveData.h"

AInventoryPreview::AInventoryPreview()
	: TurnSpeed(5.0f), ZoomSpeed(5.0f), TurnInput(nullptr), ZoomInput(nullptr)
	, ItemKey({}), ZoomRange(FVector2D::UnitVector), ZoomValue({1.0f}), Inventory(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
#if WITH_EDITOR
	SceneRoot->bVisualizeComponent = true;
#endif

	PreviewRoot = CreateDefaultSubobject<USpringArmComponent>("PreviewRoot");
	PreviewRoot->SetupAttachment(SceneRoot);
	PreviewRoot->bEnableCameraRotationLag = true;
	PreviewRoot->bDoCollisionTest = false;
	PreviewRoot->TargetArmLength = 0.0f;

	PreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>("PreviewMesh");
	PreviewMesh->SetupAttachment(PreviewRoot, USpringArmComponent::SocketName);
	PreviewMesh->SetLightingChannels(false, true, false);

	PreviewLight = CreateDefaultSubobject<USpotLightComponent>("PreviewLight");
	PreviewLight->SetRelativeLocation(FVector(-150.0f, 100.0f, 80.0f));
	PreviewLight->SetRelativeRotation(FRotator(-35.0f, 30.0f, 0.0f));
	PreviewLight->SetLightingChannels(false, true, false);
	PreviewLight->SetupAttachment(SceneRoot);
	
	PreviewCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("PreviewCapture");
	PreviewCapture->SetRelativeLocation(FVector(-150.0f, 0.0f, 0.0f));
	PreviewCapture->SetupAttachment(SceneRoot);
	
	PreviewCapture->FOVAngle = 60.0f;
	PreviewCapture->bCaptureEveryFrame = false;
	PreviewCapture->bAlwaysPersistRenderingState = true;
	PreviewCapture->ShowFlags.SetAntiAliasing(true);
	PreviewCapture->ShowFlags.SetAtmosphere(false);
	PreviewCapture->ShowFlags.SetBSP(false);
	PreviewCapture->ShowFlags.SetCloud(false);
	PreviewCapture->ShowFlags.SetDecals(false);
	PreviewCapture->ShowFlags.SetFog(false);
	PreviewCapture->ShowFlags.SetLandscape(false);
	PreviewCapture->ShowFlags.SetParticles(false);
	PreviewCapture->ShowFlags.SetSkeletalMeshes(false);
	PreviewCapture->ShowFlags.SetStaticMeshes(true);
	PreviewCapture->ShowFlags.SetTranslucency(true);
	PreviewCapture->ShowFlags.SetDeferredLighting(true);
	PreviewCapture->ShowFlags.SetInstancedFoliage(false);
	PreviewCapture->ShowFlags.SetInstancedGrass(false);
	PreviewCapture->ShowFlags.SetInstancedStaticMeshes(false);
	PreviewCapture->ShowFlags.SetNaniteMeshes(true);
	PreviewCapture->ShowFlags.SetPaper2DSprites(false);
	PreviewCapture->ShowFlags.SetTextRender(false);
	PreviewCapture->ShowFlags.SetTemporalAA(true);
	PreviewCapture->ShowFlags.SetBloom(true);
	PreviewCapture->ShowFlags.SetEyeAdaptation(true);
	PreviewCapture->ShowFlags.SetLocalExposure(true);
	PreviewCapture->ShowFlags.SetMotionBlur(false);
	PreviewCapture->ShowFlags.SetToneCurve(true);
	PreviewCapture->ShowFlags.SetSkyLighting(true);
	PreviewCapture->ShowFlags.SetAmbientOcclusion(false);
	PreviewCapture->ShowFlags.SetDynamicShadows(true);
	PreviewCapture->ShowFlags.SetAmbientCubemap(true);
	PreviewCapture->ShowFlags.SetDistanceFieldAO(true);
	PreviewCapture->ShowFlags.SetLightFunctions(false);
	PreviewCapture->ShowFlags.SetLightShafts(false);
	PreviewCapture->ShowFlags.SetReflectionEnvironment(true);
	PreviewCapture->ShowFlags.SetScreenSpaceReflections(true);
	PreviewCapture->ShowFlags.SetTexturedLightProfiles(false);
	PreviewCapture->ShowFlags.SetVolumetricFog(false);
	PreviewCapture->ShowFlags.SetGame(true);
	PreviewCapture->ShowFlags.SetLighting(true);
	PreviewCapture->ShowFlags.SetPathTracing(false);
	PreviewCapture->ShowFlags.SetPostProcessing(true);
}

void AInventoryPreview::SetItem(const FGuid& InItemKey)
{
	if (InItemKey.IsValid() && Inventory->ItemSlots.Contains(InItemKey))
	{
		ItemKey = InItemKey;
		const FInventorySlotData& SlotData = Inventory->ItemSlots.FindRef(InItemKey);
		const UInventoryItemData* ItemData = SlotData.GetItemData<UInventoryItemData>();
		const FTransformMeshData& MeshData = ItemData->GetMeshData(SlotData.Metadata);
		UPrimitiveDataLibrary::SetStaticMeshProperties(PreviewMesh, MeshData);
		PreviewMesh->SetRelativeTransform(MeshData.Transform);
		PreviewMesh->SetHiddenInGame(false);
		
		ZoomRange = ItemData->PreviewZoomRange;
		ZoomValue.TargetValue = (ZoomRange.X + ZoomRange.Y) * 0.5f;
		ZoomValue.SnapToTarget();
		
		PreviewRoot->bEnableCameraRotationLag = false;
		PreviewRoot->SetRelativeScale3D(FVector::OneVector);
		PreviewRoot->SetRelativeRotation(FRotator::ZeroRotator);
		PreviewRoot->bEnableCameraRotationLag = true;
	}
	else
	{
		PreviewMesh->SetHiddenInGame(true);
	}
}

void AInventoryPreview::Initialize()
{
	ZoomValue.InterpSpeed = ZoomSpeed;
	EnableInput(GetWorld()->GetFirstPlayerController());
	PreviewRoot->CameraRotationLagSpeed = TurnSpeed;
	PreviewRoot->bEnableCameraRotationLag = true;
	SetActorTickEnabled(true);
}

void AInventoryPreview::Deinitialize()
{
	ItemKey = {};
	ZoomRange = FVector2D::UnitVector;
	DisableInput(GetWorld()->GetFirstPlayerController());
	PreviewRoot->bEnableCameraRotationLag = false;
	PreviewRoot->SetRelativeRotation(FRotator::ZeroRotator);
	PreviewMesh->SetRelativeTransform(FTransform::Identity);
	SetActorTickEnabled(false);
}

void AInventoryPreview::InputBinding_Turn(const FInputActionValue& InValue)
{
	const FVector2D Value = InValue.Get<FVector2D>();
	if (PreviewCapture->IsComponentTickEnabled() && !FMath::IsNearlyZero(Value.Size()))
	{
		PreviewRoot->AddWorldRotation(FRotator(Value.Y, Value.X, 0.0f));
	}
}

void AInventoryPreview::InputBinding_Zoom(const FInputActionValue& InValue)
{
	const float Value = InValue.Get<float>();
	if (PreviewCapture->IsComponentTickEnabled() && !FMath::IsNearlyZero(Value))
	{
		ZoomValue.TargetValue = FMath::Clamp(ZoomValue.TargetValue + Value * 0.1f, ZoomRange.X, ZoomRange.Y);
	}
}

void AInventoryPreview::BeginPlay()
{
	Super::BeginPlay();
	PreviewMesh->SetOnlyOwnerSee(true);
	PreviewMesh->SetVisibleInSceneCaptureOnly(true);
	SetActorEnableCollision(false);
	Deinitialize();

	CreateInputComponent(UInputSettings::GetDefaultInputComponentClass());
	if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		InputComp->BindAction(TurnInput, ETriggerEvent::Triggered, this, &AInventoryPreview::InputBinding_Turn);
		InputComp->BindAction(ZoomInput, ETriggerEvent::Triggered, this, &AInventoryPreview::InputBinding_Zoom);
	}
}

void AInventoryPreview::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (ItemKey.IsValid())
	{
		if (!ZoomValue.IsComplete())
		{
			ZoomValue.Tick(DeltaSeconds);
			PreviewRoot->SetRelativeScale3D(FVector(ZoomValue.CurrentValue));
		}
		PreviewCapture->CaptureScene();
	}
}

void AInventoryPreview::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	PreviewLight->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(
		PreviewLight->GetComponentLocation(), PreviewRoot->GetComponentLocation()));
	
	PreviewCapture->ShowOnlyComponent(PreviewMesh);
	PreviewCapture->ShowOnlyActors.AddUnique(this);
}
