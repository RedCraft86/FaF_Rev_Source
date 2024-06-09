// Copyright (C) RedCraft86. All Rights Reserved.

#include "Optimization/SmartCulling.h"

void USmartCullingComponent::AddRenderRequest(const UObject* Object)
{
	if (bDisableComponent || !Object) return;
	const int32 PreSize = RenderRequests.Num();
	RenderRequests.AddUnique(Object);
	if (PreSize != RenderRequests.Num())
	{
		UpdateRenderingState();
	}
}

void USmartCullingComponent::RemoveRenderRequest(const UObject* Object)
{
	if (bDisableComponent || !Object) return;
	if (RenderRequests.Remove(Object) > 0)
	{
		UpdateRenderingState();
	}
}

bool USmartCullingComponent::HasRenderRequest(const UObject* Object) const
{
	return RenderRequests.Contains(Object);
}

void USmartCullingComponent::CheckRenderRequests()
{
	if (bDisableComponent) return;
	const uint8 NumRemoved = RenderRequests.RemoveAll([](const TSoftObjectPtr<const UObject>& Element) -> bool
	{
		return Element.IsNull() || !IsValid(Element.LoadSynchronous());
	});
	if (NumRemoved > 0)
	{
		UpdateRenderingState();
	}
}

void USmartCullingComponent::UpdateRenderingState()
{
	if (bDisableComponent) return;
	if (bCachedHiddenState != RenderRequests.IsEmpty())
	{
		bCachedHiddenState = RenderRequests.IsEmpty();
		GetOwner()->SetActorHiddenInGame(bCachedHiddenState);
		if (bAffectTicking)
		{
			if (bCachedHiddenState)
			{
				bInitialTickState = GetOwner()->IsActorTickEnabled();
				GetOwner()->SetActorTickEnabled(false);
			}
			else
			{
				GetOwner()->SetActorTickEnabled(bInitialTickState);
			}
		}
	}
}

void USmartCullingComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<USmartCullingComponent*> Comps;
	GetOwner()->GetComponents<USmartCullingComponent>(Comps);
#if WITH_EDITOR
	ensureMsgf(Comps.Num() == 1, TEXT("Multiple Smart Culling Components found on actor %s (%s) [%s]."),
		*TSoftObjectPtr(GetOwner()).ToString(), *GetOwner()->GetActorLabel(), *GetOwner()->GetClass()->GetName());
#endif
	if (Comps.Num() != 1)
	{
		bDisableComponent = true;
		return;
	}
	
	bInitialTickState = GetOwner()->IsActorTickEnabled();
	GetWorld()->GetTimerManager().SetTimer(CheckTimer, this,
		&ThisClass::CheckRenderRequests, NullCheckInterval, true);

	UpdateRenderingState();
}

USmartCullingComponent* USmartCullingComponent::GetSmartCulling(const AActor* Target, EGTFoundPins& OutStatus)
{
	USmartCullingComponent* Component = GetSmartCulling(Target);
	OutStatus = Component ? EGTFoundPins::Found : EGTFoundPins::NotFound;
	return Component;
}

USmartCullingComponent* USmartCullingComponent::GetSmartCulling(const AActor* Target)
{
	TArray<USmartCullingComponent*> Comps;
	if (Target) Target->GetComponents<USmartCullingComponent>(Comps);
	if (Comps.Num() == 1) return Comps[0];
	return nullptr;
}
