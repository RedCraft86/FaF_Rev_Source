// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "UserInterface/GCUserWidget.h"
#include "GCPauseWidget.generated.h"

class UGCAnimatedButton;

UCLASS(Abstract, DisplayName = "Pause Widget Base")
class GAMECORE_API UGCPauseWidget : public UGCUserWidget
{
	GENERATED_BODY()
	
public:

	UGCPauseWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		UGCAnimatedButton* ResumeButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UGCAnimatedButton* SettingsButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UGCAnimatedButton* PhotoModeButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UGCAnimatedButton* BugReportButton;

	UPROPERTY(Transient, meta = (BindWidget))
		UGCAnimatedButton* CheckpointButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UGCAnimatedButton* MainMenuButton;

	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* GameVersionText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ScreenFadeAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PauseWidget")
		TSoftObjectPtr<UWorld> MainMenuLevel;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PauseWidget")
		TSubclassOf<class UGCSettingsWidget> SettingsWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PauseWidget")
		TSubclassOf<class UGCPhotoModeWidget> PhotoModeWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PauseWidget")
		TSubclassOf<class UGCBugReportWidget> BugReportWidgetClass;
	
protected:

	UPROPERTY(Transient)
		UGCSettingsWidget* SettingsWidget;

	UPROPERTY(Transient)
		UGCPhotoModeWidget* PhotoModeWidget;

	UPROPERTY(Transient)
		UGCBugReportWidget* BugReportWidget;

	void FadeScreen(const TFunction<void()>& Callback);
	virtual void InitData(AGCPlayerController* Controller) override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	void OnExitExternalMenu();
	void OnResumeClicked();
	void OnSettingsClicked();
	void OnPhotoModeClicked();
	void OnBugReportClicked();
	void OnCheckpointClicked();
	void OnMainMenuClicked();
};
