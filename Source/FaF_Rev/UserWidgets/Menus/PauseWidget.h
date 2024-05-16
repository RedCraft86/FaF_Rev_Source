// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "WidgetInterface.h"
#include "PauseWidget.generated.h"

class UFRAnimatedButtonBase;

UCLASS(Abstract)
class FAF_REV_API UPauseWidgetBase final : public UGTUserWidget, public IWidgetInterface
{
	GENERATED_BODY()
	
public:

	UPauseWidgetBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> ResumeButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> SettingsButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> CheckpointButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> MainMenuButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UTextBlock> GameVersionText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ScreenFadeAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PauseWidget")
		TSoftObjectPtr<UWorld> MainMenuLevel;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PauseWidget")
		TSubclassOf<class USettingsWidgetBase> SettingsWidgetClass;

	virtual void RemoveWidget(const TFunction<void()>& OnFinished) override;
	
protected:

	UPROPERTY(Transient) TObjectPtr<class AFRPlayerBase> PlayerChar;
	UPROPERTY(Transient) TObjectPtr<USettingsWidgetBase> SettingsWidget;
	
	void OnResumeClicked();
	void OnSettingsClicked();
	void OnCheckpointClicked();
	void OnMainMenuClicked();
	
	void FadeScreen(const TFunction<void()>& Callback);
	virtual void Return_Implementation(UUserWidget* From) override;
	virtual void NativeConstruct() override;
	virtual void InitWidget() override;
};
