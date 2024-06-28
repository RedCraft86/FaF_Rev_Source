// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "JsonObjectWrapper.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GTConfigSubsystem.generated.h"

UCLASS(BlueprintType, DisplayName = "Config Subsystem")
class GTCORE_API UGTConfigSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameConfig", DisplayName = "Set Game Config (Boolean)")
		void SetGameConfigBool(const FString& InKey, const bool InValue) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameConfig", DisplayName = "Set Game Config (Float)")
		void SetGameConfigFloat(const FString& InKey, const float InValue) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameConfig", DisplayName = "Set Game Config (Integer)")
		void SetGameConfigInt(const FString& InKey, const int32 InValue) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GameConfig", DisplayName = "Set Game Config (String)")
		void SetGameConfigString(const FString& InKey, const FString& InValue) const;

	UFUNCTION(BlueprintPure, Category = "GameConfig", DisplayName = "Get Game Config (Boolean)", meta = (AdvancedDisplay = "InDefault"))
		bool GetGameConfigBool(const FString& InKey, const bool InDefault = false) const;

	UFUNCTION(BlueprintPure, Category = "GameConfig", DisplayName = "Get Game Config (Float)", meta = (AdvancedDisplay = "InDefault"))
		float GetGameConfigFloat(const FString& InKey, const float InDefault = 0.0f) const;

	UFUNCTION(BlueprintPure, Category = "GameConfig", DisplayName = "Get Game Config (Integer)", meta = (AdvancedDisplay = "InDefault"))
		int32 GetGameConfigInt(const FString& InKey, const int32 InDefault = 0) const;

	UFUNCTION(BlueprintPure, Category = "GameConfig", DisplayName = "Get Game Config (String)", meta = (AdvancedDisplay = "InDefault"))
		FString GetGameConfigString(const FString& InKey, const FString& InDefault = TEXT("")) const;

	UFUNCTION(BlueprintPure, Category = "GameConfig|Defaults")
		bool IsFirstLaunch() const;
	
	UFUNCTION(BlueprintPure, Category = "GameConfig|Defaults")
		bool IsDeveloperMode() const;
	
	void LoadGameConfig();
	void SaveGameConfig() const;
	bool CheckGameConfigDefaults() const;
	void MarkFirstLaunchFalse() const;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
private:

	FJsonObjectWrapper GameConfig;

public: // Statics
	
	static UGTConfigSubsystem* Get(const UObject* WorldContext)
	{
		const UWorld* World = WorldContext ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
		const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
		return GI ? GI->GetSubsystem<UGTConfigSubsystem>() : nullptr;
	}

	static FString GetGameConfigPath()
	{
		return FPaths::ProjectSavedDir() / TEXT("game-config.json");
	}
};
