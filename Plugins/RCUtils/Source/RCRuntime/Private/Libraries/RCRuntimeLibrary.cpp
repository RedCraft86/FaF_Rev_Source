// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "RCRuntimeLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/PlayerController.h"
#include "LevelSequence/Public/LevelSequenceActor.h"
#include "Components/AudioComponent.h"
#include "LevelSequencePlayer.h"
#include "Sound/AmbientSound.h"
#include "InputCoreTypes.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/MaterialBillboardComponent.h"

void URCRuntimeLibrary::SetPrimitiveCollision(UPrimitiveComponent* Target, const FRCPrimitiveCollision& CollisionData)
{
	if (!Target)
		return;

	FRCPrimitiveCollision Data = CollisionData;
	if (FRCPrimitiveCollision::IsProfileValid(CollisionData.CollisionProfileName))
	{
		if (FApp::IsGame())
		{
			Target->SetCollisionProfileName(CollisionData.CollisionProfileName);
			return;
		}
		
		Data.SetCollisionProfile(CollisionData.CollisionProfileName);
	}

	Target->SetCollisionEnabled(Data.CollisionEnabled);
	Target->SetCollisionObjectType(Data.ObjectType);
	for (TPair<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> Pair : Data.Responses)
	{
		Target->SetCollisionResponseToChannel(Pair.Key, Pair.Value);
	}
}

FRCPrimitiveCollision URCRuntimeLibrary::GetPrimitiveCollisions(const UPrimitiveComponent* Target)
{
	if (!Target)
		return {};

	if (FRCPrimitiveCollision::IsProfileValid(Target->GetCollisionProfileName()))
	{
		return FRCPrimitiveCollision(Target->GetCollisionProfileName());
	}
	
	FRCPrimitiveCollision CollisionData(Target->GetCollisionEnabled(), Target->GetCollisionObjectType());
	if (const UEnum* ChannelEnum = StaticEnum<ECollisionChannel>())
	{
		for (int32 i = 0; i < ChannelEnum->NumEnums(); i++)
		{
			const TEnumAsByte<ECollisionChannel> EnumVal(i);
			
#if WITH_EDITORONLY_DATA
			if (!ChannelEnum->HasMetaData(TEXT("Hidden"), i) && EnumVal != ECC_MAX)
#else
			if (EnumVal != ECC_MAX)
#endif
			{
				CollisionData.Responses.Add(EnumVal, Target->GetCollisionResponseToChannel(EnumVal));
			}
		}
	}
		
	return CollisionData;
}

void URCRuntimeLibrary::ResetStaticMeshMaterials(UStaticMeshComponent* Target)
{
	if (!Target)
		return;

	for (uint8 i = 0; i < Target->GetNumMaterials(); i++)
	{
		Target->SetMaterial(i, Target->GetStaticMesh()->GetStaticMaterials()[i].MaterialInterface);
	}
}

void URCRuntimeLibrary::SetStaticMeshProperties(UStaticMeshComponent* Target, FRCStaticMeshProperties Properties)
{
	if (!Target || !Properties.IsValidMesh())
		return;

	Properties.FillMaterials();

	Target->SetStaticMesh(Properties.Mesh);
	Target->SetCastShadow(Properties.bCastShadows);
	if (!Properties.Materials.IsEmpty())
	{
		for (uint8 i = 0; i < Properties.Materials.Num(); i++)
		{
			Target->SetMaterial(i, Properties.Materials[i]);
		}
	}
}

FRCStaticMeshProperties URCRuntimeLibrary::GetStaticMeshProperties(const UStaticMeshComponent* Target)
{
	if (!Target)
		return {};

	FRCStaticMeshProperties Properties(Target->GetStaticMesh(), Target->GetMaterials(), Target->CastShadow);
	Properties.FillMaterials();
	return Properties;
}

