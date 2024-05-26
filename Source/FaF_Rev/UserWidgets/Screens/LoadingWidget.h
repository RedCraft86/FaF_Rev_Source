// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "LoadingWidget.generated.h"

class UTextBlock;

UCLASS(Abstract)
class FAF_REV_API ULoadingWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

	friend class UGameSectionManager;

public:

	ULoadingWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UWidgetSwitcher> SwitcherWidget;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> LoadingLabel;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UProgressBar> LoadingBar;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UImage> BackgroundImage;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> TipLabel;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UTextBlock> TipText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LoadingWidget")
		TSoftObjectPtr<UTexture2D> DefaultBackground;
	
private:
	
	bool bUnloading;
	int32 TotalObjs;
	TArray<FAssetData> LoadingObjs;
	FTimerHandle SlowTickHandle;
	UPROPERTY(Transient) TObjectPtr<UTexture2D> Background;
	
	void SlowTick();
	void Update(const bool bFinish) const;
	void SetMinimalMode(const bool bInMinimalMode) const;
	void FinishLoading(const TFunction<void()>& OnFinished);
	void BeginLoading(const TSet<FAssetData>& InObjects, const TSoftObjectPtr<UTexture2D>& InBackground, const TPair<FString, FText>& InTip);
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
};
