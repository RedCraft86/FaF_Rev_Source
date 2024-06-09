// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "GTUserWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE(FGTWidgetCallbackSignature);

UCLASS(Abstract)
class GTRUNTIME_API UGTUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UGTUserWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animations", meta = (BindWidgetAnimOptional))
		TObjectPtr<UWidgetAnimation> FadeAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserWidget")
		int32 ZOrder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserWidget")
		bool bAutoAdd;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserWidget")
		FGameplayTagContainer WidgetTags;

	UFUNCTION(BlueprintPure, Category = "UserWidget")
		bool IsWidgetHidden() const { return bWidgetHidden; }
	
	UFUNCTION(BlueprintCallable, Category = "UserWidget")
		void SetWidgetHidden(const bool bInHidden);
	
	UFUNCTION(BlueprintCallable, Category = "UserWidget", meta = (Keywords = "add widget"))
		void AddUserWidget(FGTWidgetCallbackSignature OnFinished);

	UFUNCTION(BlueprintCallable, Category = "UserWidget", meta = (Keywords = "remove widget"))
		void RemoveUserWidget(FGTWidgetCallbackSignature OnFinished);
	
	virtual void InitWidget() {}
	virtual void AddWidget(const TFunction<void()>& OnFinished = nullptr);
	virtual void RemoveWidget(const TFunction<void()>& OnFinished = nullptr);

	template<typename T = AGameModeBase>
	T* GetGameMode() { return Cast<T>(GameMode); }
	
	template<typename T = APlayerController>
	T* GetPlayerController() { return Cast<T>(PlayerController); }

private:

	UPROPERTY(BlueprintReadOnly, Category = "UserWidget", meta = (AllowPrivateAccess = true))
		TObjectPtr<AGameModeBase> GameMode;
	
	UPROPERTY(BlueprintReadOnly, Category = "UserWidget", meta = (AllowPrivateAccess = true))
		TObjectPtr<APlayerController> PlayerController;

	bool bRemoving;
	bool bWidgetHidden;

public: // Statics

	UFUNCTION(BlueprintCallable, Category = "UserWidget", meta = (WorldContext = "ContextObject"))
		static UGTUserWidget* CreateSmartWidget(const UObject* ContextObject, APlayerController* Owner, const TSubclassOf<UGTUserWidget> Class, const FGameplayTagContainer Tags);
	template <typename T = UGTUserWidget>
	static T* CreateNew(APlayerController* Owner, const FGameplayTagContainer Tags = {})
	{
		return Cast<T>(CreateSmartWidget(nullptr, Owner, T::StaticClass(), Tags));
	}
	template <typename T = UGTUserWidget>
	static T* CreateNew(APlayerController* Owner, const TSubclassOf<T> Class, const FGameplayTagContainer Tags = {})
	{
		return Cast<T>(CreateSmartWidget(nullptr, Owner, Class, Tags));
	}
};
