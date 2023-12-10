// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCLightFunctionComponent.h"

#include "Components/SpotLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Light.h"

UGCLightFunctionComponent::UGCLightFunctionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	LightFunction = nullptr;
	ScalarParams = {};
	VectorParams = {};
	MID = nullptr;
	bFailed = false;
}

void UGCLightFunctionComponent::Update()
{
	bFailed = false;
	if (!GetOwner()->IsA(ALight::StaticClass()))
	{
		UE_LOG(GameCore, Error, TEXT("Light Function Component can only be placed on light source actors. Invalid Actor: %s"), *GetName());
		bFailed = true;
		return;
	}

	if (!LightFunction)
	{
		bFailed = true;
		return;
	}
	
	if (!IsValid(MID))
	{
		MID = UMaterialInstanceDynamic::Create(LightFunction, this, FName(GetName() + "_LightFuncMat"));
	}

	if (MID)
	{
		TArray<FMaterialParameterInfo> SParams; TArray<FGuid> SGuids;
		LightFunction->GetAllScalarParameterInfo(SParams, SGuids);
		for (FMaterialParameterInfo SParam : SParams)
		{
			const float Value = ScalarParams.FindOrAdd(SParam.Name, 0.0f);
			MID->SetScalarParameterValue(SParam.Name, Value);
		}
		
		TArray<FMaterialParameterInfo> VParams; TArray<FGuid> VGuids;
		LightFunction->GetAllVectorParameterInfo(VParams, VGuids);
		for (FMaterialParameterInfo VParam : VParams)
		{
			const FLinearColor Value = VectorParams.FindOrAdd(VParam.Name, FLinearColor::Black);
			MID->SetVectorParameterValue(VParam.Name, Value);
		}

		if (const ALight* Light = Cast<ALight>(GetOwner()))
		{
			ULightComponent* LC = Light->GetLightComponent();
			if (UPointLightComponent* PLC = Cast<UPointLightComponent>(LC))
			{
				PLC->SetLightFunctionMaterial(MID);
			}
			else if (USpotLightComponent* SLC = Cast<USpotLightComponent>(LC))
			{
				SLC->SetLightFunctionMaterial(MID);
			}
		}
	}
	else
	{
		bFailed = true;
	}
}

void UGCLightFunctionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Update();
	if (bFailed)
	{
		DestroyComponent();
	}
}
