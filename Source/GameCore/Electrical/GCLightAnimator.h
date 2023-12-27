// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "GCLightAnimator.generated.h"

UCLASS(Abstract, EditInlineNew, DefaultToInstanced, Blueprintable, DisplayName = "Light Animator")
class GAMECORE_API UGCLightAnimator : public UObject
{
	GENERATED_BODY()

	friend class AGCElectricalLight;

public:

	UGCLightAnimator() { bPlaying = false; }

	UPROPERTY(EditAnywhere, Category = "Animator", meta = (ClampMin = 0.1f, UIMin = 0.1f, DisplayPriority = -1))
		bool bLooping;

	UPROPERTY(EditAnywhere, Category = "Animator", meta = (ClampMin = 0.1f, UIMin = 0.1f, DisplayPriority = -1))
		float PlayRate;

	UFUNCTION(BlueprintCallable, Category = "LightAnimator")
		virtual void StartAnimation(const bool bReset = false) { bPlaying = true; }

	UFUNCTION(BlueprintCallable, Category = "LightAnimator")
		virtual void StopAnimation() { bPlaying = false; }

	virtual void GetValues(float& Intensity, FLinearColor& Color) const {}
	
	virtual UWorld* GetWorld() const override
	{
#if WITH_EDITOR
		if (!FApp::IsGame())
		{
			return GEngine ? GEngine->GetCurrentPlayWorld() : GWorld;
		}
#endif
		
		if (UWorld* OuterWorld = Super::GetWorld())
		{
			return OuterWorld;
		}
		
		return GEngine ? GEngine->GetCurrentPlayWorld() : GWorld;
	}
	
protected:

	bool bPlaying;

	virtual void Init() {}
	virtual void Tick(const float DeltaTime) {}
};


UCLASS(DisplayName = "Intensity Animator")
class GAMECORE_API UGCLightAnim_Intensity : public UGCLightAnimator
{
	GENERATED_BODY()

public:
	
	UGCLightAnim_Intensity();

	UPROPERTY(EditAnywhere, Category = "Animator")
		FRuntimeFloatCurve AnimationCurve;

	UPROPERTY(EditAnywhere, Category = "Animator", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		FVector2D OutputRange;

	UPROPERTY(VisibleAnywhere, Category = "Data")
		FVector2D TimeRange;
	
	UPROPERTY(VisibleAnywhere, Category = "Data")
		FVector2D ValueRange;

	UPROPERTY(VisibleAnywhere, Category = "Data")
		float PlaybackTime;

	UPROPERTY(VisibleAnywhere, Category = "Data")
		float CurrentValue;

	UPROPERTY(VisibleAnywhere, Category = "Data")
		float ScaledValue;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Preview")
		float PreviewTime = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Preview")
		float PreviewValue = 0.0f;
#endif

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void Init() override;
	virtual void Tick(const float DeltaTime) override;
	FORCEINLINE virtual void GetValues(float& Intensity, FLinearColor& Color) const override { Intensity *= ScaledValue; }
	
	virtual void StartAnimation(const bool bReset) override;
	virtual void StopAnimation() override;
};
