// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTUserWidget.h"
#include "WidgetInterface.h"
#include "MainMenuWidget.generated.h"

class UButton;

UCLASS(Abstract)
class FAF_REV_API UMainMenuWidgetBase final : public UGTUserWidget, public IWidgetInterface
{
	GENERATED_BODY()

public:

	UMainMenuWidgetBase(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> PlayButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> SettingsButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> QuitButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> PhaseMapButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> ExtrasButton;

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

	virtual void AddWidget(const TFunction<void()>& OnFinished) override;
	virtual void RemoveWidget(const TFunction<void()>& OnFinished) override;
	
private:

	UPROPERTY(Transient) TObjectPtr<USettingsWidgetBase> SettingsWidget;
	UPROPERTY(Transient) TObjectPtr<UPhaseMapWidgetBase> PhaseMapWidget;

	UFUNCTION() void OnPlayClicked();
	UFUNCTION() void OnSettingsClicked();
	UFUNCTION() void OnQuitClicked();
	UFUNCTION() void OnPhaseMapClicked();
	UFUNCTION() void OnExtrasClicked();

	void FadeOut();
	virtual void Return_Implementation(UUserWidget* From) override;
	virtual void NativeConstruct() override;
	virtual void InitWidget() override;
};
