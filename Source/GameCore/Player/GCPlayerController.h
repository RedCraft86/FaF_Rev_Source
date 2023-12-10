// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "GCPlayerController.generated.h"

class UDialogue;

UCLASS(Abstract, DisplayName = "Main Player Controller")
class GAMECORE_API AGCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AGCPlayerController();

	static AGCPlayerController* Get(const UObject* WorldContext);

protected:
	
	UPROPERTY(/*VisibleDefaultsOnly, */BlueprintReadOnly, Category = "DefaultObjects")
		class UGCAchievementManager* AchievementManager;

	UPROPERTY(/*VisibleDefaultsOnly, */BlueprintReadOnly, Category = "DefaultObjects")
		class UGCNarrativeComponent* NarrativeComponent;
	
	UPROPERTY(/*VisibleDefaultsOnly, */BlueprintReadOnly, Category = "DefaultObjects")
		class UGCInventoryManager* InventoryManager;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
		TSet<TSubclassOf<class UGCUserWidget>> WidgetClasses;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TSubclassOf<class UGCFrameRateWidget> FrameRateWidgetClass;

public:

	UFUNCTION(BlueprintPure, Category = "PlayerController")
		FORCEINLINE UGCAchievementManager* GetAchievementManager() const { return AchievementManager; }

	UFUNCTION(BlueprintPure, Category = "PlayerController")
		FORCEINLINE UGCNarrativeComponent* GetNarrativeComponent() const { return NarrativeComponent; }

	UFUNCTION(BlueprintPure, Category = "PlayerController")
		FORCEINLINE UGCInventoryManager* GetInventoryManager() const { return InventoryManager; }
	
	UFUNCTION(BlueprintPure, Category = "PlayerController")
		FORCEINLINE class AGCPlayerCharacter* GetPlayer() const { return PlayerCharacter; }

	UFUNCTION(BlueprintPure, Category = "PlayerController")
		FORCEINLINE class AGCPhotoModeActor* GetPhotoModeActor() const { return PhotoModeActor; }

	UFUNCTION(BlueprintPure, Category = "PlayerController")
		FORCEINLINE class AGCInspectionActor* GetInspectionActor() const { return InspectionActor; }

	UFUNCTION(BlueprintPure, Category = "PlayerController", meta = (DeterminesOutputType = "WidgetClass"))
		UGCUserWidget* GetUserWidget(UPARAM(meta = (AllowAbstract = false)) const TSubclassOf<UGCUserWidget> WidgetClass) const;

	UFUNCTION(BlueprintPure, Category = "PlayerController")
		FORCEINLINE bool IsInInventory() const { return bInInventory; }

	UFUNCTION(BlueprintCallable, Category = "PlayerController")
		void ToggleQuestsHidden();
	
	UFUNCTION(BlueprintCallable, Category = "PlayerController")
		void EnterCutscene();

	UFUNCTION(BlueprintCallable, Category = "PlayerController")
		void ExitCutscene();
	
	UFUNCTION(BlueprintCallable, Category = "PlayerController")
		void OpenInventory();

	UFUNCTION(BlueprintCallable, Category = "PlayerController")
		void CloseInventory();
	
	UFUNCTION(BlueprintCallable, Category = "PlayerController")
		void PauseGame();

	UFUNCTION(BlueprintCallable, Category = "PlayerController")
		void UnpauseGame();
	
	template<typename T = UGCUserWidget>
	FORCEINLINE T* GetUserWidget() const
	{
		return Cast<T>(GetUserWidget(T::StaticClass()));
	}
	
	virtual void Tick(float DeltaTime) override;
	class UEnhancedInputLocalPlayerSubsystem* GetInputSubsystem() const;

protected:

	UPROPERTY(Transient)
		class UGCGameInstance* GameInstance;
	
	UPROPERTY(Transient)
		TSet<UGCUserWidget*> WidgetObjects;

	UPROPERTY(Transient)
		UGCFrameRateWidget* FrameRateWidget;
	
	UPROPERTY(Transient)
		AGCPlayerCharacter* PlayerCharacter;

	UPROPERTY(Transient)
		AGCPhotoModeActor* PhotoModeActor;

	UPROPERTY(Transient)
		AGCInspectionActor* InspectionActor;
	
	bool bInScriptedEvent;
	bool bInInventory;

	virtual void BeginPlay() override;

	void ExitScriptedEvent();
	void EnterScriptedEvent(const bool bHidePlayer);
	void ExitInventoryInternal(const TFunction<void()>& OnFinished);

	void OnItemEquipped(AActor* Actor);
	void OnItemUnequipped(AActor* Actor);
	void OnDynamicSettingsApply(class UGCUserSettings* InUserSettings);

	UFUNCTION()
		void OnDialogueStarted(UDialogue* Dialogue);
	
	UFUNCTION()
		void OnDialogueFinished(UDialogue* Dialogue);
};
