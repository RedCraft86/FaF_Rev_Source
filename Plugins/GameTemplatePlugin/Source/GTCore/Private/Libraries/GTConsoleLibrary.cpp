// Copyright (C) RedCraft86. All Rights Reserved.

#include "Libraries/GTConsoleLibrary.h"

TMap<FString, IConsoleVariable*> UGTConsoleLibrary::CachedCVars = {};

void UGTConsoleLibrary::SetConsoleVariableBool(const FString& InName, const bool InValue)
{
	if (IConsoleVariable* CVar = FindCVar(*InName))
	{
		CVar->Set(InValue);
	}
}

void UGTConsoleLibrary::SetConsoleVariableInt(const FString& InName, const int32 InValue)
{
	if (IConsoleVariable* CVar = FindCVar(*InName))
	{
		CVar->Set(InValue);
	}
}

void UGTConsoleLibrary::SetConsoleVariableFloat(const FString& InName, const float InValue)
{
	if (IConsoleVariable* CVar = FindCVar(*InName))
	{
		CVar->Set(InValue);
	}
}

void UGTConsoleLibrary::SetConsoleVariableString(const FString& InName, const FString& InValue)
{
	if (IConsoleVariable* CVar = FindCVar(*InName))
	{
		CVar->Set(*InValue);
	}
}

bool UGTConsoleLibrary::GetConsoleVariableBool(const FString& InName)
{
	if (const IConsoleVariable* CVar = FindCVar(*InName))
	{
		return CVar->GetBool();
	}

	return false;
}

int32 UGTConsoleLibrary::GetConsoleVariableInt(const FString& InName)
{
	if (const IConsoleVariable* CVar = FindCVar(*InName))
	{
		return CVar->GetInt();
	}

	return 0;
}

float UGTConsoleLibrary::GetConsoleVariableFloat(const FString& InName)
{
	if (const IConsoleVariable* CVar = FindCVar(*InName))
	{
		return CVar->GetFloat();
	}

	return 0.0f;
}

FString UGTConsoleLibrary::GetConsoleVariableString(const FString& InName)
{
	if (const IConsoleVariable* CVar = FindCVar(*InName))
	{
		return CVar->GetString();
	}

	return TEXT("");
}

IConsoleVariable* UGTConsoleLibrary::FindCVar(const FString& InName)
{
	IConsoleVariable** CVar = CachedCVars.Find(InName);
	if (CVar && *CVar)
	{
		return *CVar;
	}

	if (IConsoleVariable* NewCVar = IConsoleManager::Get().FindConsoleVariable(*InName))
	{
		CachedCVars.Add(InName, NewCVar);
		return NewCVar;
	}

	return nullptr;
}
