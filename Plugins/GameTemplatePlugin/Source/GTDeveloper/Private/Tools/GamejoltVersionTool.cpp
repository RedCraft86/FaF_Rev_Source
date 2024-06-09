// Copyright (C) RedCraft86. All Rights Reserved.

#include "Tools/GamejoltVersionTool.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"

void FGamejoltVersionTool::ExecuteAction()
{
	if (UEditorUtilitySubsystem* EditorUtility = GEditor ? GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>() : nullptr)
	{
		if (UEditorUtilityWidgetBlueprint* WidgetBlueprint = LoadObject<UEditorUtilityWidgetBlueprint>(nullptr,
			TEXT("/GameTemplatePlugin/EditorUtility/GamejoltVersion.GamejoltVersion")))
		{
			Widget = Cast<UGTGamejoltVersionWidget>(EditorUtility->SpawnAndRegisterTab(WidgetBlueprint));
		}
	}
}
