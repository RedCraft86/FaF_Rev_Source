// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once



#include "FaF_Rev.h"
#include "Data/LightingData.h"
#include "Engine/AssetManager.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PlayerData.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum EPlayerControlFlags
{
	PCF_None		= 0			UMETA(Hidden),
	PCF_Locked		= 1 << 0	UMETA(DisplayName = "Locked"),

	// Ability flags
	PCF_UseStamina	= 1 << 1	UMETA(DisplayName = "Use Stamina"),
	PCF_CanPause	= 1 << 2	UMETA(DisplayName = "Can Pause"),
	PCF_CanTurn		= 1 << 3	UMETA(DisplayName = "Can Turn"),
	PCF_CanWalk		= 1 << 4	UMETA(DisplayName = "Can Walk"),
	PCF_CanRun		= 1 << 5	UMETA(DisplayName = "Can Run"),
	PCF_CanCrouch	= 1 << 6	UMETA(DisplayName = "Can Crouch"),
	PCF_CanLean		= 1 << 7	UMETA(DisplayName = "Can Lean"),
	PCF_CanInteract	= 1 << 8	UMETA(DisplayName = "Can Interact"),
	PCF_CanHide		= 1 << 9	UMETA(DisplayName = "Can Hide")
};
ENUM_CLASS_FLAGS(EPlayerControlFlags);
ENUM_RANGE_BY_FIRST_AND_LAST(EPlayerControlFlags, PCF_Locked, PCF_CanHide);
#define DEFAULT_PLAYER_CONTROL_FLAGS PCF_UseStamina | PCF_CanPause | PCF_CanTurn \
	| PCF_CanWalk | PCF_CanRun | PCF_CanCrouch | PCF_CanLean | PCF_CanInteract | PCF_CanHide

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlayerStateFlags : uint8
{
	PSF_None		= 0			UMETA(Hidden),
	PCF_Running		= 1 << 0	UMETA(DisplayName = "Running"),
	PCF_RunLocked	= 1 << 1	UMETA(DisplayName = "Stamina Punished"),
	PCF_Crouching	= 1 << 2	UMETA(DisplayName = "Crouching"),
	PCF_Interacting	= 1 << 3	UMETA(DisplayName = "Interacting"),
};
ENUM_CLASS_FLAGS(EPlayerStateFlags);
#define PLAYER_STATE_FLAG(Flag) static_cast<uint8>(EPlayerStateFlags::Flag)

USTRUCT(BlueprintType)
struct FAF_REV_API FPlayerLockFlag : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN(FPlayerLockFlag)
};

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (Bitmask, BitmaskEnum = "/Script/FaF_Rev.EPlayerControlFlags"))
		int32 ControlFlags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Movement")
		float MoveSpeedMultiplier;

	// X: Gain, Y: Drain
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Running|Stamina", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 5.0f, UIMax = 5.0f))
		FVector2D StaminaRates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Misc")
		FPointLightProperties LightProperties;

	FPlayerSettings()
		: ControlFlags(DEFAULT_PLAYER_CONTROL_FLAGS)
		, MoveSpeedMultiplier(1.0f)
		, StaminaRates({2.5f, 1.75f})
	{}
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
	FORCEINLINE bool IsValidData() const { return IsValid(Target) && Hit.bBlockingHit; }
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

	UPROPERTY(EditAnywhere, Category = "PlayerFootsteps", meta = (ReadOnlyKeys, DisplayThumbnail = false))
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
				const EPhysicalSurface Type = static_cast<EPhysicalSurface>(i);
				if (i == 0 || Enum->HasMetaData(TEXT("Hidden"), i))
				{
					if (SurfaceSounds.Contains(TEnumAsByte(Type)))
						SurfaceSounds.Remove(TEnumAsByte(Type));
				}
				else if (!SurfaceSounds.Contains(TEnumAsByte(Type)))
				{
					SurfaceSounds.Add(TEnumAsByte(Type));
				}
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
		FPlayerFootstepSounds CrouchSounds;

	FPlayerFootsteps()
		: Volume(1.0f), FloorTraceChannel(ECC_Visibility), Intervals(FVector(0.5f, 0.35f, 0.6f))
		, WalkSounds({}), RunSounds({}), CrouchSounds({}) {}

	void CheckEntries()
	{
		WalkSounds.CheckEntries();
		RunSounds.CheckEntries();
		CrouchSounds.CheckEntries();
	}

	float GetInterval(const bool bRun, const bool bCrouch) const
	{
		return bRun ? Intervals.Y : bCrouch ? Intervals.Z : Intervals.X;
	}

	USoundBase* GetAudio(const bool bRun, const bool bCrouch, const EPhysicalSurface Surface) const
	{
		return bRun ? RunSounds.GetSound(Surface) : bCrouch ? CrouchSounds.GetSound(Surface) : WalkSounds.GetSound(Surface);
	}

	void LoadAssetsAsync() const
	{
		WalkSounds.LoadAssetsAsync();
		RunSounds.LoadAssetsAsync();
		CrouchSounds.LoadAssetsAsync();
	}
};

namespace Player
{
	namespace LockFlags
	{
		inline static FName Loading			= TEXT("Loading");
		inline static FName Cutscene		= TEXT("Cutscene");
		inline static FName Dialogue		= TEXT("Dialogue");
		inline static FName Jumpscare		= TEXT("Jumpscare");
		inline static FName GuideScreen		= TEXT("GuideScreen");
		inline static FName Inventory		= TEXT("Inventory");
		inline static FName Inspection		= TEXT("Inspection");
		inline static FName WorldDevice		= TEXT("WorldDevice");
		inline static FName Hiding			= TEXT("Hiding");

		inline static TSet All = {Loading, Cutscene, Dialogue, Jumpscare, GuideScreen,
			Inventory, Inspection, WorldDevice, Hiding};

		inline static TSet CanReset = {Inventory, Inspection, WorldDevice, Hiding};
		inline static TSet AllowJumpscare = {Inventory, Inspection, WorldDevice};
	}
	
	namespace Actions
	{
		inline static FName Pause			= TEXT("Pause");
		inline static FName Turn			= TEXT("Turn");
		inline static FName Move			= TEXT("Move");
		inline static FName Run				= TEXT("Run");
		inline static FName Crouch			= TEXT("Crouch");
		inline static FName Lean			= TEXT("Lean");
		inline static FName Inventory		= TEXT("Inventory");
		inline static FName HideQuests		= TEXT("HideQuests");
		inline static FName Interact		= TEXT("Interact");
		inline static FName Equipment		= TEXT("Equipment");
		inline static FName Equipment_Alt	= TEXT("Equipment_Alt");

		inline static TSet All = {Pause, Turn, Move, Run, Crouch, Lean,
			Inventory, HideQuests, Interact, Equipment, Equipment_Alt};
	}

	static float LeanStateToFloat(const EPlayerLeanState& State)
	{
		switch (State)
		{
		case EPlayerLeanState::Left: return -1.0f;
		case EPlayerLeanState::Right: return 1.0f;
		default: return 0.0f;
		}
	}
}