void URCRuntimeLibrary::SetLightDrawDistanceSettings(ULightComponent* Target, const FRCLightDrawDistance& Settings)
{
	if (Target)
	{
		Target->MaxDrawDistance = Settings.GetMaxDrawDistance();
		Target->MaxDistanceFadeRange = Settings.GetMaxDistanceFadeRange();
	}
}

FRCLightDrawDistance URCRuntimeLibrary::GetLightDrawDistanceSettings(const ULightComponent* Target)
{
	FRCLightDrawDistance OutData;
	if (Target)
	{
		OutData.MaxDrawDistance = Target->MaxDrawDistance;
		OutData.MaxDistanceFadeRange = Target->MaxDistanceFadeRange;
		OutData.IdentifyDrawDistanceUsage();
	}
	
	return OutData;
}

void URCRuntimeLibrary::SetBaseLightProperties(ULightComponent* Target, const FRCBaseLightProperties& Properties)
{
	if (!Target)
		return;

	Target->SetIntensity(Properties.Intensity);
	Target->SetLightColor(Properties.Color);
	Target->SetUseTemperature(Properties.bUseTemperature);
	Target->SetTemperature(Properties.Temperature);
	Target->SetIndirectLightingIntensity(Properties.IndirectIntensity);
	Target->SetVolumetricScatteringIntensity(Properties.VolumetricScatteringIntensity);
	Target->SetCastShadows(Properties.bLightCastShadows);
	Target->SetCastVolumetricShadow(Properties.bCastVolumetricShadows);
	Target->SetSpecularScale(Properties.SpecularScale);

	// NOTE: Intensity Units and Attenuation Radius are not part of the ULightComponent so we need to cast to ULocalLightComponent
	if (ULocalLightComponent* LocalLight = Cast<ULocalLightComponent>(Target))
	{
		LocalLight->SetIntensityUnits(Properties.IntensityUnits);
		LocalLight->SetAttenuationRadius(Properties.AttenuationRadius);
	}

	SetLightDrawDistanceSettings(Target, Properties.DrawDistance);
}

FRCBaseLightProperties URCRuntimeLibrary::GetBaseLightProperties(const ULightComponent* Target)
{
	FRCBaseLightProperties OutData;
	if (Target)
	{
		OutData.Intensity = Target->Intensity;
		OutData.Color = Target->LightColor;
		OutData.bUseTemperature = Target->bUseTemperature;
		OutData.Temperature = Target->Temperature;
		OutData.IndirectIntensity = Target->IndirectLightingIntensity;
		OutData.VolumetricScatteringIntensity = Target->VolumetricScatteringIntensity;
		OutData.bLightCastShadows = Target->CastShadows;
		OutData.bCastVolumetricShadows = Target->bCastVolumetricShadow;
		OutData.SpecularScale = Target->SpecularScale;

		// NOTE: Intensity Units and Attenuation Radius are not part of the ULightComponent so we need to cast to ULocalLightComponent
		if (const ULocalLightComponent* LocalLight = Cast<ULocalLightComponent>(Target))
		{
			OutData.IntensityUnits = LocalLight->IntensityUnits;
			OutData.AttenuationRadius = LocalLight->AttenuationRadius;
		}

		OutData.DrawDistance = GetLightDrawDistanceSettings(Target);
	}
	
	return OutData;
}

void URCRuntimeLibrary::SetPointLightProperties(UPointLightComponent* Target, const FRCPointLightProperties& Properties)
{
	if (!Target)
		return;
	
	SetBaseLightProperties(Target, Properties);
	Target->SetSourceRadius(Properties.SourceRadius);
	Target->SetSoftSourceRadius(Properties.SoftSourceRadius);
	Target->SetSourceLength(Properties.SourceLength);
	Target->bUseInverseSquaredFalloff = Properties.bUseInverseSquaredFalloff;
	Target->SetLightFalloffExponent(Properties.LightFalloffExponent);
}

