// Copyright (C) RedCraft86. All Rights Reserved.

#include "SmartBlendable.h"
#include "SmartPostProcess.h"

void USmartBlendable::SetIntensity(const float NewValue)
{
	InterpTarget = FMath::Max(0.0f, NewValue);
	InterpSpeed = 0.0f;
}

void USmartBlendable::AdjustIntensity(const float NewValue, const float Speed)
{
	InterpTarget = FMath::Max(0.0f, NewValue);
	InterpSpeed = FMath::IsNearlyZero(Speed) || Speed < 0.0f ? InterpSpeed : Speed;
}

UWorld* USmartBlendable::GetWorld() const
{
	return PostProcessActor ? PostProcessActor->GetWorld() : Super::GetWorld();
}

void USmartBlendable::OnInitialize()
{
	InterpTarget = Intensity;
	AssignBlendable();
	EventInit();
}

void USmartBlendable::AssignBlendable()
{
	if (!DynamicInstance)
	{
		DynamicInstance = UMaterialInstanceDynamic::Create(ParentMaterial, this);
	}

	if (bEnabled)
	{
		DynamicInstance->SetScalarParameterValue(TEXT("BlendIntensity"), Intensity);
		DynamicInstance->SetScalarParameterValue(TEXT("BlendStencilValue"), bUseStencil ? StencilValue : -1.0f);
	}
}

void USmartBlendable::OnTick(const float DeltaTime)
{
#if WITH_EDITOR
	if (!FApp::IsGame())
	{
		Intensity = InterpTarget;
	}
	else
#endif
	{
		if (FMath::IsNearlyZero(InterpSpeed) || InterpSpeed < 0.0f)
		{
			Intensity = InterpTarget;
		}
		else
		{
			Intensity = FMath::FInterpTo(Intensity, InterpTarget, DeltaTime, InterpSpeed);
		}
	}
	
	AssignBlendable();
	EventTick(DeltaTime);
}
