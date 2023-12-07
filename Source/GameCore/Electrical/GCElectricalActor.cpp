// Copyright (C) Tayzar Linn. All Rights Reserved.

// ReSharper disable CppParameterMayBeConst
#include "GCElectricalActor.h"
#include "EngineUtils.h"

UGCElectronicVisualizer::UGCElectronicVisualizer()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UGCElectronicVisualizer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (const AGCElectricalActor* Electronic = Cast<AGCElectricalActor>(GetOwner()))
	{
		for (const AGCElectricalActor* Link : Electronic->NextLinks)
		{
			if (Link)
			{
				DrawDebugLine(GetWorld(), Electronic->GetActorLocation(), Link->GetActorLocation(),
					Electronic->bLocalState ? (Electronic->EvaluateConditions() ? FColor::Green : FColor::Yellow) : FColor::Red,
					false, DeltaTime, SDPG_Foreground, 2);
			}
		}
	}
}

#if WITH_EDITOR
TArray<FVector> UGCElectronicVisualizer::GetNextTargets() const
{
	TArray<FVector> Result;
	if (const AGCElectricalActor* Electronic = Cast<AGCElectricalActor>(GetOwner()))
	{
		for (const AGCElectricalActor* Link : Electronic->NextLinks)
		{
			if (Link && Link->LastLinks.Contains(Electronic))
			{
				Result.Add(Link->GetActorLocation());
			}
		}
	}

	return Result;
}

TArray<FVector> UGCElectronicVisualizer::GetLastTargets() const
{
	TArray<FVector> Result;
	if (const AGCElectricalActor* Electronic = Cast<AGCElectricalActor>(GetOwner()))
	{
		for (const AGCElectricalActor* Link : Electronic->LastLinks)
		{
			if (Link && Link->NextLinks.Contains(Electronic))
			{
				Result.Add(Link->GetActorLocation());
			}
		}
	}

	return Result;
}
#endif

AGCElectricalActor::AGCElectricalActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = true;
#endif
#if WITH_EDITOR
	DebugVisualizer = CreateEditorOnlyDefaultSubobject<UGCElectronicVisualizer>("DebugVisualizer");
	if (DebugVisualizer) DebugVisualizer->SetIsVisualizationComponent(true);
#endif
	
	ElectronicType = EGCElectronicType::Toggle;
	bLocalState = false;
	bIsDamaged = false;
	ElectronicTag = {};
	ConditionType = EGCConditionType::RequiresAny;
	bDrawDebugLines = false;
	NextLinks = {};
	LastLinks = {};

	bFirstUpdate = false;
	bLastFinalState = false;
}

void AGCElectricalActor::ResyncLinks()
{
	NextLinks.Remove(nullptr);
	for (AGCElectricalActor* Link : NextLinks)
	{
		if (Link)
		{
			Link->LastLinks.Add(this);
		}
	}
	
	LastLinks.Remove(nullptr);
	const TSet<AGCElectricalActor*> TempLastLinks = LastLinks;
	for (const AGCElectricalActor* Link : TempLastLinks)
	{
		if (Link && !Link->NextLinks.Contains(this))
		{
			LastLinks.Remove(Link);
		}
	}
}

#if WITH_EDITOR
void AGCElectricalActor::FindNextLinks()
{
#if WITH_EDITORONLY_DATA
	if (LinkClass == NULL || ElectronicType == EGCElectronicType::Appliance)
	{
		return;
	}
	
	for (AGCElectricalActor* Actor : TActorRange<AGCElectricalActor>(GetWorld()))
	{
		if (Actor && Actor->ElectronicType != EGCElectronicType::Generator && Actor->IsA(LinkClass)
			&& (LinkTags.IsEmpty() || LinkTags.HasTagExact(Actor->ElectronicTag)))
		{
			NextLinks.Add(Actor);
		}
	}
#endif
}
#endif

void AGCElectricalActor::RefreshElectronic()
{
	const bool bFinalState = GetFinalState();
	if (bLastFinalState != bFinalState || bFirstUpdate)
	{
		bFirstUpdate = false;
		bLastFinalState = bFinalState;
		OnStateChanged(bLastFinalState);
	}
	
	for (AGCElectricalActor* Link : NextLinks)
	{
		if (Link)
		{
			Link->RefreshElectronic();
		}
	}
}

void AGCElectricalActor::SetLocalState(const bool bNewState)
{
	if (bLocalState != bNewState && ElectronicType != EGCElectronicType::Connector)
	{
		bLocalState = bNewState;
		RefreshElectronic();
	}
}

