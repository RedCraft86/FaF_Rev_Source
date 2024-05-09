// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "PhotoModeWidget.generated.h"

class APhotoModeActor;
class USpinBox;
class UButton;

UCLASS(Abstract)
class FAF_REV_API UPhotoModeWidgetBase final : public UGTUserWidget
{
	GENERATED_BODY()

public:

	UPhotoModeWidgetBase(const FObjectInitializer& ObjectInitializer);

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
		class UImage* CapturedImage;
	
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
		UWidgetAnimation* SidebarAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* CaptureStartAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* CaptureEndAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ReturnAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PhotoModeWidget")
		TArray<FName> FilterIDs;

	UFUNCTION(BlueprintImplementableEvent)
		UTexture* GetColorGradingLUT(const FName InFilterName) const;
	
	void BeginCapture();
	void SetPhotoModeActor(APhotoModeActor* InActor);
	
protected:

	bool bIsSidebarOpen;
	TMap<FString, FName> FilterNamesToID;
	UPROPERTY(Transient) UUserWidget* ParentWidget;
	UPROPERTY(Transient) UTexture2D* CapturedTexture;
	UPROPERTY(Transient) APhotoModeActor* PhotoModeActor;
	UPROPERTY(Transient) class AFRPlayerController* Controller;
	UPROPERTY(Transient) class AFRPlayerBase* PlayerChar;
	
	UFUNCTION() void OnGameTitleCheckChanged(bool bValue);
	UFUNCTION() void OnResScaleChanged(float Value);
	UFUNCTION() void OnFocalDistChanged(float Value);
	UFUNCTION() void OnFieldOfViewChanged(float Value);
	UFUNCTION() void OnApertureChanged(float Value);
	UFUNCTION() void OnLUTIntensityChanged(float Value);
	UFUNCTION() void OnLUTSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnColorSplitChanged(float Value);
	UFUNCTION() void OnVignetteChanged(float Value);
	UFUNCTION() void OnLensFlareChanged(float Value);
	UFUNCTION() void OnBackClicked();
	UFUNCTION() void OnFolderClicked();
	UFUNCTION() void OnSidebarClicked();
	UFUNCTION() void OnExitClicked();

	void OnPhotoCaptured(UTexture2D* InTexture);
	virtual void NativeConstruct() override;
};
