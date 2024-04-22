// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTGameMode.h"
#include "FRGameMode.generated.h"

#define FRGamemode(Context) AGTGameMode::Get<AFRGameModeBase>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRGameModeBase final : public AGTGameMode
{
	GENERATED_BODY()

public:

	AFRGameModeBase();

	UPROPERTY(EditAnywhere, Category = "Settings")
		TMap<TSubclassOf<UGTUserWidget>, FGameplayTagContainer> DefaultWidgets;

	UFUNCTION(BlueprintCallable, Category = "GameMode", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class"))
		UGTUserWidget* FindOrAddWidget(const TSubclassOf<UGTUserWidget> Class, const FGameplayTagContainer InTags);
	
	UFUNCTION(BlueprintPure, Category = "GameMode", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class"))
		UGTUserWidget* GetWidget(const TSubclassOf<UGTUserWidget> Class, const FGameplayTag FilterTag);
	
	template<typename T>
	T* GetWidget(const FGameplayTag FilterTag = {})
	{
		return Cast<T>(GetWidget(T::StaticClass(), FilterTag));
	}

private:

	UPROPERTY(Transient) TMap<TSubclassOf<UGTUserWidget>, UGTUserWidget*> WidgetObjs;

	virtual void BeginPlay() override;
};
