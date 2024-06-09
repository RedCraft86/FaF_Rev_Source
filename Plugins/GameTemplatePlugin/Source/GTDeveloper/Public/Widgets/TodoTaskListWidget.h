// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "WebBrowser.h"
#include "EditorUtilityWidget.h"
#include "TodoTaskListWidget.generated.h"

UCLASS(Abstract)
class GTDEVELOPER_API UGTTodoTaskListWidget final : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:

	void LoadURL(const FString& URL) const
	{
		if(WebBrowserWidget) WebBrowserWidget->LoadURL(URL);
	}
	
private:

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UWebBrowser> WebBrowserWidget = nullptr;
};
