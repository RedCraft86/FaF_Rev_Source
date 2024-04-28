// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Engine/AssetManager.h"
#include "PlayerData.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlayerControlFlags : uint8
{
	Locked,
};
ENUM_CLASS_FLAGS(EPlayerControlFlags);

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlayerStateFlags : uint8
{
	Hiding,
};
ENUM_CLASS_FLAGS(EPlayerStateFlags);

UENUM(BlueprintType)
enum class EPlayerLeanState : uint8
{
	None,
	Left,
	Right
};

USTRUCT(BlueprintType)
struct FPlayerSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerFlags"))
		uint8 PlayerFlags;

	FPlayerSettings() : PlayerFlags(0) {}
};

USTRUCT(BlueprintInternalUseOnly)
struct FPlayerInteraction
{
	GENERATED_BODY()
	
	AActor* Target;
	FHitResult Hit;
	
	FPlayerInteraction() : Target(nullptr) {}
	FPlayerInteraction(AActor* InTarget, const FHitResult& InHit) : Target(InTarget), Hit(InHit) {}
	FORCEINLINE bool operator==(const FPlayerInteraction& Other) const { return Target == Other.Target; }
	FORCEINLINE bool operator!=(const FPlayerInteraction& Other) const { return Target != Other.Target; }
	FORCEINLINE bool IsValidData() const { return IsValid(Target) && IsValid(Hit.GetActor()); }
	void Reset()
	{
		Target = nullptr;
		Hit.Reset(1.0f, false);
	}
};

USTRUCT(BlueprintInternalUseOnly)
struct FPlayerCameraShakes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "CameraShake")
		TSoftClassPtr<UCameraShakeBase> WalkShake;

	UPROPERTY(EditAnywhere, Category = "CameraShake", meta = (EditCondition = "WalkShake != nullptr"))
		float WalkScale;

	UPROPERTY(EditAnywhere, Category = "CameraShake")
		TSoftClassPtr<UCameraShakeBase> RunShake;

	UPROPERTY(EditAnywhere, Category = "CameraShake", meta = (EditCondition = "RunShake != nullptr"))
		float RunScale;

	FPlayerCameraShakes() : WalkShake(nullptr), WalkScale(1.0f), RunShake(nullptr), RunScale(0.75f) {}
	void LoadAssetsAsync() const
	{
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();
		Manager.RequestAsyncLoad(WalkShake.ToSoftObjectPath());
		Manager.RequestAsyncLoad(RunShake.ToSoftObjectPath());
	}
};

USTRUCT(BlueprintInternalUseOnly)
struct FPlayerFootstepSounds
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "PlayerFootsteps")
		TSoftObjectPtr<USoundBase> DefaultSound;

	UPROPERTY(EditAnywhere, Category = "PlayerFootsteps", meta = (ReadOnlyKeys))
		TMap<TEnumAsByte<EPhysicalSurface>, TSoftObjectPtr<USoundBase>> SurfaceSounds;

	FPlayerFootstepSounds() : DefaultSound(nullptr), SurfaceSounds({}) {}

	USoundBase* GetSound(const EPhysicalSurface Surface) const
	{
		TSoftObjectPtr<USoundBase> const* FoundPtr = SurfaceSounds.Find(TEnumAsByte(Surface));
		USoundBase* Sound = FoundPtr ? FoundPtr->LoadSynchronous() : nullptr;
		return Sound ? Sound : DefaultSound.LoadSynchronous();
	}
	
	void LoadAssetsAsync() const
	{
		TArray<TSoftObjectPtr<USoundBase>> Sounds;
		SurfaceSounds.GenerateValueArray(Sounds);
		Sounds.AddUnique(DefaultSound);
		
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();
		for (TSoftObjectPtr<USoundBase>& Sound : Sounds)
		{
			Manager.RequestAsyncLoad(Sound.ToSoftObjectPath());
		}
	}

	void CheckEntries()
	{
#if WITH_EDITOR
		if (SurfaceSounds.Contains(TEnumAsByte(SurfaceType_Default)))
		{
			TSoftObjectPtr<USoundBase> const* FoundPtr = SurfaceSounds.Find(TEnumAsByte(SurfaceType_Default));
			DefaultSound = DefaultSound.IsNull() && FoundPtr && !FoundPtr->IsNull() ? *FoundPtr : DefaultSound;
			SurfaceSounds.Remove(TEnumAsByte(SurfaceType_Default));
		}
		
		if (const UEnum* Enum = StaticEnum<EPhysicalSurface>())
		{
			for(int32 i = 1; i < Enum->NumEnums(); i++)
			{
				if(i == 0 || Enum->HasMetaData(TEXT("Hidden"), i)) continue;
				const EPhysicalSurface Type = static_cast<EPhysicalSurface>(i);
				if (SurfaceSounds.Contains(TEnumAsByte(Type))) continue;
				SurfaceSounds.Add(TEnumAsByte(Type));
			}
		}
#endif
	}
};

USTRUCT(BlueprintInternalUseOnly)
struct FPlayerFootsteps
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootsteps", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		float Volume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootsteps")
		TEnumAsByte<ECollisionChannel> FloorTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootsteps", meta = (ClampMin = 0.1f, UIMin = 0.1f))
		FVector Intervals;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootsteps")
		FPlayerFootstepSounds WalkSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootsteps")
		FPlayerFootstepSounds RunSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerFootsteps")
		FPlayerFootstepSounds SneakSounds;

	FPlayerFootsteps()
		: Volume(1.0f), FloorTraceChannel(ECC_Visibility), Intervals(FVector(0.5f, 0.35f, 0.6f))
		, WalkSounds({}), RunSounds({}), SneakSounds({}) {}

	void CheckEntries()
	{
		WalkSounds.CheckEntries();
		RunSounds.CheckEntries();
		SneakSounds.CheckEntries();
	}

	float GetInterval(const bool bRun, const bool bSneak) const
	{
		return bRun ? Intervals.Y : bSneak ? Intervals.Z : Intervals.X;
	}

	USoundBase* GetAudio(const bool bRun, const bool bSneak, const EPhysicalSurface Surface) const
	{
		return bRun ? RunSounds.GetSound(Surface) : bSneak ? SneakSounds.GetSound(Surface) : WalkSounds.GetSound(Surface);
	}

	void LoadAssetsAsync() const
	{
		WalkSounds.LoadAssetsAsync();
		RunSounds.LoadAssetsAsync();
		SneakSounds.LoadAssetsAsync();
	}
};
