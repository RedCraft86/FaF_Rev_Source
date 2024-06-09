// Copyright (C) RedCraft86. All Rights Reserved.

#include "GameSection/GameSectionNode.h"
#include "GameSection/GameSectionGraph.h"

#define LOCTEXT_NAMESPACE "GameSection"
UGameSectionNode::UGameSectionNode()
{
	DisplayLabel = LOCTEXT("Unknown", "Unknown Section");
#if WITH_EDITORONLY_DATA
	bCanEditName = true;
	CompatibleGraph = UGameSectionGraph::StaticClass();
	NodeTitle = LOCTEXT("NodeTitle", "Game Section");
	ContextMenuName = LOCTEXT("NodeContextName", "Game Section Node");
#endif
}
#undef LOCTEXT_NAMESPACE