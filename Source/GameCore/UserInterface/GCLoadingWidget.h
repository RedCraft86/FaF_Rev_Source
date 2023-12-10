// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GCUserWidget.h"
#include "GameSequence/GCSequenceTypes.h"
#include "GCLoadingWidget.generated.h"

UCLASS(Abstract, DisplayName = "Loading Widget Base")
class GAMECORE_API UGCLoadingWidget : public UGCUserWidget
{
	GENERATED_BODY()
	
public:

	UGCLoadingWidget(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UImage* BackgroundImage;

	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* SequenceLabel;

	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* TextLabel;
	
	UPROPERTY(Transient, meta = (BindWidgetOptional))
		class URichTextBlock* TextDesc;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UProgressBar* LoadingBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LoadingWidget")
		UTexture2D* DefaultBackgroundImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LoadingWidget")
		TMap<FString, FText> LoadingTexts;

	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction = true))
		void SetTextDesc(const FText& InText);
	
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "LoadingWidget")
		UTexture2D* GetBackgroundImage(FGCSequenceData& Data);
	UTexture2D* GetBackgroundImage_Implementation(FGCSequenceData& Data);

protected:

	UPROPERTY(Transient, BlueprintReadOnly)
		class UGCSequenceManager* SequenceManager;

	UPROPERTY(Transient)
		TSoftObjectPtr<UTexture2D> BackgroundImg;

	float RandomMax;
	float TargetPercent;
	bool bLoadInProgress;
	FTimerHandle ProgressTimer;

	void UpdateProgress();
	void OnSequenceChangeStart(const FName& ID);
	void OnSequenceChangeFinish(const FName& ID);
	virtual void InitData(AGCPlayerController* Controller) override;
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime) override;
};