FRCPointLightProperties URCRuntimeLibrary::GetPointLightProperties(const UPointLightComponent* Target)
{
	FRCPointLightProperties OutData;
	if (Target)
	{
		OutData.CopyFrom(GetBaseLightProperties(Target));
		OutData.SourceRadius = Target->SourceRadius;
		OutData.SoftSourceRadius = Target->SoftSourceRadius;
		OutData.SourceLength = Target->SourceLength;
		OutData.bUseInverseSquaredFalloff = Target->bUseInverseSquaredFalloff;
		OutData.LightFalloffExponent = Target->LightFalloffExponent;
	}
	
	return OutData;
}

void URCRuntimeLibrary::SetSpotLightProperties(USpotLightComponent* Target, const FRCSpotLightProperties& Properties)
{
	if (!Target)
		return;

	SetBaseLightProperties(Target, Properties);
	Target->SetInnerConeAngle(Properties.InnerConeAngle);
	Target->SetOuterConeAngle(Properties.OuterConeAngle);
	Target->SetSourceRadius(Properties.SourceRadius);
	Target->SetSoftSourceRadius(Properties.SoftSourceRadius);
	Target->SetSourceLength(Properties.SourceLength);
	Target->bUseInverseSquaredFalloff = Properties.bUseInverseSquaredFalloff;
	Target->SetLightFalloffExponent(Properties.LightFalloffExponent);
}

FRCSpotLightProperties URCRuntimeLibrary::GetSpotLightProperties(const USpotLightComponent* Target)
{
	FRCSpotLightProperties OutData;
	if (Target)
	{
		OutData.CopyFrom(GetBaseLightProperties(Target));
		OutData.InnerConeAngle = Target->InnerConeAngle;
		OutData.OuterConeAngle = Target->OuterConeAngle;
		OutData.SourceRadius = Target->SourceRadius;
		OutData.SoftSourceRadius = Target->SoftSourceRadius;
		OutData.SourceLength = Target->SourceLength;
		OutData.bUseInverseSquaredFalloff = Target->bUseInverseSquaredFalloff;
		OutData.LightFalloffExponent = Target->LightFalloffExponent;
	}

	return OutData;
}

void URCRuntimeLibrary::SetRectLightProperties(URectLightComponent* Target, const FRCRectLightProperties& Properties)
{
	if (!Target)
		return;

	SetBaseLightProperties(Target, Properties);
	Target->SetSourceWidth(Properties.SourceWidth);
	Target->SetSourceHeight(Properties.SourceHeight);
	Target->SetBarnDoorAngle(Properties.BarnDoorAngle);
	Target->SetBarnDoorLength(Properties.BarnDoorLength);
	Target->SetSourceTexture(Properties.SourceTexture);
}

FRCRectLightProperties URCRuntimeLibrary::GetRectLightProperties(const URectLightComponent* Target)
{
	FRCRectLightProperties OutData;
	if (Target)
	{
		OutData.CopyFrom(GetBaseLightProperties(Target));
		OutData.SourceWidth = Target->SourceWidth;
		OutData.SourceHeight = Target->SourceHeight;
		OutData.BarnDoorAngle = Target->BarnDoorAngle;
		OutData.BarnDoorLength = Target->BarnDoorLength;
		OutData.SourceTexture = Target->SourceTexture;
	}
	
	return OutData;
}

int32 URCRuntimeLibrary::TraceNumberOfWalls(const UObject* WorldContextObject, const FVector Start, const FVector End, ECollisionChannel TraceChannel, const TArray<AActor*>& ActorsToIgnore, const int MaxNumOfWalls)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
		return 0;
	
	const AActor* OwnerActor = Cast<AActor>(WorldContextObject);
	if (!OwnerActor)
		OwnerActor = Cast<AActor>(WorldContextObject->GetOuter());
	
	FCollisionQueryParams TraceParams(TEXT("RCTrace_NumOfWalls"), false, OwnerActor);
	TraceParams.AddIgnoredActor(World->GetFirstPlayerController()->GetPawn());
	TraceParams.AddIgnoredActors(ActorsToIgnore);

	int32 NumOfWalls = 0;
	FHitResult HitResult;
	HitResult.bBlockingHit = true;
	FVector StartInternal = Start;
	while (HitResult.bBlockingHit && NumOfWalls <= MaxNumOfWalls)
	{
		FCollisionQueryParams TempTraceParams = TraceParams;
		TempTraceParams.AddIgnoredActor(HitResult.GetActor());
		if (World->LineTraceSingleByChannel(HitResult, StartInternal, End, TraceChannel, TempTraceParams))
		{
			NumOfWalls++;
			StartInternal = HitResult.Location;
		}
	}

	return NumOfWalls;
}

