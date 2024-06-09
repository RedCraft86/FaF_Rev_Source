// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "ExecEnums.h"
#include "Engine/GameInstance.h"
#include "GTGameInstance.generated.h"

UCLASS()
class GTRUNTIME_API UGTGameInstance : public UGameInstance
{
	GENERATED_BODY()

	friend class AGTGameModeBase;

public:

	UGTGameInstance() : CachedVMI(VMI_Lit) {}

	UFUNCTION(BlueprintImplementableEvent)
		void WorldBeginPlay();

	UFUNCTION(BlueprintImplementableEvent)
		void WorldTick(const float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
		void SetUnlitViewMode(const bool bUnlit);

	UFUNCTION(BlueprintPure, Category = "GameInstance")
		bool IsUnlitViewMode() const { return CachedVMI == VMI_Unlit; }
	
protected:
	
	EViewModeIndex CachedVMI;
	
	virtual void OnWorldBeginPlay(UWorld* InWorld);
	virtual void OnWorldTick(const float DeltaTime);

	virtual void Init() override;

public: // Statics
	
	UFUNCTION(BlueprintPure, Category = "Game", DisplayName = "Get Game Instance (Smart)", meta = (DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", WorldContext = "WorldContextObject", AdvancedDisplay = "Class"))
		static UGTGameInstance* GetGameInstanceSmart(const UObject* WorldContextObject, const TSubclassOf<UGTGameInstance> Class);

	UFUNCTION(BlueprintCallable, Category = "Game", DisplayName = "Get Game Instance (Checked)", meta = (DynamicOutputParam = "OutObject", DeterminesOutputType = "Class", ExpandEnumAsExecs = "ReturnValue", WorldContext = "WorldContextObject"))
		static EGTValidPins GetGameInstanceChecked(UGTGameInstance*& OutObject, const UObject* WorldContextObject, const TSubclassOf<UGTGameInstance> Class);

	template <typename T = UGTGameInstance>
	static T* Get(const UObject* WorldContextObject)
	{
		return Cast<T>(GetGameInstanceSmart(WorldContextObject, T::StaticClass()));
	}
};
