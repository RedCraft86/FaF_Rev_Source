// Copyright (C) RedCraft86. All Rights Reserved.

#include "Tools/TodoTaskListTool.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"

void FTodoTaskListTool::ExecuteAction()
{
	if (UEditorUtilitySubsystem* EditorUtility = GEditor ? GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>() : nullptr)
	{
		if (UEditorUtilityWidgetBlueprint* WidgetBlueprint = LoadObject<UEditorUtilityWidgetBlueprint>(nullptr,
			TEXT("/GameTemplatePlugin/EditorUtility/TodoTaskList.TodoTaskList")))
		{
			Widget = Cast<UGTTodoTaskListWidget>(EditorUtility->SpawnAndRegisterTab(WidgetBlueprint));
		}
	}

	if (IsValid(Widget))
	{
		FString PathL, PathR, URL = TEXT("https://trello.com/");
		FPaths::GetProjectFilePath().Split(TEXT("/"), &PathL, &PathR, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		GConfig->GetString(TEXT("/Script/GTEditor.GTEditorSettings"), TEXT("TodoTaskBoardURL"),
			URL, PathL / TEXT("Config/DefaultEditor.ini"));
					
		Widget->LoadURL(URL);
	}
}
