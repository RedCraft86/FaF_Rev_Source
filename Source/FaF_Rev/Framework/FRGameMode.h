// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTGameMode.h"
#include "FRGameMode.generated.h"

#define FRGamemode(Context) AGTGameMode::Get<AFRGameMode>(Context)

UCLASS()
class FAF_REV_API AFRGameMode final : public AGTGameMode
{
	GENERATED_BODY()

public:

	AFRGameMode();

	UPROPERTY(EditAnywhere, Category = "Settings")
		TSet<TSubclassOf<UGTUserWidget>> DefaultWidgets;

	UFUNCTION(BlueprintCallable, Category = "GameMode", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class"))
		UGTUserWidget* FindOrAddWidget(const TSubclassOf<UGTUserWidget> Class, const FGameplayTagContainer Tags);
	
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
