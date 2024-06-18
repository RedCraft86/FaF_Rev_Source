// Copyright (C) RedCraft86. All Rights Reserved.

#include "ElectricTaskActor.h"

void AElectricTaskActor::SetupEvents()
{
	SetActorTickEnabled(false);
	
	ActivateEventPtrs.Empty(ActivateEvents.Num());
	for (FInstancedStruct& Event : ActivateEvents)
	{
		FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>();
		if (!EventPtr) continue;
		
		ActivateEventPtrs.Add(EventPtr);
		if (EventPtr->bRequiresTick && !IsActorTickEnabled())
		{
			SetActorTickEnabled(true);
		}
	}

	DeactivateEventPtrs.Empty(DeactivateEvents.Num());
	for (FInstancedStruct& Event : DeactivateEvents)
	{
		FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>();
		if (!EventPtr) continue;
		
		DeactivateEventPtrs.Add(EventPtr);
		if (EventPtr->bRequiresTick && !IsActorTickEnabled())
		{
			SetActorTickEnabled(true);
		}
	}
}

void AElectricTaskActor::BeginPlay()
{
	Super::BeginPlay();

	SetupEvents();
	for (FWorldEventBase* EventPtr : ActivateEventPtrs)
	{
		if (EventPtr) EventPtr->OnBeginPlay(this);
	}
	for (FWorldEventBase* EventPtr : DeactivateEventPtrs)
	{
		if (EventPtr) EventPtr->OnBeginPlay(this);
	}
}

void AElectricTaskActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	for (FWorldEventBase* EventPtr : ActivateEventPtrs)
	{
		if (EventPtr && EventPtr->bRequiresTick) EventPtr->OnTick(this, DeltaSeconds);
	}
	for (FWorldEventBase* EventPtr : DeactivateEventPtrs)
	{
		if (EventPtr && EventPtr->bRequiresTick) EventPtr->OnTick(this, DeltaSeconds);
	}
}

void AElectricTaskActor::OnStateChanged(const bool bState)
{
	if (bState)
	{
		for (FWorldEventBase* EventPtr : ActivateEventPtrs)
		{
			if (EventPtr) EventPtr->RunEvent(this);
		}
	}
	else
	{
		for (FWorldEventBase* EventPtr : DeactivateEventPtrs)
		{
			if (EventPtr) EventPtr->RunEvent(this);
		}
	}
	Super::OnStateChanged(bState);
}

#if WITH_EDITOR
void AElectricTaskActor::PostLoad()
{
	Super::PostLoad();
	if (FApp::IsGame()) return;
	TArray<FInstancedStruct> Events = ActivateEvents;
	Events.Append(DeactivateEvents);
	
	for (FInstancedStruct& Event : Events)
	{
		if (FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>())
		{
			EventPtr->OnConstruction(this, true);
		}
	}
}

void AElectricTaskActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (FApp::IsGame()) return;
	TArray<FInstancedStruct> Events = ActivateEvents;
	Events.Append(DeactivateEvents);
	
	for (FInstancedStruct& Event : Events)
	{
		if (FWorldEventBase* EventPtr = Event.GetMutablePtr<FWorldEventBase>())
		{
			EventPtr->OnConstruction(this, true);
		}
	}
}
#endif
