// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExecEnums.h"
#include "InputModeData.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameModeBase.h"
#include "GameMusic/MusicManagerBase.h"
#include "GTGameMode.generated.h"

class UGTUserWidget;

UCLASS()
class GTRUNTIME_API AGTGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	AGTGameModeBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = "Classes")
		TSubclassOf<AMusicManagerBase> MusicManagerClass;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TArray<TSubclassOf<UGTUserWidget>> DefaultWidgets;

	UPROPERTY(Transient)
		TObjectPtr<class UGTGameInstance> GameInstance;

	UFUNCTION(BlueprintCallable, Category = "Input", meta = (AdvancedDisplay = "MouseLock, bHideCursorOnCapture, FocusWidget"))
		void SetGameInputMode(const EGameInputMode InputMode, const bool bShowMouseCursor = false, const EMouseLockMode MouseLock = EMouseLockMode::LockAlways, const bool bHideCursorOnCapture = true, UUserWidget* FocusWidget = nullptr);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
		void SetInputModeData(const FGameInputModeData& InputMode);

	UFUNCTION(BlueprintPure, Category = "Input")
		const FGameInputModeData& GetInputModeData() const { return InputModeData; }

	UFUNCTION(BlueprintCallable, Category = "GameMode", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class"))
		UGTUserWidget* FindOrAddWidget(const TSubclassOf<UGTUserWidget> Class, const FGameplayTagContainer InTags);
	
	UFUNCTION(BlueprintPure, Category = "GameMode", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class"))
		UGTUserWidget* GetWidget(const TSubclassOf<UGTUserWidget> Class, const FGameplayTag FilterTag);
	
	template<typename T>
	T* GetWidget(const FGameplayTag FilterTag = {})
	{
		return Cast<T>(GetWidget(T::StaticClass(), FilterTag));
	}
	
	template<typename T = AMusicManagerBase>
	T* GetMusicManager()
	{
		static_assert(TIsDerivedFrom<T, AMusicManagerBase>::IsDerived, "Type is not an AMusicManager class");
		return Cast<T>(MusicManager);
	}

protected:

	UPROPERTY(Transient) TObjectPtr<AMusicManagerBase> MusicManager;
	UPROPERTY(Transient) TSet<TObjectPtr<UGTUserWidget>> WidgetObjs;
	
	FGameInputModeData InputModeData;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PreInitializeComponents() override;
	void Tick(float DeltaSeconds) override;

public: // Statics
	
	UFUNCTION(BlueprintPure, Category = "Game", DisplayName = "Get Game Mode (Smart)", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", WorldContext = "WorldContextObject", AdvancedDisplay = "Class"))
		static AGTGameModeBase* GetGameModeSmart(const UObject* WorldContextObject, const TSubclassOf<AGTGameModeBase> Class);
	
	UFUNCTION(BlueprintCallable, Category = "Game", DisplayName = "Get Game Mode (Checked)", meta = (DynamicOutputParam = "OutObject", DeterminesOutputType = "Class", ExpandEnumAsExecs = "ReturnValue", WorldContext = "WorldContextObject"))
		static EGTValidPins GetGameModeChecked(AGTGameModeBase*& OutObject, const UObject* WorldContextObject, const TSubclassOf<AGTGameModeBase> Class);

	UFUNCTION(BlueprintCallable, Category = "Game", meta = (WorldContext = "WorldContextObject", DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", ExpandEnumAsExecs = "ReturnValue"))
		static EGTFoundPins GetSmartWidget(UGTUserWidget*& FoundWidget, const UObject* WorldContextObject, UPARAM(meta = (AllowAbstract = false)) const TSubclassOf<UGTUserWidget> Class, const FGameplayTag FilterTag);
	
	template <typename T = AGTGameModeBase>
	static T* Get(const UObject* WorldContextObject)
	{
		return Cast<T>(GetGameModeSmart(WorldContextObject, T::StaticClass()));
	}
};
