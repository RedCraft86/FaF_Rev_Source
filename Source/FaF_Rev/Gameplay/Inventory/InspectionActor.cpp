// Copyright (C) RedCraft86. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "InspectionActor.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/InputSettings.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItemData.h"
#include "Data/PrimitiveData.h"

AInspectionActor::AInspectionActor()
	: TurnSpeed(5.0f), ZoomSpeed(5.0f), TurnInput(nullptr), ZoomInput(nullptr)
	, ItemKey({}), ZoomRange(FVector2D::UnitVector), ZoomValue({1.0f}), Inventory(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
#if WITH_EDITOR
	SceneRoot->bVisualizeComponent = true;
#endif

	InspectionRoot = CreateDefaultSubobject<USpringArmComponent>("InspectionRoot");
	InspectionRoot->SetupAttachment(SceneRoot);
	InspectionRoot->bEnableCameraRotationLag = true;
	InspectionRoot->bDoCollisionTest = false;
	InspectionRoot->TargetArmLength = 0.0f;

	InspectionMesh = CreateDefaultSubobject<UStaticMeshComponent>("InspectionMesh");
	InspectionMesh->SetupAttachment(InspectionRoot, USpringArmComponent::SocketName);
	InspectionMesh->SetLightingChannels(false, true, false);
	InspectionMesh->SetVisibleInSceneCaptureOnly(true);

	InspectionLight = CreateDefaultSubobject<USpotLightComponent>("InspectionLight");
	InspectionLight->SetRelativeLocation(FVector(-150.0f, 100.0f, 80.0f));
	InspectionLight->SetRelativeRotation(FRotator(-35.0f, 30.0f, 0.0f));
	InspectionLight->SetLightingChannels(false, true, false);
	InspectionLight->SetupAttachment(SceneRoot);
	
	InspectionCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("InspectionCapture");
	InspectionCapture->SetRelativeLocation(FVector(-150.0f, 0.0f, 0.0f));
	InspectionCapture->SetupAttachment(SceneRoot);
	
	InspectionCapture->FOVAngle = 60.0f;
	InspectionCapture->bAlwaysPersistRenderingState = true;
	InspectionCapture->ShowFlags.SetAntiAliasing(true);
	InspectionCapture->ShowFlags.SetAtmosphere(false);
	InspectionCapture->ShowFlags.SetBSP(false);
	InspectionCapture->ShowFlags.SetCloud(false);
	InspectionCapture->ShowFlags.SetDecals(false);
	InspectionCapture->ShowFlags.SetFog(false);
	InspectionCapture->ShowFlags.SetLandscape(false);
	InspectionCapture->ShowFlags.SetParticles(false);
	InspectionCapture->ShowFlags.SetSkeletalMeshes(false);
	InspectionCapture->ShowFlags.SetStaticMeshes(true);
	InspectionCapture->ShowFlags.SetTranslucency(true);
	InspectionCapture->ShowFlags.SetDeferredLighting(true);
	InspectionCapture->ShowFlags.SetInstancedFoliage(false);
	InspectionCapture->ShowFlags.SetInstancedGrass(false);
	InspectionCapture->ShowFlags.SetInstancedStaticMeshes(false);
	InspectionCapture->ShowFlags.SetNaniteMeshes(true);
	InspectionCapture->ShowFlags.SetPaper2DSprites(false);
	InspectionCapture->ShowFlags.SetTextRender(false);
	InspectionCapture->ShowFlags.SetTemporalAA(true);
	InspectionCapture->ShowFlags.SetBloom(true);
	InspectionCapture->ShowFlags.SetEyeAdaptation(true);
	InspectionCapture->ShowFlags.SetLocalExposure(true);
	InspectionCapture->ShowFlags.SetMotionBlur(false);
	InspectionCapture->ShowFlags.SetToneCurve(true);
	InspectionCapture->ShowFlags.SetSkyLighting(true);
	InspectionCapture->ShowFlags.SetAmbientOcclusion(false);
	InspectionCapture->ShowFlags.SetDynamicShadows(true);
	InspectionCapture->ShowFlags.SetAmbientCubemap(true);
	InspectionCapture->ShowFlags.SetDistanceFieldAO(true);
	InspectionCapture->ShowFlags.SetLightFunctions(false);
	InspectionCapture->ShowFlags.SetLightShafts(false);
	InspectionCapture->ShowFlags.SetReflectionEnvironment(true);
	InspectionCapture->ShowFlags.SetScreenSpaceReflections(true);
	InspectionCapture->ShowFlags.SetTexturedLightProfiles(false);
	InspectionCapture->ShowFlags.SetVolumetricFog(false);
	InspectionCapture->ShowFlags.SetGame(true);
	InspectionCapture->ShowFlags.SetLighting(true);
	InspectionCapture->ShowFlags.SetPathTracing(false);
	InspectionCapture->ShowFlags.SetPostProcessing(true);
}

