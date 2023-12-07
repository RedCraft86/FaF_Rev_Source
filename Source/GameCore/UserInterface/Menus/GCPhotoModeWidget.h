// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "UserInterface/GCUserWidget.h"
#include "GCPhotoModeWidget.generated.h"

class UImage;
class UButton;
class USpinBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhotoModeBackPressedBP);

UCLASS(Abstract, DisplayName = "Photo Mode Widget Base")
class GAMECORE_API UGCPhotoModeWidget : public UGCUserWidget
{
	GENERATED_BODY()

public:

	UGCPhotoModeWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, DisplayName = "On Exit")
		FOnPhotoModeBackPressedBP OnExitBP;

	UPROPERTY(Transient, meta = (BindWidget))
		class UCheckBox* GameTitleCheckBox;

	UPROPERTY(Transient, meta = (BindWidget))
		USpinBox* ResScaleSpinBox;
	
	UPROPERTY(Transient, meta = (BindWidget))
		USpinBox* FocalDistSpinBox;

	UPROPERTY(Transient, meta = (BindWidget))
		USpinBox* FieldOfViewSpinBox;

	UPROPERTY(Transient, meta = (BindWidget))
		USpinBox* ApertureSpinBox;

	UPROPERTY(Transient, meta = (BindWidget))
		USpinBox* FilterSpinBox;

	UPROPERTY(Transient, meta = (BindWidget))
		class UComboBoxString* FilterComboBox;

	UPROPERTY(Transient, meta = (BindWidget))
		USpinBox* ColorSplitSpinBox;

	UPROPERTY(Transient, meta = (BindWidget))
		USpinBox* VignetteSpinBox;

	UPROPERTY(Transient, meta = (BindWidget))
		USpinBox* LensFlareSpinBox;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UImage* CapturedImage;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UButton* BackButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* FolderButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* FileButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* SideBarButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UButton* ExitButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ScreenSwapAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SidebarAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* CaptureStartAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* CaptureEndAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PhotoModeWidget")
		TArray<FName> FilterIDs;

	DECLARE_MULTICAST_DELEGATE(FOnBackPressed);
	FOnBackPressed OnExit;

	UFUNCTION(BlueprintImplementableEvent)
		UTexture* GetColorGradingLUT(const FName InFilterName) const;

	bool CanEnterPhotoMode() const;
	
protected:

	UPROPERTY(Transient)
		class AGCPhotoModeActor* PhotoModeActor;

	bool bIsSidebarOpen;
	float CapturedDelay;
	UTexture2D* TempImg;
	TMap<FString, FName> FilterNamesToID;

	void OnBeginCapture();
	void OnShotCaptured(UTexture2D* Image);
	virtual void InitData(AGCPlayerController* Controller) override;
	virtual void NativeTick(const FGeometry& InGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

	UFUNCTION()
		void OnGameTitleCheckChanged(bool bValue);

	UFUNCTION()
		void OnResScaleChanged(float Value);
	
	UFUNCTION()
		void OnFocalDistChanged(float Value);

	UFUNCTION()
		void OnFieldOfViewChanged(float Value);

	UFUNCTION()
		void OnApertureChanged(float Value);

	UFUNCTION()
		void OnLUTIntensityChanged(float Value);
	
	UFUNCTION()
		void OnLUTSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
		void OnColorSplitChanged(float Value);

	UFUNCTION()
		void OnVignetteChanged(float Value);

	UFUNCTION()
		void OnLensFlareChanged(float Value);
	
	UFUNCTION()
		void OnBackClicked();

	UFUNCTION()
		void OnFolderClicked();
	
	UFUNCTION()
		void OnSidebarClicked();
	
	UFUNCTION()
		void OnExitClicked();
};
