// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GCPostProcessBlendable.h"
#include "GCPostProcessActor.generated.h"

USTRUCT(BlueprintType, DisplayName = "Bloom Settings")
struct GAMECORE_API FGCBloomSettings
{
	GENERATED_BODY()

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "BloomSettings", meta = (DisplayName = "Method"))
		TEnumAsByte<EBloomMethod> BloomMethod;
	
	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "BloomSettings", meta = (ClampMin = 0.0f, UIMax = 8.0f, DisplayName = "Intensity"))
		float BloomIntensity;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category="BloomSettings", meta = (ClampMin = -1.0f, UIMax = 8.0f, DisplayName = "Threshold", EditCondition = "BloomMethod == EBloomMethod::BM_SOG"))
		float BloomThreshold;
	
	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category="BloomSettings", meta = (ClampMin = 0.0f, UIMax = 20.0f, DisplayName = "Convolution Scatter Dispersion", EditCondition = "BloomMethod == EBloomMethod::BM_FFT"))
		float BloomConvolutionScatterDispersion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloomSettings", meta = (DisplayName = "Convolution Kernel", EditCondition = "BloomMethod == EBloomMethod::BM_FFT"))
		TObjectPtr<UTexture2D> BloomConvolutionTexture;

	FGCBloomSettings()
		: BloomMethod(BM_SOG)
		, BloomIntensity(0.675f)
		, BloomThreshold(-1.0f)
		, BloomConvolutionScatterDispersion(1.0f)
		, BloomConvolutionTexture(nullptr)
	{}

	void AlterPostProcess(FPostProcessSettings& Settings) const
	{
		Settings.bOverride_BloomMethod = true;
		Settings.bOverride_BloomIntensity = true;
		Settings.bOverride_BloomThreshold = true;
		Settings.bOverride_BloomConvolutionScatterDispersion = true;
		Settings.bOverride_BloomConvolutionTexture = true;

		Settings.BloomMethod = BloomMethod;
		Settings.BloomIntensity = BloomIntensity;
		Settings.BloomThreshold = BloomThreshold;
		Settings.BloomConvolutionScatterDispersion = BloomConvolutionScatterDispersion;
		Settings.BloomConvolutionTexture = BloomConvolutionTexture;
	}
};

USTRUCT(BlueprintType, DisplayName = "Extended Post Process Settings")
struct GAMECORE_API FGCPostProcessSettings
{
	GENERATED_BODY()

	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ShowOnlyInnerProperties))
		FPostProcessSettings Settings;

	/* The shape of the bloom effect.
	* 0: Off (Default)
	* <0: Anamorphic Shape (X)
	* >0: Cross Shape (XY)
	*/
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Settings|Bloom", meta = (ClampMin = -1.0f, UIMin = -1.0f, ClampMax = 1.0f, UIMax = 1.0f))
		float BloomShape;

	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Settings|Bloom")
		bool bStartFancy;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Settings|Bloom")
		FGCBloomSettings SimpleBloom;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Settings|Bloom")
		FGCBloomSettings FancyBloom;

	FGCPostProcessSettings()
		: BloomShape(0.0f)
		, bStartFancy(true)
	{
		Settings.bOverride_AutoExposureMethod = true;
		Settings.AutoExposureMethod = AEM_Manual;

		Settings.bOverride_AutoExposureBias = true;
		Settings.AutoExposureBias = 10.0f;
	}

	FORCEINLINE const FPostProcessSettings& operator*() const { return Settings; }
	const FPostProcessSettings& GetScaledSettings(const bool bFancyBloom);
};

UCLASS(NotBlueprintable, DisplayName = "Post Process Actor", HideCategories = (Rendering, Collision, Physics, Input))
class GAMECORE_API AGCPostProcessActor final : public AActor
{
	GENERATED_BODY()

public:

	AGCPostProcessActor();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UBoxComponent* SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		class UPostProcessComponent* PostProcess;

	/* Priority of this volume. In the case of overlapping volumes the one with the highest priority
	* overrides the lower priority ones. The order is undefined if two or more overlapping volumes have the same priority
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume")
		float Priority;

	/* World space radius around the volume that is used for blending (only if not unbound) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume", Interp, meta = (ClampMin = 0.0, UIMin = 0.0, UIMax = 6000.0f))
		float BlendRadius;

	/* 0:no effect, 1:full effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume", Interp, meta = (UIMin = 0.0, UIMax = 1.0f))
		float BlendWeight;

	/* Whether this volume is enabled or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume", Interp)
		bool bEnabled;

	/* set this to false to use the parent shape component as volume bounds. True affects the whole world regardless */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume", Interp)
		bool bUnbound;
	
	/* Post process settings to use for this volume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Interp, meta = (ShowOnlyInnerProperties))
		FGCPostProcessSettings Settings;

	/* Post process blendable materials */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Instanced, meta = (DisplayPriority = 0))
		TArray<UGCPostProcessBlendable*> Blendables;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Copying")
		bool bPreserveExposure = true;
	
	UPROPERTY(EditAnywhere, Category = "Copying")
		const AActor* CopyTarget = nullptr;
#endif
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Copying")
		void CopyFromTarget();
#endif
	
	UFUNCTION(BlueprintPure, Category = "PostProcessActor", meta = (DeterminesOutputType = "InClass"))
		UGCPostProcessBlendable* FindSmartBlendable(UPARAM(meta = (AllowAbstract = false)) const TSubclassOf<UGCPostProcessBlendable> InClass) const;
	
	UFUNCTION(BlueprintCallable, Category = "PostProcessActor", meta = (DeterminesOutputType = "InClass"))
		UGCPostProcessBlendable* AddSmartBlendable(UPARAM(meta = (AllowAbstract = false)) const TSubclassOf<UGCPostProcessBlendable> InClass);
	
	template<typename T = UGCPostProcessBlendable>
	T* FindSmartBlendable() const
	{
		return Cast<T>(FindSmartBlendable(T::StaticClass()));
	}

	template<typename T = UGCPostProcessBlendable>
	TArray<T*> AddSmartBlendable() const
	{
		return Cast<T>(AddSmartBlendable(T::StaticClass()));
	}
	
protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
private:
#if WITH_EDITOR
	UBillboardComponent* VisualIcon;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	float DistToPoint;

	void ApplySettings();
	void ApplyBlendables();
	float CalculateCVarBlend();
	bool GetCameraPosition(FVector& OutCameraLocation) const;
};
