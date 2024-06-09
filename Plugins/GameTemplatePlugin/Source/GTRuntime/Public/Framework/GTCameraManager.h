// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExecEnums.h"
#include "Camera/PlayerCameraManager.h"
#include "GTCameraManager.generated.h"

UCLASS()
class GTRUNTIME_API AGTCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	AGTCameraManager();
	
protected:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public: // Statics
	
	UFUNCTION(BlueprintPure, Category = "Game", DisplayName = "Get Camera Manager (Smart)", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", WorldContext = "WorldContextObject", AdvancedDisplay = "Class"))
		static AGTCameraManager* GetCameraManagerSmart(const UObject* WorldContextObject, const TSubclassOf<AGTCameraManager> Class);

	UFUNCTION(BlueprintCallable, Category = "Game", DisplayName = "Get Camera Manager (Checked)", meta = (DynamicOutputParam = "OutObject", DeterminesOutputType = "Class", ExpandEnumAsExecs = "ReturnValue", WorldContext = "WorldContextObject"))
		static EGTValidPins GetCameraManagerChecked(AGTCameraManager*& OutObject, const UObject* WorldContextObject, const TSubclassOf<AGTCameraManager> Class);
	
	template <typename T = AGTCameraManager>
	static T* Get(const UObject* WorldContextObject)
	{
		return Cast<T>(GetCameraManagerSmart(WorldContextObject, T::StaticClass()));
	}
};