void URCRuntimeLibrary::TriggerVirtualKey(const FKey& InKey, const ERCKeyTriggerType TriggerType, const bool bRepeat, const bool bDoubleClick)
{
	if (!FSlateApplication::IsInitialized() || !InKey.IsValid())
		return;

	const TSharedPtr<GenericApplication> PlatformApp = FSlateApplication::Get().GetPlatformApplication();
	const TSharedRef<FGenericApplicationMessageHandler> MsgHandler = PlatformApp->GetMessageHandler();
	if (InKey.IsMouseButton())
	{
		const EMouseButtons::Type MouseButton = KeyAsMouseButton(InKey);
		switch(TriggerType)
		{
		case ERCKeyTriggerType::Clicked:
			MsgHandler->OnMouseDown(PlatformApp->GetWindowUnderCursor(), MouseButton);
			MsgHandler->OnMouseUp(MouseButton);
			if (bDoubleClick)
			{
				MsgHandler->OnMouseDown(PlatformApp->GetWindowUnderCursor(), MouseButton);
				MsgHandler->OnMouseUp(MouseButton);
			}
			break;

		case ERCKeyTriggerType::Pressed:
			MsgHandler->OnMouseDown(PlatformApp->GetWindowUnderCursor(), MouseButton);
			break;

		case ERCKeyTriggerType::Released:
			MsgHandler->OnMouseUp(MouseButton);
			break;
		}
	}
	else
	{
		int32 KeyCode = 0;
		int32 CharCode = 0;
		GetKeyCode(InKey, KeyCode, CharCode);
		switch(TriggerType)
		{
		case ERCKeyTriggerType::Clicked:
			MsgHandler->OnKeyDown(KeyCode, CharCode, false);
			MsgHandler->OnKeyChar(static_cast<TCHAR>(CharCode),false);
			MsgHandler->OnKeyUp(KeyCode, CharCode, false);
			break;

		case ERCKeyTriggerType::Pressed:
			MsgHandler->OnKeyDown(KeyCode, CharCode, bRepeat);
			MsgHandler->OnKeyChar(static_cast<TCHAR>(CharCode),bRepeat);
			break;

		case ERCKeyTriggerType::Released:
			MsgHandler->OnKeyUp(KeyCode, CharCode, false);
			break;
		}
	}
}

void URCRuntimeLibrary::RestartLevel(const UObject* WorldContextObject)
{
	const FString NameStr = UGameplayStatics::GetCurrentLevelName(WorldContextObject, true);
	UGameplayStatics::OpenLevel(WorldContextObject, *NameStr, true, TEXT(""));
}

void URCRuntimeLibrary::PlayAmbientAudio(AAmbientSound* Target, const float StartTime)
{
	if (IsValid(Target))
	{
		Target->GetAudioComponent()->Play(StartTime);
	}
}

void URCRuntimeLibrary::PlayLevelSequence(ALevelSequenceActor* Target)
{
	if (IsValid(Target))
	{
		Target->GetSequencePlayer()->Play();
	}
}

void URCRuntimeLibrary::OnLevelSequenceFinished(ALevelSequenceActor* Target, const FOnSequencerFinished& Callback)
{
	if (IsValid(Target) && Callback.IsBound())
	{
		Target->GetSequencePlayer()->OnFinished.Add(Callback);
	}
}

