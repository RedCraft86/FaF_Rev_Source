// Copyright (C) RedCraft86. All Rights Reserved.

#include "Tools/ChannelPackerTool.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"

void FChannelPackerTool::ExecuteAction()
{
	if (UEditorUtilitySubsystem* EditorUtility = GEditor ? GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>() : nullptr)
	{
		if (UEditorUtilityWidgetBlueprint* WidgetBlueprint = LoadObject<UEditorUtilityWidgetBlueprint>(nullptr,
			TEXT("/GameTemplatePlugin/EditorUtility/ChannelPacker.ChannelPacker")))
		{
			Widget = Cast<UGTChannelPackerWidget>(EditorUtility->SpawnAndRegisterTab(WidgetBlueprint));
		}
	}

	if (IsValid(Widget))
	{
		Widget->Update();
	}
}
