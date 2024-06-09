// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameSectionNode.h"
#include "GenericDataGraph.h"
#include "GameSectionGraph.generated.h"

UCLASS()
class GTRUNTIME_API UGameSectionGraph final : public UGenericDataGraph
{
	GENERATED_BODY()

public:

	UGameSectionGraph();

	static inline TSubclassOf<UGameSectionNode> GraphNodeType = UGameSectionNode::StaticClass();

	virtual TSubclassOf<UGenericDataNode> GetNodeType() const override { return GraphNodeType; }
	
#if WITH_EDITOR
protected:
	bool CanEditChange(const FProperty* InProperty) const override
	{
		return Super::CanEditChange(InProperty) && !(
			InProperty->GetName() == GET_MEMBER_NAME_CHECKED(UGenericDataGraph, EdgeType)
			|| InProperty->GetName() == GET_MEMBER_NAME_CHECKED(UGenericDataGraph, NodeType)
			|| InProperty->GetName() == GET_MEMBER_NAME_CHECKED(UGenericDataGraph, bCanBeCyclical)
		);
	}
#endif
};
