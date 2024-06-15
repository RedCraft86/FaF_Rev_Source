// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRDoorActor.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Inventory/InventoryComponent.h"
#include "FRGameMode.h"
#include "FRPlayer.h"
#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#include "Components/DebugShapesComponent.h"
#endif

#define PlayHigh(Sound) AudioHigh->SetSound(Sound); AudioHigh->Play();

AFRDoorBase::AFRDoorBase() : bMultidirectional(false), OpenRotation(-100.0f), BoxScale(2.0f, 1.25f, 1.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	DoorPivot = CreateDefaultSubobject<USceneComponent>("DoorPivot");
	DoorPivot->SetupAttachment(SceneRoot);
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh");
	DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DoorMesh->SetCollisionObjectType(ECC_WorldDynamic);
	DoorMesh->SetCollisionResponseToAllChannels(ECR_Block);
	DoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	DoorMesh->SetupAttachment(DoorPivot);
	
	DoorBox = CreateDefaultSubobject<UBoxComponent>("DoorBox");
	DoorBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DoorBox->SetCollisionObjectType(ECC_WorldDynamic);
	DoorBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	DoorBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	DoorBox->SetupAttachment(DoorPivot);

	AudioHigh = CreateDefaultSubobject<UAudioComponent>("AudioHigh");
	AudioHigh->SetupAttachment(DoorBox);
	AudioHigh->bCanPlayMultipleInstances = true;
	AudioHigh->bAutoActivate = false;
	AudioHigh->VolumeMultiplier = 0.5f;
	AudioHigh->bOverrideAttenuation = true;
	AudioHigh->AttenuationOverrides.AttenuationShapeExtents.X = 250.0f;
	AudioHigh->AttenuationOverrides.FalloffDistance = 500.0f;

	AudioLow = CreateDefaultSubobject<UAudioComponent>("AudioLow");
	AudioLow->SetupAttachment(DoorBox);
	AudioLow->bCanPlayMultipleInstances = true;
	AudioLow->bAutoActivate = false;
	AudioLow->VolumeMultiplier = 0.2f;
	AudioLow->bOverrideAttenuation = true;
	AudioLow->AttenuationOverrides.AttenuationShapeExtents.X = 200.0f;
	AudioLow->AttenuationOverrides.FalloffDistance = 400.0f;

	AnimationCurve.GetRichCurve()->SetKeyTangentMode(AnimationCurve.GetRichCurve()
		->UpdateOrAddKey(0.0f, 0.0f), RCTM_Auto);
	
	AnimationCurve.GetRichCurve()->SetKeyTangentMode(AnimationCurve.GetRichCurve()
		->UpdateOrAddKey(0.5f, 0.0f), RCTM_Auto);

	LockedInfo.Label = FText::FromString(TEXT("Locked") LINE_TERMINATOR TEXT("[16pt,&regular]([%key Key])"));
	
#if WITH_EDITORONLY_DATA
	bRunConstructionScriptOnDrag = true;
	
	ForwardArrow = CreateEditorOnlyDefaultSubobject<UArrowComponent>("ForwardArrow");
	if (ForwardArrow) ForwardArrow->bIsEditorOnly = true; ForwardArrow->SetupAttachment(DoorBox);
	
	ShapeVisualizer = CreateEditorOnlyDefaultSubobject<UDebugShapesComponent>("ShapeVisualizer");
	if (ShapeVisualizer) ShapeVisualizer->bIsEditorOnly = true;
#endif
}

