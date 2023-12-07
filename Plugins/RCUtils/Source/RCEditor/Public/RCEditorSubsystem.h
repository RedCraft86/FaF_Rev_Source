// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "EditorSubsystem.h"
#include "RCEditorSubsystem.generated.h"

UCLASS(DisplayName = "RCEditor Subsystem")
class RCEDITOR_API URCEditorSubsystem final : public UEditorSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
