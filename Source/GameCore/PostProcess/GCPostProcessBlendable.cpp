// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCPostProcessBlendable.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GCPostProcessActor.h"

UGCPostProcessBlendable::UGCPostProcessBlendable()
{
	bEnabled = true;
	Intensity = 1.0f;
	bUseStencil = false;
	StencilValue = 0;
	ParentMaterial = nullptr;
	DynamicInstance = nullptr;
	PostProcessActor = nullptr;
	InterpTarget = 1.0f;
	InterpSpeed = 5.0f;
}

void UGCPostProcessBlendable::SetIntensity(const float NewValue)
{
	InterpTarget = FMath::Max(0.0f, NewValue);
	InterpSpeed = 0.0f;
}

void UGCPostProcessBlendable::AdjustIntensity(const float NewValue, const float Speed)
{
	InterpTarget = FMath::Max(0.0f, NewValue);
	InterpSpeed = FMath::IsNearlyZero(Speed) || Speed < 0.0f ? InterpSpeed : Speed;
}

void UGCPostProcessBlendable::OnInit()
{
	InterpTarget = Intensity;
	AssignBlendable();
	EventInit();
}

void UGCPostProcessBlendable::OnTick(const float DeltaTime)
{
#if WITH_EDITOR
	if (FApp::IsGame())
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
#if WITH_EDITOR
	else
	{
		Intensity = InterpTarget;
	}
#endif
	
	AssignBlendable();
	EventTick(DeltaTime);
}

void UGCPostProcessBlendable::AssignBlendable()
{
	if (PostProcessActor)
	{
		if (!DynamicInstance)
		{
			DynamicInstance = UMaterialInstanceDynamic::Create(ParentMaterial, this);
		}

        if (bEnabled)
        {
        	DynamicInstance->SetScalarParameterValue(TEXT("Intensity"), Intensity);
        	DynamicInstance->SetScalarParameterValue(TEXT("StencilValue"), bUseStencil ? StencilValue : -1.0f);
        	PostProcessActor->PostProcess->Settings.AddBlendable(DynamicInstance, 1.0f);
        }
        else
        {
        	PostProcessActor->PostProcess->Settings.RemoveBlendable(DynamicInstance);
        }
	}
}