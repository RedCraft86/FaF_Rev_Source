// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "GenericDataEdge.h"

UGenericDataEdge::UGenericDataEdge()
{
	UniqueID = FGuid::NewGuid();
	StartNode = nullptr;
	EndNode = nullptr;
	Graph = nullptr;
#if WITH_EDITORONLY_DATA
	bShouldDrawTitle = false;
	NodeTitle = NSLOCTEXT("DataGraphRuntime", "GenericDataEdgeName", "Generic Data Edge");
	EdgeColor = FLinearColor{0.9f, 0.9f, 0.9f, 1.0f};
#endif
}

FString UGenericDataEdge::GetEdgeName_Implementation() const
{
	return UniqueID.ToString();
}
