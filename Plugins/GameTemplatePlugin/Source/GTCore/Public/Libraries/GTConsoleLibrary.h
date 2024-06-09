// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GTConsoleLibrary.generated.h"

UCLASS(DisplayName = "Console Library")
class GTCORE_API UGTConsoleLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "ConsoleVariables", DisplayName = "Set Console Variable (Bool)")
		static void SetConsoleVariableBool(const FString& InName, const bool InValue);

	UFUNCTION(BlueprintCallable, Category = "ConsoleVariables", DisplayName = "Set Console Variable (Int)")
		static void SetConsoleVariableInt(const FString& InName, const int32 InValue);

	UFUNCTION(BlueprintCallable, Category = "ConsoleVariables", DisplayName = "Set Console Variable (float)")
		static void SetConsoleVariableFloat(const FString& InName, const float InValue);

	UFUNCTION(BlueprintCallable, Category = "ConsoleVariables", DisplayName = "Set Console Variable (String)")
		static void SetConsoleVariableString(const FString& InName, const FString& InValue);
	
	UFUNCTION(BlueprintPure, Category = "ConsoleVariables", DisplayName = "Get Console Variable (Bool)")
		static bool GetConsoleVariableBool(const FString& InName);

	UFUNCTION(BlueprintPure, Category = "ConsoleVariables", DisplayName = "Get Console Variable (Int)")
		static int32 GetConsoleVariableInt(const FString& InName);

	UFUNCTION(BlueprintPure, Category = "ConsoleVariables", DisplayName = "Get Console Variable (float)")
		static float GetConsoleVariableFloat(const FString& InName);

	UFUNCTION(BlueprintPure, Category = "ConsoleVariables", DisplayName = "Get Console Variable (String)")
		static FString GetConsoleVariableString(const FString& InName);
	
	static IConsoleVariable* FindCVar(const FString& InName);
	
private:
	
	static TMap<FString, IConsoleVariable*> CachedCVars;
};