void AGCElectricalActor::ToggleLocalState()
{
	SetLocalState(!bLocalState);
}

void AGCElectricalActor::SetIsDamaged(const bool bNewDamaged)
{
	if (bIsDamaged != bNewDamaged)
	{
		bIsDamaged = bNewDamaged;
		RefreshElectronic();
	}
}

bool AGCElectricalActor::EvaluateConditions() const
{
	if (LastLinks.IsEmpty() || ElectronicType == EGCElectronicType::Generator)
	{
		return true;
	}
	
	bool bResult = ConditionType == EGCConditionType::RequiresAll;
	for (const AGCElectricalActor* Link : LastLinks)
	{
		if (Link && Link->NextLinks.Contains(this))
		{
			switch (ConditionType)
			{
			case EGCConditionType::RequiresAny:
				if (Link->GetFinalState())
				{
					return true;
				}
				break;

			case EGCConditionType::RequiresAll:
				bResult &= Link->GetFinalState();
				if (!bResult)
				{
					return false;
				}
				break;
			}
		}
	}

	return bResult;
}

bool AGCElectricalActor::GetFinalState() const
{
	return bLocalState && !bIsDamaged && EvaluateConditions();
}

void AGCElectricalActor::BeginPlay()
{
	Super::BeginPlay();
	
	ResyncLinks();

	bFirstUpdate = true;
	RefreshElectronic();

	if (bIsDamaged)
	{
		OnDamaged(bIsDamaged);
	}

#if WITH_EDITOR
	if (!bDrawDebugLines && DebugVisualizer)
	{
		DebugVisualizer->DestroyComponent();
	}
#endif
}

void AGCElectricalActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	NextLinks.Remove(this);
	LastLinks.Remove(this);
	
	if (ElectronicType == EGCElectronicType::Connector)
	{
		bLocalState = true;
		bIsDamaged = false;
	}

	for (AGCElectricalActor* Link : NextLinks)
	{
		if (Link)
		{
			Link->LastLinks.Add(this);
		}
	}

	bFirstUpdate = true;
	RefreshElectronic();
}

void AGCElectricalActor::OnStateChanged(const bool bState)
{
	EventStateChanged(bState);
}

void AGCElectricalActor::OnDamaged(const bool bDamaged)
{
	EventDamaged(bDamaged);
}

AGCElectricalLight::AGCElectricalLight()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

#if WITH_EDITORONLY_DATA
	SceneRoot->bVisualizeComponent = false;
#endif

	bLocalState = true;
	ElectronicType = EGCElectronicType::Appliance;
#if WITH_EDITOR
	bForceStaticType = true;
#endif

	AnimBroken = nullptr;
	AnimEnable = nullptr;
}

void AGCElectricalLight::GetAnimMultipliers(float& OutIntensity, FLinearColor& OutColor) const
{
	float Intensity = 1.0f;
	FLinearColor Color = FLinearColor::White;
	if (AnimBroken) { AnimBroken->GetValues(Intensity, Color); }
	if (AnimEnable) { AnimEnable->GetValues(Intensity, Color); }

	OutIntensity = Intensity;
	OutColor = Color;
}

void AGCElectricalLight::BeginPlay()
{
	Super::BeginPlay();

	if (AnimBroken)
	{
		AnimBroken->Init();
	}
	
	if (AnimEnable)
	{
		AnimEnable->Init(); AnimEnable->bLooping = false;
	}
}

void AGCElectricalLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AnimBroken) { AnimBroken->Tick(DeltaTime); }
	if (AnimEnable) { AnimEnable->Tick(DeltaTime); }
	ExecuteApply();
}

void AGCElectricalLight::ExecuteApply()
{
	float Intensity;
	FLinearColor Color;
	GetAnimMultipliers(Intensity, Color);
	ApplyProperties(Intensity, Color);
}

void AGCElectricalLight::OnDamaged(const bool bDamaged)
{
	Super::OnDamaged(bDamaged);

	ExecuteApply();
	if (AnimBroken)
	{
		bDamaged ? AnimBroken->StartAnimation(true) : AnimBroken->StopAnimation();
	}
}

void AGCElectricalLight::OnStateChanged(const bool bState)
{
	Super::OnStateChanged(bState);
	
	ExecuteApply();
	if (bState)
	{
		if (AnimEnable) { AnimEnable->StartAnimation(true); }
	}
	else
	{
		if (AnimEnable) { AnimEnable->StopAnimation(); }
	}
}
