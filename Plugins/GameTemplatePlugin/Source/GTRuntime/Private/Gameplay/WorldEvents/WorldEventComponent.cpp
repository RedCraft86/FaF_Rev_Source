// Copyright (C) RedCraft86. All Rights Reserved.

#include "WorldEvents/WorldEventComponent.h"

UWorldEventComponent::UWorldEventComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	Events = {};
}

void UWorldEventComponent::SetupEvents()
{
	SetComponentTickEnabled(false);

	EventPtrs.Empty(Events.Num());
	for (FInstancedStruct& Event : Events)
	{
		FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>();
		if (!EventPtr) continue;
		
		EventPtrs.Add(EventPtr);
		if (EventPtr->bRequiresTick && !IsComponentTickEnabled())
		{
			SetComponentTickEnabled(true);
		}
	}
}

void UWorldEventComponent::RunEvents()
{
	if (EventPtrs.IsEmpty() && !Events.IsEmpty()) SetupEvents();
	for (FWorldEventBase* EventPtr : EventPtrs)
	{
		if (EventPtr) EventPtr->RunEvent(this);
	}
}

void UWorldEventComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetupEvents();
	for (FWorldEventBase* EventPtr : EventPtrs)
	{
		if (EventPtr) EventPtr->OnBeginPlay(this);
	}
}

void UWorldEventComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	for (FWorldEventBase* EventPtr : EventPtrs)
	{
		if (EventPtr && EventPtr->bRequiresTick) EventPtr->OnTick(this, DeltaTime);
	}
}

#if WITH_EDITOR
void UWorldEventComponent::PostLoad()
{
	Super::PostLoad();
	if (FApp::IsGame()) return;
	for (FInstancedStruct& Event : Events)
	{
		if (FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>())
		{
			EventPtr->OnConstruction(this, true);
		}
	}
}

void UWorldEventComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (FApp::IsGame()) return;
	for (FInstancedStruct& Event : Events)
	{
		if (FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>())
		{
			EventPtr->OnConstruction(this, true);
		}
	}
}
#endif
