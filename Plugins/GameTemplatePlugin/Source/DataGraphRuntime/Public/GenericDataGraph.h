// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "Engine/DataAsset.h"
#include "GenericDataGraph.generated.h"

class UGenericDataEdge;
class UGenericDataNode;

UCLASS(BlueprintType)
class DATAGRAPHRUNTIME_API UGenericDataGraph : public UDataAsset
{
	GENERATED_BODY()

public:

	UGenericDataGraph();

	UPROPERTY(EditDefaultsOnly, Category = "DataGraph")
		TSubclassOf<UGenericDataEdge> EdgeType;

	UPROPERTY(EditDefaultsOnly, Category = "DataGraph")
		TSubclassOf<UGenericDataNode> NodeType;

	UPROPERTY(BlueprintReadOnly, Category = "DataGraph")
		TArray<TObjectPtr<UGenericDataNode>> RootNodes;

	UPROPERTY(BlueprintReadOnly, Category = "DataGraph")
		TArray<TObjectPtr<UGenericDataNode>> AllNodes;
		
	UFUNCTION(BlueprintPure, Category = "DataGraph")
		int32 GetDepth() const;
	
	UFUNCTION(BlueprintPure, Category = "DataGraph")
		int32 GetEndings() const;

	UFUNCTION(BlueprintPure, Category = "DataGraph", meta = (AutoCreateRefTerm = "InSequence"))
		TArray<uint8> ValidateSequence(const TArray<uint8>& InSequence) const;

	UFUNCTION(BlueprintPure, Category = "DataGraph", meta = (AutoCreateRefTerm = "InSequence"))
		UGenericDataNode* GetNodeBySequence(const TArray<uint8>& InSequence) const;

	UFUNCTION(BlueprintPure, Category = "DataGraph", meta = (AutoCreateRefTerm = "InSequence"))
		TSet<UGenericDataNode*> GetNodesInSequence(const TArray<uint8>& InSequence) const;

	UFUNCTION(BlueprintPure, Category = "DataGraph")
		TArray<uint8> GetSequenceFromNode(UGenericDataNode* InNode) const;

	UFUNCTION(BlueprintPure, Category = "DataGraph")
		TArray<uint8> GetSequenceFromNodeID(const FGuid& InNodeID) const;

	template<typename T>
	T* GetNodeBySequence(const TArray<uint8>& InSequence) const
	{
		return Cast<T>(GetNodeBySequence(InSequence));
	}

	void ClearGraph();
	virtual TSubclassOf<UGenericDataEdge> GetEdgeType() const { return EdgeType; }
	virtual TSubclassOf<UGenericDataNode> GetNodeType() const { return NodeType; }

#if WITH_EDITORONLY_DATA
	UPROPERTY()
		TObjectPtr<UEdGraph> EdGraph;

	// If enabled, nodes can cycle back to any of the parent nodes in the chain.
	UPROPERTY(EditAnywhere, Category = "DataGraph", AdvancedDisplay)
		bool bCanBeCyclical;
#endif
};
