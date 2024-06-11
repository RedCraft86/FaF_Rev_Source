// Copyright (C) RedCraft86. All Rights Reserved.

#include "ActorIconData.h"
#include "Components/BillboardComponent.h"

FGTActorIconData::FGTActorIconData(const FString& BaseIconPath)
	: Sprite(nullptr), bScreenSizeScaled(true), ScreenSizeScale(0.0025f)
{
	Transform = {
		FRotator::ZeroRotator,
		FVector::ZeroVector,
		FVector{0.25f}
	};
#if WITH_EDITOR
	BaseSprite = FSoftObjectPath(BaseIconPath.IsEmpty()
		? TEXT("/Engine/EditorResources/EmptyActor.EmptyActor") : BaseIconPath);
#endif
}

void FGTActorIconData::ApplyData(UBillboardComponent* InBillboard) const
{
	if (InBillboard)
	{
		if (Sprite)
		{
			InBillboard->SetSprite(Sprite);
			InBillboard->SetHiddenInGame(false);
		}
#if WITH_EDITOR
		else
		{
			InBillboard->SetSprite(BaseSprite.LoadSynchronous());
			InBillboard->SetHiddenInGame(true);
		}
#endif
		
		InBillboard->SetRelativeTransform(Transform);
		InBillboard->SetRelativeScale3D_Direct(Transform.GetScale3D());
		InBillboard->bIsScreenSizeScaled = bScreenSizeScaled;
		InBillboard->ScreenSize = ScreenSizeScale;
	}
}