void URCRuntimeLibrary::ActorAddTag(AActor* Target, const FName InTag)
{
	if (IsValid(Target) && !InTag.IsNone())
	{
		Target->Tags.AddUnique(InTag);
	}
}

void URCRuntimeLibrary::ActorDeleteTag(AActor* Target, const FName InTag)
{
	if (IsValid(Target) && !InTag.IsNone())
	{
		Target->Tags.Remove(InTag);
	}
}

void URCRuntimeLibrary::SetBillboardElement(UMaterialBillboardComponent* Target, const int32 Index, UMaterialInterface* Material, const bool bSizeIsInScreenSpace, const float BaseSizeX, const float BaseSizeY)
{
	TArray<FMaterialSpriteElement> Elems = Target->Elements;
	if (Elems.IsValidIndex(Index))
	{
		FMaterialSpriteElement& ElemRef = Elems[Index];
		ElemRef.bSizeIsInScreenSpace = bSizeIsInScreenSpace;
		ElemRef.BaseSizeX = BaseSizeX;
		ElemRef.BaseSizeY = BaseSizeY;
		ElemRef.Material = Material;
	}

	Target->SetElements(Elems);
}

TArray<FVector> URCRuntimeLibrary::GetBoundingBoxVertices(const AActor* Target, const bool bOnlyCollidingComponents, const bool bIncludeFromChildActors, FVector& Origin, FVector& BoxExtent)
{
	if (!IsValid(Target)) return {};

	FVector _Origin, _BoxExtent = FVector::ZeroVector;
	Target->GetActorBounds(bOnlyCollidingComponents, _Origin, _BoxExtent, bIncludeFromChildActors);

	Origin = _Origin;
	BoxExtent = _BoxExtent;

	TArray<FVector> Result;
	Result.Add(_Origin + _BoxExtent);
	Result.Add(_Origin - _BoxExtent);
	Result.Add(FVector(_Origin.X - _BoxExtent.X, _Origin.Y + _BoxExtent.Y, _Origin.Z + _BoxExtent.Z));
	Result.Add(FVector(_Origin.X + _BoxExtent.X, _Origin.Y - _BoxExtent.Y, _Origin.Z + _BoxExtent.Z));
	Result.Add(FVector(_Origin.X + _BoxExtent.X, _Origin.Y + _BoxExtent.Y, _Origin.Z - _BoxExtent.Z));
	Result.Add(FVector(_Origin.X - _BoxExtent.X, _Origin.Y - _BoxExtent.Y, _Origin.Z + _BoxExtent.Z));
	Result.Add(FVector(_Origin.X + _BoxExtent.X, _Origin.Y - _BoxExtent.Y, _Origin.Z - _BoxExtent.Z));
	Result.Add(FVector(_Origin.X - _BoxExtent.X, _Origin.Y + _BoxExtent.Y, _Origin.Z - _BoxExtent.Z));

	return Result;
}

bool URCRuntimeLibrary::IsActorInFront(const AActor* Target, const AActor* ActorToTest)
{
	if (!IsValid(Target) || !IsValid(ActorToTest))
	{
		return false;
	}
	
	const APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(Target, 0);
	if (!CamManager) CamManager = UGameplayStatics::GetPlayerCameraManager(ActorToTest, 0);

	FVector DotVec = CamManager->GetCameraLocation() - Target->GetActorLocation();
	DotVec.Normalize();

	return FVector::DotProduct(Target->GetActorForwardVector(), DotVec) > 0.0f;
}