void AFRDoorBase::SetState(const bool bInState)
{
	if (bState != bInState)
	{
		bState = bInState;
		AudioLow->Play();
		PlayHigh(bState ? OpenSound : CloseSound)
		DoorMesh->SetCollisionEnabled(bState ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
		if (bState) DoorRotaton = CalcOpenRotation();
		PlayAnim(!bState);
	}
}

float AFRDoorBase::CalcOpenRotation() const
{
	if (!bMultidirectional || !Interactor)
		return OpenRotation;
	
	const float AbsRotation = FMath::Abs(OpenRotation);
	const bool bFrontInteract = FVector::DotProduct(Interactor->GetActorForwardVector(), GetActorForwardVector()) < 0.0f;
	return bFrontInteract ? AbsRotation : -AbsRotation;
}

void AFRDoorBase::PlayAnim(const bool bReverse)
{
	if (AnimationCurve.HasAnyData())
	{
		float MinTime, MaxTime;
		AnimationCurve.GetTimeRange(MinTime, MaxTime);
		AnimSpeed = 1.0f / (MaxTime - MinTime);
		AnimValues.Y = bReverse ? 0.0f : 1.0f;
		SetActorTickEnabled(true);
	}
}

void AFRDoorBase::ApplyRotation(const float Alpha) const
{
	DoorPivot->SetRelativeRotation({0.0f, FMath::Lerp(0.0f, DoorRotaton, Alpha), 0.0f});
}

bool AFRDoorBase::CheckKey(const AFRPlayerBase* Player)
{
	if (!IsLocked()) return true;
	if (Player->GetGameMode()->Inventory->UseKeyItem(KeyItem, KeyID))
	{
		KeyID = TEXT("");
		KeyItem = nullptr;
		DoorMesh->SetCanEverAffectNavigation(false);
		return true;
	}

	AudioHigh->Stop();
	PlayHigh(LockedSound)
	return false;
}

void AFRDoorBase::OnAnimFinished() const
{
	AudioLow->FadeOut(0.2f, 0.0f);
	AudioHigh->FadeOut(0.2f, 0.0f);
}

void AFRDoorBase::OnAnimTick(const float Alpha) const
{
	float MinTime, MaxTime;
	AnimationCurve.GetTimeRange(MinTime, MaxTime);
	ApplyRotation(AnimationCurve.GetValue(FMath::GetMappedRangeValueClamped(
		FVector2D::UnitY(), {MinTime, MaxTime}, Alpha)));
}

bool AFRDoorBase::GetInteractionInfo_Implementation(FInteractionInfo& Info)
{
	if (IsLocked())
	{
		Info = LockedInfo;
		Info.Label = FText::FromString(LockedInfo.Label.ToString().Replace(TEXT("%key"), *KeyID));
	}
	else
	{
		Info.Label = bState ? INVTEXT("Close") : INVTEXT("Open");
	}
	return true;
}

void AFRDoorBase::OnBeginInteract_Implementation(AFRPlayerBase* Player, const FHitResult& HitResult)
{
	if (!GetWorldTimerManager().IsTimerActive(PawnCooldown) && CheckKey(Player))
	{
		Interactor = Player;
		SetState(!bState);
	}
}

void AFRDoorBase::OnBeginPawnInteract_Implementation(APawn* Pawn, const FHitResult& HitResult)
{
	if (IsLocked()) return;
	if (!GetWorldTimerManager().IsTimerActive(PawnCooldown))
	{
		Interactor = Pawn;
		SetState(true);
		GetWorldTimerManager().SetTimer(PawnCooldown, [WEAK_THIS]()
		{
			if (WeakThis.IsValid()) WeakThis->PawnCooldown.Invalidate();
		}, 0.2f, false);
	}
}

void AFRDoorBase::BeginPlay()
{
	Super::BeginPlay();
	AudioLow->SetSound(CreakSound);
	DoorPivot->SetRelativeRotation(FRotator::ZeroRotator);
}

void AFRDoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FMath::IsNearlyEqual(AnimValues.X, AnimValues.Y))
	{
		bAnimFinished = false;
		AnimValues.X = FMath::FInterpConstantTo(AnimValues.X, AnimValues.Y, DeltaTime, AnimSpeed);
		OnAnimTick(AnimValues.X);
	}
	else if (!bAnimFinished)
	{
		bAnimFinished = true;
		SetActorTickEnabled(PrimaryActorTick.bStartWithTickEnabled);
		OnAnimFinished();
	}
}

void AFRDoorBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	DoorProperties.FillMaterials();
	UPrimitiveDataLibrary::SetStaticMeshProperties(DoorMesh, *DoorProperties);
	DoorMesh->SetRelativeTransform(DoorProperties.Transform);
	if (KeyItem && !KeyID.IsEmpty())
		DoorMesh->SetCanEverAffectNavigation(true);

	DoorBox->SetRelativeScale3D(BoxScale);
#if WITH_EDITORONLY_DATA
	if (FMath::IsNearlyZero(PreviewAlpha))
#endif
	{
		DoorBox->SetBoxExtent(DoorMesh->Bounds.BoxExtent);
		DoorBox->SetWorldLocation(DoorMesh->Bounds.Origin);
	}

#if WITH_EDITORONLY_DATA
	if (ForwardArrow) ForwardArrow->SetWorldScale3D(FVector::OneVector);
	if (ShapeVisualizer)
	{
		const float Radius = 90.0f * DoorMesh->Bounds.BoxExtent.GetMax() / 101.0f;
		if (bDebugView && (bMultidirectional || OpenRotation < 0.0f))
		{
			FDebugArcData& N = ShapeVisualizer->DebugArcs.FindOrAdd("N");
			N.Rotation = {0.0f, 90.0f, 0.0f};
			N.bRenderOnTopOfEverything = true;
			N.MaxAngle = 0.0f;
			N.Radius = Radius;
			N.MinAngle = -FMath::Abs(OpenRotation);
		}
		else
		{
			ShapeVisualizer->DebugArcs.Remove("N");
		}

		if (bDebugView && (bMultidirectional || OpenRotation > 0.0f))
		{
			FDebugArcData& P = ShapeVisualizer->DebugArcs.FindOrAdd("P");
			P.Rotation = {0.0f, 90.0f, 0.0f};
			P.bRenderOnTopOfEverything = true;
			P.MinAngle = 0.0f;
			P.Radius = Radius;
			P.MaxAngle = FMath::Abs(OpenRotation);
		}
		else
		{
			ShapeVisualizer->DebugArcs.Remove("P");
		}
		
		DoorPivot->SetRelativeRotation({0.0f, (bMultidirectional ? PreviewAlpha : FMath::Abs(PreviewAlpha))
			* (bMultidirectional ? FMath::Abs(OpenRotation) : OpenRotation), 0.0f});
	}
#endif
}