void AInspectionActor::SetItem(const FGuid& InItemKey)
{
	if (InItemKey.IsValid() && Inventory->ItemSlots.Contains(InItemKey))
	{
		ItemKey = InItemKey;
		const UInventoryItemData* ItemData = Inventory->ItemSlots.FindRef(InItemKey).GetItemData<UInventoryItemData>();
		UPrimitiveDataLibrary::SetStaticMeshProperties(InspectionMesh, ItemData->InspectionData);
		InspectionMesh->SetRelativeTransform(ItemData->InspectionTransform);
		InspectionMesh->SetHiddenInGame(false);
		
		ZoomRange = ItemData->InspectionZoomRange;
		ZoomValue.TargetValue = (ZoomRange.X + ZoomRange.Y) * 0.5f;
		ZoomValue.SnapToTarget();
		
		InspectionRoot->bEnableCameraRotationLag = false;
		InspectionRoot->SetRelativeScale3D(FVector::OneVector);
		InspectionRoot->SetRelativeRotation(FRotator::ZeroRotator);
		InspectionRoot->bEnableCameraRotationLag = true;
	}
	else
	{
		InspectionMesh->SetHiddenInGame(true);
	}
}

void AInspectionActor::Initialize()
{
	ZoomValue.InterpSpeed = ZoomSpeed;
	EnableInput(GetWorld()->GetFirstPlayerController());
	InspectionRoot->CameraRotationLagSpeed = TurnSpeed;
	InspectionRoot->bEnableCameraRotationLag = true;
	InspectionCapture->SetComponentTickEnabled(true);
	SetActorTickEnabled(true);
}

void AInspectionActor::Deinitialize()
{
	ItemKey = {};
	ZoomRange = FVector2D::UnitVector;
	DisableInput(GetWorld()->GetFirstPlayerController());
	InspectionCapture->SetComponentTickEnabled(false);
	InspectionRoot->bEnableCameraRotationLag = false;
	InspectionRoot->SetRelativeRotation(FRotator::ZeroRotator);
	InspectionMesh->SetRelativeTransform(FTransform::Identity);
	SetActorTickEnabled(false);
}

void AInspectionActor::InputBinding_Turn(const FInputActionValue& InValue)
{
	const FVector2D Value = InValue.Get<FVector2D>();
	if (InspectionCapture->IsComponentTickEnabled() && !FMath::IsNearlyZero(Value.Size()))
	{
		InspectionRoot->AddWorldRotation(FRotator(Value.Y, Value.X, 0.0f));
	}
}

void AInspectionActor::InputBinding_Zoom(const FInputActionValue& InValue)
{
	const float Value = InValue.Get<float>();
	if (InspectionCapture->IsComponentTickEnabled() && !FMath::IsNearlyZero(Value))
	{
		ZoomValue.TargetValue = FMath::Clamp(ZoomValue.TargetValue + Value * 0.1f, ZoomRange.X, ZoomRange.Y);
	}
}

void AInspectionActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorEnableCollision(false);
	Deinitialize();

	CreateInputComponent(UInputSettings::GetDefaultInputComponentClass());
	if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		InputComp->BindAction(TurnInput, ETriggerEvent::Triggered, this, &AInspectionActor::InputBinding_Turn);
		InputComp->BindAction(ZoomInput, ETriggerEvent::Triggered, this, &AInspectionActor::InputBinding_Zoom);
	}
}

void AInspectionActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ZoomValue.Tick(DeltaSeconds);
	InspectionRoot->SetRelativeScale3D(FVector(ZoomValue.CurrentValue));
}

void AInspectionActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	InspectionLight->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(
		InspectionLight->GetComponentLocation(), InspectionRoot->GetComponentLocation()));
	
	InspectionCapture->ShowOnlyComponent(InspectionMesh);
	InspectionCapture->ShowOnlyActors.AddUnique(this);
}
