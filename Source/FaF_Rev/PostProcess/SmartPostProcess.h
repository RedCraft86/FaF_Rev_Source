// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "SmartBlendable.h"
#include "GameFramework/Actor.h"
#include "SmartPostProcess.generated.h"

USTRUCT(BlueprintType, DisplayName = "Bloom Settings")
struct FAF_REV_API FPPBloomSettings
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

	FPPBloomSettings() : BloomMethod(BM_SOG), BloomIntensity(0.675f), BloomThreshold(-1.0f)
		, BloomConvolutionScatterDispersion(1.0f), BloomConvolutionTexture(nullptr)
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
struct FAF_REV_API FFRPostProcessSettings
{
	GENERATED_BODY()

	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ShowOnlyInnerProperties))
		FPostProcessSettings Settings;

	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Settings|Bloom")
		bool bStartFancy;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Settings|Bloom")
		FPPBloomSettings SimpleBloom;

	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = "Settings|Bloom")
		FPPBloomSettings FancyBloom;

	FFRPostProcessSettings() : bStartFancy(true)
	{
		Settings.bOverride_AutoExposureMethod = true;
		Settings.AutoExposureMethod = AEM_Manual;

		Settings.bOverride_AutoExposureBias = true;
		Settings.AutoExposureBias = 10.0f;
	}

	FORCEINLINE const FPostProcessSettings& operator*() const { return Settings; }
	const FPostProcessSettings& GetScaledSettings(const bool bFancyBloom);
};

UCLASS(NotBlueprintable, meta = (AllowedCategories = "Copying"))
class FAF_REV_API ASmartPostProcess final : public AActor
{
	GENERATED_BODY()

public:

	ASmartPostProcess();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		TObjectPtr<class UBoxComponent> SceneRoot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DefaultSubobjects")
		TObjectPtr<class UPostProcessComponent> PostProcess;

	/* Post process settings to use for this volume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Interp)
		FFRPostProcessSettings Settings;

	/* Post process blendable materials */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Instanced)
		TMap<FName, TObjectPtr<USmartBlendable>> Blendables;

	/* Priority of this volume. In the case of overlapping volumes the one with the highest priority
	* overrides the lower priority ones. The order is undefined if two or more overlapping volumes have the same priority
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Settings")
		float Priority;

	/* World space radius around the volume that is used for blending (only if not unbound) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Settings", Interp, meta = (ClampMin = 0.0, UIMin = 0.0, UIMax = 6000.0f))
		float BlendRadius;

	/* 0:no effect, 1:full effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Settings", Interp, meta = (UIMin = 0.0, UIMax = 1.0f))
		float BlendWeight;

	/* Whether this volume is enabled or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Settings", Interp)
		bool bEnabled;

	/* set this to false to use the parent shape component as volume bounds. True affects the whole world regardless */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Settings", Interp)
		bool bUnbound;

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
		USmartBlendable* FindSmartBlendable(const FName InName) const;
	
	UFUNCTION(BlueprintCallable, Category = "PostProcessActor", meta = (DeterminesOutputType = "InClass"))
		USmartBlendable* AddSmartBlendable(const FName InName, UPARAM(meta = (AllowAbstract = false)) const TSubclassOf<USmartBlendable> InClass);
	
	template<typename T = USmartBlendable>
	T* FindSmartBlendable() const { return Cast<T>(FindSmartBlendable(T::StaticClass())); }

	template<typename T = USmartBlendable>
	TArray<T*> AddSmartBlendable() const { return Cast<T>(AddSmartBlendable(T::StaticClass())); }
	
protected:
	
	float DistToPoint;
	void ApplySettings();
	void ApplyBlendables();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
#if WITH_EDITORONLY_DATA
private:
	UPROPERTY() TObjectPtr<UBillboardComponent> VisualIcon;
#endif
#if WITH_EDITOR	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
