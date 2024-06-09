// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "EditorSubsystem.h"
#include "GTEditorSubsystem.generated.h"

UCLASS()
class GTEDITOR_API UGTEditorSubsystem final : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	
	static UGTEditorSubsystem* Get()
	{
		return GEditor ? GEditor->GetEditorSubsystem<UGTEditorSubsystem>() : nullptr;
	}

	void RunCommand(const FString& Command) const;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
