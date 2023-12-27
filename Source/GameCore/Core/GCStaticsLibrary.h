// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GCStaticsLibrary.generated.h"

UCLASS(DisplayName = "Game Core Statics")
class GAMECORE_API UGCStaticsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "GameCoreStatics")
		static class UGCSettings* GetGameCoreSettings();

	UFUNCTION(BlueprintPure, Category = "GameCoreStatics")
		static class UGCUserSettings* GetUserSettings();

	UFUNCTION(BlueprintPure, Category = "GameCoreStatics", meta = (WorldContext = "WorldContextObject"))
		static class UGCGameInstance* GetMainGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "GameCoreStatics", meta = (WorldContext = "WorldContextObject"))
		static class AGCGameMode* GetMainGameMode(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "GameCoreStatics", meta = (WorldContext = "WorldContextObject"))
		static class AGCPlayerController* GetMainPlayerController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "GameCoreStatics", meta = (WorldContext = "WorldContextObject"))
		static class AGCPlayerCharacter* GetMainPlayerCharacter(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "GameCoreStatics", meta = (WorldContext = "WorldContextObject"))
		static class AGCTeleportTarget* FindTeleporter(const UObject* WorldContextObject, const FGameplayTag InTag);
};
