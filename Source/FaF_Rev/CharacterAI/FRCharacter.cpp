// Copyright (C) RedCraft86. All Rights Reserved.

#include "FRCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Libraries/GTRuntimeLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "FRPlayer.h"

AFRCharacter::AFRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	FootstepAudio = CreateDefaultSubobject<UAudioComponent>("FootstepAudio");
	FootstepAudio->SetRelativeLocation({0.0f, 0.0f, -50.0f});
	FootstepAudio->SetupAttachment(GetCapsuleComponent());
	FootstepAudio->bAutoActivate = false;
	FootstepAudio->bEnableLowPassFilter = true;
	FootstepAudio->bOverrideAttenuation = true;
	FootstepAudio->AttenuationOverrides.bAttenuate = true;
	FootstepAudio->AttenuationOverrides.AttenuationShapeExtents = FVector::ZeroVector;
	FootstepAudio->AttenuationOverrides.FalloffDistance = 4500.0f;

	AudioVolumeCurve.GetRichCurve()->UpdateOrAddKey(0.0f, 1.0f);
	AudioVolumeCurve.GetRichCurve()->UpdateOrAddKey(1.0f, 0.01f);
}

void AFRCharacter::PlaySmartAudio(UAudioComponent* InComponent)
{
	if (InComponent && InComponent->Sound)
	{
	// 	InComponent->AttenuationOverrides.bAttenuate = true;
	// 	if (const APlayerCameraManager* CM = UGameplayStatics::GetPlayerCameraManager(this, 0))
	// 	{
	// 		FCollisionQueryParams TraceParams;
	// 		TraceParams.AddIgnoredActor(this);
	// 		TraceParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(this, 0));
	// 		
	// 		FHitResult HitResult;
	// 		GetWorld()->LineTraceSingleByChannel(HitResult, CM->GetCameraLocation(),
	// 			GetActorLocation(), ECC_Visibility, TraceParams);
	// 		if (!HitResult.bBlockingHit)
	// 		{
	// 			InComponent->SetVolumeMultiplier(1.0f);
	// 			InComponent->SetRelativeLocation(FVector::ZeroVector);
	// 			InComponent->LowPassFilterFrequency = MAX_FILTER_FREQUENCY;
	// 			InComponent->Play();
	// 			OnAudioPlayed.Broadcast(this, InComponent);
	// 			return;
	// 		}
	// 		
	// 		FVector Origin, BoxExtent;
	// 		TArray<FVector> Vertices = UGTRuntimeLibrary::GetBoundingBoxVertices(this,
	// 			true, false, Origin, BoxExtent);
	// 		Vertices.Add(Origin);
	// 		for (const FVector& Vert : Vertices)
	// 		{
	// 			GetWorld()->LineTraceSingleByChannel(HitResult, CM->GetCameraLocation(),
	// 				Vert, ECC_Visibility, TraceParams);
	// 			if (!HitResult.bBlockingHit)
	// 			{
	// 				InComponent->SetVolumeMultiplier(1.0f);
	// 				InComponent->SetRelativeLocation(FVector::ZeroVector);
	// 				InComponent->LowPassFilterFrequency = MAX_FILTER_FREQUENCY;
	// 				InComponent->Play();
	// 				OnAudioPlayed.Broadcast(this, InComponent);
	// 				return;
	// 			}
	// 		}
	//
	// 		const UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(this,
	// 			GetActorLocation(), CM->GetCameraLocation(), this);
	// 		const TArray<FVector> PathPoints = Path ? Path->PathPoints : TArray<FVector>();
	// 		const float PathLen = Path ? Path->GetPathLength() : 0.0f;
	// 		if (PathLen < 1000.0f || PathPoints.IsEmpty())
	// 		{
	// 			InComponent->SetVolumeMultiplier(1.0f);
	// 			InComponent->SetRelativeLocation(FVector::ZeroVector);
	// 			InComponent->LowPassFilterFrequency = MAX_FILTER_FREQUENCY;
	// 			InComponent->Play();
	// 			OnAudioPlayed.Broadcast(this, InComponent);
	// 			return;
	// 		}
	//
	// 		if (PathLen > InComponent->AttenuationOverrides.FalloffDistance)
	// 		{
	// 			InComponent->SetVolumeMultiplier(0.05f);
	// 			InComponent->SetRelativeLocation(FVector::ZeroVector);
	// 			InComponent->LowPassFilterFrequency = MAX_FILTER_FREQUENCY;
	// 			InComponent->Play();
	// 			OnAudioPlayed.Broadcast(this, InComponent);
	// 		}
	// 		else if (PathPoints.Num() > 1)
	// 		{
	// 			InComponent->AttenuationOverrides.bAttenuate = false;
	// 			
	// 			FVector2D CurveRange; AudioVolumeCurve.GetTimeRange(CurveRange.X, CurveRange.Y);
	// 			InComponent->SetVolumeMultiplier(AudioVolumeCurve.GetValue(FMath::GetMappedRangeValueClamped(
	// 				FVector2D(0.0f, InComponent->AttenuationOverrides.FalloffDistance),
	// 				CurveRange, PathLen)));
	//
	// 			InComponent->LowPassFilterFrequency = MAX_FILTER_FREQUENCY * 0.5f;
	// 			InComponent->SetWorldLocation(PathPoints[PathPoints.Num() - 2] + FVector(0.0f, 0.0f, 70.0f));
	// 			InComponent->Play();
	// 			OnAudioPlayed.Broadcast(this, InComponent);
	//
	// #if WITH_EDITORONLY_DATA
	// 			if (bDebugAudio)
	// 			{
	// #if WITH_EDITOR
	// 				UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("[%s, %s] Vol: %f | Pos: %s"),
	// 					*GetName(), *InComponent->Sound->GetName(), InComponent->VolumeMultiplier, *InComponent->GetComponentLocation().ToString()),
	// 					true, false, DebugColor, 1.0f, GetFName());
	// 				
	// 				DrawDebugSphere(GetWorld(), PathPoints[PathPoints.Num() - 1] + FVector(0.0f, 0.0f, 70.0f),
	// 					8, 16, DebugColor, false, 1.0f, 0, 1);
	// 				
	// 				for (int i = 0; i < PathPoints.Num() - 1; i++)
	// 				{
	// 				
	// 					DrawDebugLine(GetWorld(), PathPoints[i] + FVector(0.0f, 0.0f, 70.0f),
	// 						PathPoints[i + 1] + FVector(0.0f, 0.0f, 70.0f),
	// 						DebugColor, false, 1.0f, 0, 1);
	// 				
	// 					DrawDebugSphere(GetWorld(), PathPoints[i] + FVector(0.0f, 0.0f, 70.0f),
	// 						8, 16, DebugColor, false, 1.0f, 0, 1);
	// 				}
	// #endif
	// 			}
	// #endif
	// 		}
	// 	}
	// 	else
		{
			InComponent->Play();
			OnAudioPlayed.Broadcast(this, InComponent);
		}
	}
}

void AFRCharacter::DisableAI()
{
	if (GetLogicComponent()) GetLogicComponent()->Stop();
	GetCharacterMovement()->StopMovementImmediately();
}

void AFRCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (PlayerChar.IsNull()) PlayerChar = AFRPlayerBase::Get(this);
}
