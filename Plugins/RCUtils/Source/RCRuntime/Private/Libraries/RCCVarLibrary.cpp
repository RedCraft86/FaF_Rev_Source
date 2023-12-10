// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "RCCVarLibrary.h"

TMap<FString, IConsoleVariable*> URCCVarLibrary::CachedCVars = {};

void URCCVarLibrary::SetConsoleVariableBool(const FString& InName, const bool InValue)
{
	if (IConsoleVariable* CVar = FindCVar(InName))
	{
		CVar->Set(InValue);
	}
}

void URCCVarLibrary::SetConsoleVariableInt(const FString& InName, const int32 InValue)
{
	if (IConsoleVariable* CVar = FindCVar(InName))
	{
		CVar->Set(InValue);
	}
}

void URCCVarLibrary::SetConsoleVariableFloat(const FString& InName, const float InValue)
{
	if (IConsoleVariable* CVar = FindCVar(InName))
	{
		CVar->Set(InValue);
	}
}

void URCCVarLibrary::SetConsoleVariableString(const FString& InName, const FString& InValue)
{
	if (IConsoleVariable* CVar = FindCVar(InName))
	{
		CVar->Set(*InValue);
	}
}

bool URCCVarLibrary::GetConsoleVariableBool(const FString& InName)
{
	if (const IConsoleVariable* CVar = FindCVar(InName))
	{
		return CVar->GetBool();
	}

	return false;
}

int32 URCCVarLibrary::GetConsoleVariableInt(const FString& InName)
{
	if (const IConsoleVariable* CVar = FindCVar(InName))
	{
		return CVar->GetInt();
	}

	return 0;
}

float URCCVarLibrary::GetConsoleVariableFloat(const FString& InName)
{
	if (const IConsoleVariable* CVar = FindCVar(InName))
	{
		return CVar->GetFloat();
	}

	return 0.0f;
}

FString URCCVarLibrary::GetConsoleVariableString(const FString& InName)
{
	if (const IConsoleVariable* CVar = FindCVar(InName))
	{
		return CVar->GetString();
	}

	return TEXT("");
}

IConsoleVariable* URCCVarLibrary::FindCVar(const FString& InName)
{
	IConsoleVariable** FoundCVarPtr = CachedCVars.Find(InName);
	if (FoundCVarPtr && *FoundCVarPtr)
	{
		return *FoundCVarPtr;
	}
	
	if (IConsoleVariable* FoundCVar = IConsoleManager::Get().FindConsoleVariable(*InName))
	{
		CachedCVars.Add(InName, FoundCVar);
		return FoundCVar;
	}

	return nullptr;
}
