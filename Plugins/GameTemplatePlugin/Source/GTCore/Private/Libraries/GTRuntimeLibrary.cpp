// Copyright (C) RedCraft86. All Rights Reserved.

#include "Libraries/GTRuntimeLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Helpers/WindowsHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"

EWindowsDialogueResult UGTRuntimeLibrary::OpenWindowsDialogue(const FString Title, const FString Message,
	const EWindowsDialogueType Type, const EWindowsDialogueIcon Icon)
{
	return WindowsHelpers::OpenDialogue(Title, Message, Type, Icon);
}

void UGTRuntimeLibrary::TriggerVirtualKey(const FKey InKey, const EVirtualKeyTriggerType TriggerType, const bool bRepeat)
{
	WindowsHelpers::VirtualKey(InKey, TriggerType, bRepeat);
}

TArray<FVector> UGTRuntimeLibrary::GetBoundingBoxVertices(const AActor* Target, const bool bOnlyCollidingComponents,
	const bool bIncludeFromChildActors, FVector& Origin, FVector& BoxExtent)
{
	if (!IsValid(Target)) return {};
	
	Target->GetActorBounds(bOnlyCollidingComponents, Origin, BoxExtent, bIncludeFromChildActors);
	
	TArray<FVector> Result;
	Result.AddUnique(Origin + BoxExtent);
	Result.AddUnique(Origin - BoxExtent);
	Result.AddUnique(FVector(Origin.X - BoxExtent.X, Origin.Y + BoxExtent.Y, Origin.Z + BoxExtent.Z));
	Result.AddUnique(FVector(Origin.X + BoxExtent.X, Origin.Y - BoxExtent.Y, Origin.Z + BoxExtent.Z));
	Result.AddUnique(FVector(Origin.X + BoxExtent.X, Origin.Y + BoxExtent.Y, Origin.Z - BoxExtent.Z));
	Result.AddUnique(FVector(Origin.X - BoxExtent.X, Origin.Y - BoxExtent.Y, Origin.Z + BoxExtent.Z));
	Result.AddUnique(FVector(Origin.X + BoxExtent.X, Origin.Y - BoxExtent.Y, Origin.Z - BoxExtent.Z));
	Result.AddUnique(FVector(Origin.X - BoxExtent.X, Origin.Y + BoxExtent.Y, Origin.Z - BoxExtent.Z));

	return Result;
}

bool UGTRuntimeLibrary::IsActorInScreen(const AActor* Target, const float MaxDistance, const bool bOriginOnly,
	const bool bLineTraceCheck, const FActorBoundsCheckParams& TraceCheckParams)
{
		if (!IsValid(Target) || MaxDistance <= 0
		|| (bLineTraceCheck && TraceCheckParams.BoundingBoxLerp.Size() <= 0))
	{
		return false;
	}

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(Target, 0);
	if (!CameraManager) return false;

	if (FVector::Distance(CameraManager->GetCameraLocation(), Target->GetActorLocation()) > MaxDistance)
	{
		return false;
	}

	APlayerController* Controller = UGameplayStatics::GetPlayerController(Target, 0);
	if (!Controller) return false;

	TArray<FVector> TestVectors;
	TestVectors.Add(Target->GetActorLocation());

	if (!bOriginOnly)
	{
		FVector Origin, BoxExtent = FVector::ZeroVector;
		TArray<FVector> BoundingBoxVertices = GetBoundingBoxVertices(Target, TraceCheckParams.bOnlyCollidingComponents,
			TraceCheckParams.bIncludeFromChildActors, Origin, BoxExtent);

		TestVectors.AddUnique(Origin);
		for (const FVector Vector : BoundingBoxVertices)
		{
			TestVectors.AddUnique(FVector(
				FMath::Lerp(Origin.X, Vector.X, TraceCheckParams.BoundingBoxLerp.X),
				FMath::Lerp(Origin.Y, Vector.Y, TraceCheckParams.BoundingBoxLerp.Y),
				FMath::Lerp(Origin.Z, Vector.Z, TraceCheckParams.BoundingBoxLerp.Z)
			));
		}
	}

	bool bPassedScreenTest = false;
	for (int i = 0; i < TestVectors.Num(); i++)
	{
		FVector2D ScreenLoc;
		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(Controller);
		
		if (!Controller->ProjectWorldLocationToScreen(TestVectors[i], ScreenLoc)) continue;
		if (ScreenLoc.X >= 0 && ScreenLoc.X <= ViewportSize.X && ScreenLoc.Y >= 0 && ScreenLoc.Y <= ViewportSize.Y)
		{
			bPassedScreenTest = true;
			break;
		}
	}

	if (!bPassedScreenTest) return false;
	if (!bLineTraceCheck) return true;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Target);
	QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(Target, 0));

	FHitResult HitResult;
	for (int i = 0; i < TestVectors.Num(); i++)
	{
		if (!Target->GetWorld()->LineTraceSingleByChannel(HitResult, CameraManager->GetCameraLocation(),
			TestVectors[i], TraceCheckParams.LineTraceChannel, QueryParams))
			return true;
	}

	return false;
}

bool UGTRuntimeLibrary::IsLocationInFront(const AActor* Target, const FVector Location)
{
	if (!IsValid(Target)) return false;
	FVector DotVec = Location - Target->GetActorLocation(); DotVec.Normalize();
	return FVector::DotProduct(Target->GetActorForwardVector(), DotVec) > 0.0f;
}

bool UGTRuntimeLibrary::IsActorInFront(const AActor* Target, const AActor* ActorToTest)
{
	if (!IsValid(Target) || !IsValid(ActorToTest)) return false;
	return IsLocationInFront(Target, ActorToTest->GetActorLocation());
}

UWorld* UGTRuntimeLibrary::GetPlayWorld()
{
	return GEngine->GetCurrentPlayWorld();
}

void UGTRuntimeLibrary::RestartLevel(const UObject* ContextObject, const FString Options)
{
	UGameplayStatics::OpenLevel(ContextObject, *UGameplayStatics::GetCurrentLevelName(ContextObject), true, Options);
}

void UGTRuntimeLibrary::CallLocalEvent(UObject* Target, const FName EventName)
{
	if (!IsValid(Target) || EventName.IsNone()) return;
	
	FOutputDeviceNull Ar;
	Target->CallFunctionByNameWithArguments(*EventName.ToString(),
		Ar, nullptr, true);
}

bool UGTRuntimeLibrary::IsInEditor()
{
#if WITH_EDITOR
	return !FApp::IsGame();
#else
	return false;
#endif
}
