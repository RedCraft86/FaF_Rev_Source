// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "FRPlayer.h"
#include "Data/InlineCurves.h"
#include "Data/PrimitiveData.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "FRDoorActor.generated.h"

UCLASS(Abstract, DisplayName = "Door Actor Base")
class FAF_REV_API AFRDoorBase final : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:

	AFRDoorBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> DoorPivot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<class UBoxComponent> DoorBox;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UAudioComponent> AudioHigh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UAudioComponent> AudioLow;

	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bEnabled;

	UPROPERTY(EditAnywhere, Category = "Settings")
		bool bMultidirectional;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = -150.0f, UIMin = -120.0f, ClampMax = 150.0f, UIMax = 120.0f))
		float OpenRotation;

	UPROPERTY(EditAnywhere, Category = "Settings")
		FTransformMeshData DoorProperties;
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		FVector BoxScale;

	UPROPERTY(EditAnywhere, Category = "Settings|Key", meta = (DisplayThumbnail = false))
		TObjectPtr<class UInventoryItemDataBase> KeyItem;

	UPROPERTY(EditAnywhere, Category = "Settings|Key", meta = (EditCondition = "KeyItem != nullptr", EditConditionHides))
		FString KeyID;

	// Inject Variables: {Key}
	UPROPERTY(EditAnywhere, Category = "Settings")
		FInteractionInfo LockedInfo;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		bool bDebugView = true;
	
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay, meta = (ClampMin = -1.0f, UIMin = -1.0f, ClampMax = 1.0f, UIMax = 1.0f))
		float PreviewAlpha = 0.0f;
#endif
	
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		FInlineFloatCurve AnimationCurve;

	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		TObjectPtr<USoundBase> LockedSound;

	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		TObjectPtr<USoundBase> OpenSound;
	
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		TObjectPtr<USoundBase> CloseSound;
	
	UPROPERTY(EditAnywhere, Category = "Settings", AdvancedDisplay)
		TObjectPtr<USoundBase> CreakSound;
	
	UFUNCTION(BlueprintCallable, Category = "DoorActor")
		void SetState(const bool bInState);

	UFUNCTION(BlueprintPure, Category = "DoorActor")
		bool GetState() const { return bState; }

	UFUNCTION(BlueprintPure, Category = "DoorActor")
		bool IsLocked() const { return KeyItem && !KeyID.IsEmpty(); }
	
private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient) TObjectPtr<UArrowComponent> ForwardArrow;
	UPROPERTY(Transient) TObjectPtr<class UDebugShapesComponent> ShapeVisualizer;
#endif

	bool bState;
	float DoorRotaton;
	TObjectPtr<AActor> Interactor;
	FTimerHandle PawnCooldown;
	
	float AnimSpeed;
	bool bAnimFinished;
	FVector2D AnimValues;

	float CalcOpenRotation() const;
	void PlayAnim(const bool bReverse);
	void ApplyRotation(const float Alpha) const;
	bool CheckKey(const AFRPlayerBase* Player);
	
	void OnAnimFinished() const;
	void OnAnimTick(const float Alpha) const;

	virtual bool GetInteractionInfo_Implementation(FInteractionInfo& Info) override;
	virtual void OnBeginInteract_Implementation(AFRPlayerBase* Player, const FHitResult& HitResult) override;
	virtual void OnBeginPawnInteract_Implementation(APawn* Pawn, const FHitResult& HitResult) override;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