bool URCRuntimeLibrary::IsActorInScreen(const AActor* Target, const float MaxDistance, const bool bOriginOnly, const bool bLineTraceCheck, const FActorBoundsCheckParams& TraceCheckParams)
{
	if (!IsValid(Target) || MaxDistance <= 0 || (bLineTraceCheck && TraceCheckParams.BoundingBoxLerp.Size() <= 0))
	{
		return false;
	}

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(Target, 0);
	if (!CameraManager) return false;

	if (FVector::Distance(CameraManager->GetCameraLocation(), Target->GetActorLocation()) > MaxDistance)
	{
		return false;
	}

	APlayerController* Controller = UGameplayStatics::GetPlayerController(Target, 0);
	if (!Controller) return false;

	TArray<FVector> TestVectors;
	TestVectors.Add(Target->GetActorLocation());

	if (!bOriginOnly)
	{
		FVector Origin, BoxExtent = FVector::ZeroVector;
		TArray<FVector> BoundingBoxVertices = GetBoundingBoxVertices(Target, TraceCheckParams.bOnlyCollidingComponents, TraceCheckParams.bIncludeFromChildActors, Origin, BoxExtent);

		TestVectors.AddUnique(Origin);
		for (const FVector Vector : BoundingBoxVertices)
		{
			TestVectors.AddUnique(FVector(
				FMath::Lerp(Origin.X, Vector.X, TraceCheckParams.BoundingBoxLerp.X),
				FMath::Lerp(Origin.Y, Vector.Y, TraceCheckParams.BoundingBoxLerp.Y),
				FMath::Lerp(Origin.Z, Vector.Z, TraceCheckParams.BoundingBoxLerp.Z)));
		}
	}

	bool bPassedScreenTest = false;
	for (int i = 0; i < TestVectors.Num(); i++)
	{
		FVector2D ScreenLoc;
		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(Controller);
		if (!Controller->ProjectWorldLocationToScreen(TestVectors[i], ScreenLoc))
			continue;

		if ((ScreenLoc.X >= 0 && ScreenLoc.X <= ViewportSize.X) && (ScreenLoc.Y >= 0 && ScreenLoc.Y <= ViewportSize.Y))
		{
			bPassedScreenTest = true;
			break;
		}
	}

	if (!bPassedScreenTest)
		return false;
	else if (!bLineTraceCheck)
		return true;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Target);
	QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(Target, 0));

	FHitResult HitResult;
	for (int i = 0; i < TestVectors.Num(); i++)
	{
		if (!Target->GetWorld()->LineTraceSingleByChannel(HitResult, CameraManager->GetCameraLocation(), TestVectors[i], TraceCheckParams.LineTraceChannel, QueryParams))
			return true;
	}

	return false;
}

float URCRuntimeLibrary::GetDistanceFromCamera(const AActor* Target, const bool bIncludeZ)
{
	if (!IsValid(Target)) return 0.0f;
	
	if (const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(Target, 0))
	{
		const FVector A = Target->GetActorLocation();
		const FVector B = CameraManager->GetCameraLocation();
		return bIncludeZ ? FVector::Dist(A, B) : FVector::DistXY(A, B);
	}

	return 0.0f;
}

EMouseButtons::Type URCRuntimeLibrary::KeyAsMouseButton(const FKey& InKey)
{
	if (InKey.IsMouseButton())
	{
		if (InKey == EKeys::LeftMouseButton)
		{
			return EMouseButtons::Left;
		}
		if (InKey == EKeys::RightMouseButton)
		{
			return EMouseButtons::Right;
		}
		if (InKey == EKeys::MiddleMouseButton)
		{
			return EMouseButtons::Middle;
		}
		if (InKey == EKeys::ThumbMouseButton)
		{
			return EMouseButtons::Thumb01;
		}
		if (InKey == EKeys::ThumbMouseButton2)
		{
			return EMouseButtons::Thumb02;
		}
	}

	return EMouseButtons::Invalid;
}

void URCRuntimeLibrary::GetKeyCode(const FKey& InKey, int32& KeyCode, int32& CharCode)
{
	const uint32* KeyCodePtr;
	const uint32* CharCodePtr;
	FInputKeyManager::Get().GetCodesFromKey(InKey, KeyCodePtr, CharCodePtr);
	KeyCode = KeyCodePtr ? static_cast<int32>(*KeyCodePtr) : 0;
	CharCode = CharCodePtr ? static_cast<int32>(*CharCodePtr) : 0;
}
