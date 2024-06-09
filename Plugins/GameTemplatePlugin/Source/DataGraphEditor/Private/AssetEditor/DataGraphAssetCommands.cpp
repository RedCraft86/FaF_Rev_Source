// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "DataGraphAssetCommands.h"

#define LOCTEXT_NAMESPACE "DataGraphEditor"

void FDataGraphAssetCommands::RegisterCommands()
{
	UI_COMMAND(AutoArrange, "Auto Arrange", "Auto Arrange", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
