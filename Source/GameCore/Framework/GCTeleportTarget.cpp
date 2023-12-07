// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCTeleportTarget.h"
#include "Player/GCPlayerCharacter.h"
#include "Core/CoreLogging.h"
#include "EngineUtils.h"
#if WITH_EDITOR
#include "Components/BillboardComponent.h"
#endif

AGCTeleportTarget::AGCTeleportTarget()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
#endif
#if WITH_EDITOR
	if (GetSpriteComponent())
	{
		ConstructorHelpers::FObjectFinder<UTexture2D> IconFinder(TEXT("/Engine/EditorResources/Waypoint.Waypoint"));
		if (IconFinder.Succeeded())
		{
			GetSpriteComponent()->Sprite = IconFinder.Object;
		}
	}
#endif
}

AGCTeleportTarget* AGCTeleportTarget::Get(const UObject* WorldContext, const FGameplayTag& Tag)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (AGCTeleportTarget* Target : TActorRange<AGCTeleportTarget>(World))
		{
			if (Target && Target->GetTeleporterTags().HasTag(Tag))
			{
				return Target;
			}
		}
	}

	return nullptr;
}

void AGCTeleportTarget::TeleportPlayer() const
{
	if (AGCPlayerCharacter* Player = AGCPlayerCharacter::Get(this))
	{
		TeleportActor(Player);
	}
	else
	{
		UE_LOG(GameCore, Warning, TEXT("Could not find Player, failed to teleport to teleporter with tags %s (Name: %s)"),
			*TeleporterTag.ToStringSimple(), *GetName());
	}
}

void AGCTeleportTarget::TeleportActor(AActor* ActorToTeleport) const
{
	if (ActorToTeleport)
	{
		const FTransform TargetPos = GetActorTransform();
		ActorToTeleport->TeleportTo(TargetPos.GetTranslation(),
			TargetPos.GetRotation().Rotator(), false, true);
	}
	else
	{
		UE_LOG(GameCore, Warning, TEXT("Actor is null, failed to teleport to teleporter with tags %s (Name: %s)"),
			*TeleporterTag.ToStringSimple(), *GetName());
	}
}
