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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Elements", meta = (BindWidget))
		UTextBlock* LoadingLabel;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Elements", meta = (BindWidget))
		class UProgressBar* LoadingBar;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Elements", meta = (BindWidget))
		class UImage* BackgroundImage;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Elements", meta = (BindWidget))
		UTextBlock* TipLabel;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Elements", meta = (BindWidget))
		UTextBlock* TipText;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Elements")
		TSoftObjectPtr<UTexture2D> DefaultBackground;
	
private:

	UPROPERTY(Transient)
		UTexture2D* Background;

	bool bUnloading;
	int32 TotalObjs;
	TArray<FAssetData> LoadingObjs;
	FTimerHandle SlowTickHandle;
	
	void SlowTick();
	void Update(const bool bFinish) const;
	
	void FinishLoading(const TFunction<void()>& OnFinished);
	void BeginLoading(const TSet<FAssetData>& InObjects, const TSoftObjectPtr<UTexture2D>& InBackground, const TPair<FString, FText>& InTip);
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
};
