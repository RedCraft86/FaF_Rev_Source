// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTVolume.h"
#include "Components/BrushComponent.h"
#include "Components/BillboardComponent.h"

AGTVolume::AGTVolume() : bEnabled(true)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	IconComponent = CreateDefaultSubobject<UBillboardComponent>("IconComponent");
	IconComponent->SetupAttachment(GetRootComponent());
	IconComponent->SetHiddenInGame(true);
	
	GetBrushComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetBrushComponent()->SetCollisionObjectType(ECC_WorldDynamic);

#if WITH_EDITOR
	Icon = {TEXT("/Engine/EditorResources/S_TriggerBox.S_TriggerBox")};
#endif
#if WITH_EDITORONLY_DATA
	RuntimeGuid = ActorGuid;
#else
	RuntimeGuid = FGuid::NewGuid();
#endif
}

void AGTVolume::SetEnabled(const bool bInEnabled)
{
	if (bEnabled != bInEnabled)
	{
		bEnabled = bInEnabled;
		ON_ENABLE_STATE_CHANGED
	}
}

void AGTVolume::BeginPlay()
{
	Super::BeginPlay();
	if (!bEnabled) ON_ENABLE_STATE_CHANGED
}

void AGTVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Icon.ApplyData(IconComponent);
	if (!RuntimeGuid.IsValid())
	{
#if WITH_EDITORONLY_DATA
		RuntimeGuid = ActorGuid;
#else
		RuntimeGuid = FGuid::NewGuid();
#endif
	}
}

void AGTVolume::OnEnableStateChanged(const bool bIsEnabled)
{
	SetActorHiddenInGame(!bIsEnabled);
	SetActorEnableCollision(bIsEnabled);
}
