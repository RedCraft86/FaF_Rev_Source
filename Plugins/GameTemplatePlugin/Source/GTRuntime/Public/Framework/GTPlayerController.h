// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExecEnums.h"
#include "GameFramework/PlayerController.h"
#include "GTPlayerController.generated.h"

UCLASS()
class GTRUNTIME_API AGTPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	
	AGTPlayerController();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<USceneComponent> SceneRoot;

	UFUNCTION(BlueprintPure, Category = "Game", DisplayName = "Get Player Controller (Smart)", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", WorldContext = "WorldContextObject", AdvancedDisplay = "Class"))
		static AGTPlayerController* GetPlayerControllerSmart(const UObject* WorldContextObject, const TSubclassOf<AGTPlayerController> Class);

	UFUNCTION(BlueprintCallable, Category = "Game", DisplayName = "Get Player Controller (Checked)", meta = (DynamicOutputParam = "OutObject", DeterminesOutputType = "Class", ExpandEnumAsExecs = "ReturnValue", WorldContext = "WorldContextObject"))
		static EGTValidPins GetPlayerControllerChecked(AGTPlayerController*& OutObject, const UObject* WorldContextObject, const TSubclassOf<AGTPlayerController> Class);

	template <typename T = AGTPlayerController>
	static T* Get(const UObject* WorldContextObject)
	{
		return Cast<T>(GetPlayerControllerSmart(WorldContextObject, T::StaticClass()));
	}
};
