// Copyright (C) RedCraft86. All Rights Reserved.

#include "Libraries/GTMathLibrary.h"
#include "Kismet/KismetMathLibrary.h"

float UGTMathLibrary::GetHorizontalDistance(const FVector A, const FVector B)
{
	return FVector::Dist2D(A, B);
}

void UGTMathLibrary::GetLineTraceVectors(const FVector& Location, const FRotator& Rotation,
	const EVectorDirection Direction, const float Distance, FVector& Start, FVector& End)
{
	FVector DirectionVector;
	switch (Direction)
	{
	case EVectorDirection::Forward:
		DirectionVector = UKismetMathLibrary::GetForwardVector(Rotation);
		break;

	case EVectorDirection::Right:
		DirectionVector = UKismetMathLibrary::GetRightVector(Rotation);
		break;

	case EVectorDirection::Up:
		DirectionVector = UKismetMathLibrary::GetUpVector(Rotation);
		break;
	};

	Start = Location;
	End = Location + (DirectionVector * Distance);
}

void UGTMathLibrary::GetActorLineTraceVectors(const AActor* Actor, const EVectorDirection Direction,
	const float Distance, FVector& Start, FVector& End)
{
	if (!IsValid(Actor)) return;

	FVector DirectionVector;
	switch (Direction)
	{
	case EVectorDirection::Forward:
		DirectionVector = Actor->GetActorForwardVector();
		break;

	case EVectorDirection::Right:
		DirectionVector = Actor->GetActorRightVector();
		break;

	case EVectorDirection::Up:
		DirectionVector = Actor->GetActorUpVector();
		break;
	};

	Start = Actor->GetActorLocation();
	End = Actor->GetActorLocation() + (DirectionVector * Distance);
}

void UGTMathLibrary::GetComponentLineTraceVectors(const USceneComponent* Component, const EVectorDirection Direction,
	const float Distance, FVector& Start, FVector& End)
{
	if (!IsValid(Component)) return;

	FVector DirectionVector;
	switch (Direction)
	{
	case EVectorDirection::Forward:
		DirectionVector = Component->GetForwardVector();
		break;

	case EVectorDirection::Right:
		DirectionVector = Component->GetRightVector();
		break;

	case EVectorDirection::Up:
		DirectionVector = Component->GetUpVector();
		break;
	};

	Start = Component->GetComponentLocation();
	End = Component->GetComponentLocation() + (DirectionVector * Distance);
}

void UGTMathLibrary::GetCameraLineTraceVectors(const UObject* ContextObject, const EVectorDirection Direction,
	const float Distance, FVector& Start, FVector& End)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull);
	const APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
	const APlayerCameraManager* PCM = PC ? PC->PlayerCameraManager : nullptr;
	if (!IsValid(PCM)) return;

	FVector DirectionVector;
	switch (Direction)
	{
	case EVectorDirection::Forward:
		DirectionVector = PCM->GetActorForwardVector();
		break;

	case EVectorDirection::Right:
		DirectionVector = PCM->GetActorRightVector();
		break;

	case EVectorDirection::Up:
		DirectionVector = PCM->GetActorUpVector();
		break;
	};

	Start = PCM->GetCameraLocation();
	End = PCM->GetCameraLocation() + (DirectionVector * Distance);
}

FRotator UGTMathLibrary::Add_RotatorRotator(const FRotator A, const FRotator B)
{
	return FRotator(FQuat(A) * FQuat(B));
}

FLinearColor UGTMathLibrary::LinearColorFromHex(const FString InHex)
{
	return FColor::FromHex(InHex);
}

FString UGTMathLibrary::LinearColorToHex(const FLinearColor InColor, const bool IsSRGB)
{
	return InColor.ToFColor(IsSRGB).ToHex();
}

FLinearColor UGTMathLibrary::LinearColorFromTemperature(const float InTemperature)
{
	return FLinearColor::MakeFromColorTemperature(InTemperature);
}

FLinearColor UGTMathLibrary::RandomLinearColor(const bool bTrueRandom, const bool bRandomAlpha)
{
	FLinearColor Result;
	if (bTrueRandom)
	{
		Result.R = FMath::FRand();
		Result.G = FMath::FRand();
		Result.B = FMath::FRand();
	}
	else
	{
		Result = FLinearColor::MakeRandomColor();
	}

	Result.A = bRandomAlpha ? FMath::FRand() : 1.0f;
	return Result;
}

FColor UGTMathLibrary::ColorFromHex(const FString InHex)
{
	return FColor::FromHex(InHex);
}

FString UGTMathLibrary::ColorToHex(const FColor InColor)
{
	return InColor.ToHex();
}

FColor UGTMathLibrary::ColorFromTemperature(const float InTemperature)
{
	return FColor::MakeFromColorTemperature(InTemperature);
}

FColor UGTMathLibrary::RandomColor(const bool bTrueRandom, const bool bRandomAlpha)
{
	FColor Result;
	if (bTrueRandom)
	{
		Result.R = FMath::RandRange(0, 255);
		Result.G = FMath::RandRange(0, 255);
		Result.B = FMath::RandRange(0, 255);
	}
	else
	{
		Result = FColor::MakeRandomColor();
	}

	Result.A = bRandomAlpha ? FMath::FRand() : 255;
	return Result;
}

FDateTime UGTMathLibrary::GetDateTimeFromTicks(const int64 InTicks)
{
	return FDateTime(InTicks);
}

int64 UGTMathLibrary::GetDateTimeTicks(const FDateTime& InDateTime)
{
	return InDateTime.GetTicks();
}

FTimespan UGTMathLibrary::GetTimespanFromTicks(const int64 InTicks)
{
	return FTimespan(InTicks);
}

int64 UGTMathLibrary::GetTimespanTicks(const FTimespan& InTimespan)
{
	return InTimespan.GetTicks();
}
