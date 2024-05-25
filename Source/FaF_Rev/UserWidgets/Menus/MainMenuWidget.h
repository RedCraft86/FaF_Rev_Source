// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "WidgetInterface.h"
#include "MainMenuWidget.generated.h"

class UFRAnimatedButtonBase;

UCLASS(Abstract)
class FAF_REV_API UMainMenuWidgetBase final : public UGTUserWidget, public IWidgetInterface
{
	GENERATED_BODY()

public:

	UMainMenuWidgetBase(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> PlayButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> SettingsButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> QuitButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> PhaseMapButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UFRAnimatedButtonBase> ExtrasButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UTextBlock> GameVersionText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MainMenuWidget")
		TSoftObjectPtr<UWorld> GameplayLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MainMenuWidget")
		TSoftObjectPtr<UWorld> ExtrasLevel;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MainMenuWidget")
		TSubclassOf<class USettingsWidgetBase> SettingsWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MainMenuWidget")
		TSubclassOf<class UPhaseMapWidgetBase> PhaseMapWidgetClass;

	virtual void RemoveWidget(const TFunction<void()>& OnFinished) override;
	
private:

	UPROPERTY(Transient) TObjectPtr<USettingsWidgetBase> SettingsWidget;
	UPROPERTY(Transient) TObjectPtr<UPhaseMapWidgetBase> PhaseMapWidget;

	void OnPlayClicked();
	void OnSettingsClicked();
	void OnQuitClicked();
	void OnPhaseMapClicked();
	void OnExtrasClicked();

	void FadeOut();
	virtual void Return_Implementation(UUserWidget* From) override;
	virtual void NativeConstruct() override;
	virtual void InitWidget() override;
};
