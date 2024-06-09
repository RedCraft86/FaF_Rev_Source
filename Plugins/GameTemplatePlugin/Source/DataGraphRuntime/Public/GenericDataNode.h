// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "Engine/DataAsset.h"
#include "GenericDataNode.generated.h"

class UGenericDataEdge;
class UGenericDataGraph;

UCLASS(BlueprintType)
class DATAGRAPHRUNTIME_API UGenericDataNode : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UGenericDataNode();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataGraphNode")
		FGuid UniqueID;

	UPROPERTY(BlueprintReadOnly, Category = "DataGraphNode")
		TArray<TObjectPtr<UGenericDataNode>> ParentNodes;

	UPROPERTY(BlueprintReadOnly, Category = "DataGraphNode")
		TArray<TObjectPtr<UGenericDataNode>> ChildNodes;

	UPROPERTY(BlueprintReadOnly, Category = "DataGraphNode")
		TMap<TObjectPtr<UGenericDataNode>, TObjectPtr<UGenericDataEdge>> Edges;
	
	UPROPERTY() TObjectPtr<UGenericDataGraph> Graph;
	
#if WITH_EDITORONLY_DATA
protected:

	UPROPERTY(EditDefaultsOnly, Category = "DataGraphNode", AdvancedDisplay)
		FText NodeTitle;

	UPROPERTY(EditDefaultsOnly, Category = "DataGraphNode", AdvancedDisplay)
		FLinearColor NodeColor;

	UPROPERTY(EditDefaultsOnly, Category = "DataGraphNode", AdvancedDisplay, meta = (ClampMin = -1, UIMin = -1))
		int32 ChildLimit;
	
	UPROPERTY(EditDefaultsOnly, Category = "DataGraphNode", AdvancedDisplay, meta = (ClampMin = -1, UIMin = -1))
		int32 ParentLimit;

	// If enabled, allow double-click editing of the node name
	UPROPERTY(VisibleDefaultsOnly, Category = "DataGraphNode", AdvancedDisplay)
		bool bCanEditName;

public:
	
	UPROPERTY(VisibleDefaultsOnly, Category = "DataGraphNode", AdvancedDisplay)
		TSubclassOf<UGenericDataGraph> CompatibleGraph;

	UPROPERTY(VisibleDefaultsOnly, Category = "DataGraphNode", AdvancedDisplay)
		FText ContextMenuName;
#endif

	// By default, it will return the node's Unique ID.
	UFUNCTION(BlueprintPure, Category = "DataGraph|Node")
		virtual FString GetDataName() const { return UniqueID.ToString(); }

	UFUNCTION(BlueprintCallable, Category = "DataGraph|Node")
		bool IsLeafNode() const { return ChildNodes.IsEmpty(); }

	UFUNCTION(BlueprintCallable, Category = "DataGraph|Node")
		UGenericDataEdge* GetEdge(UGenericDataNode* ChildNode) const;
	
	UFUNCTION(BlueprintPure, Category = "DataGraph|Node")
		UGenericDataGraph* GetGraph() const { return Graph; }

#if WITH_EDITOR
	virtual bool CanEditName() const { return bCanEditName; }
	virtual FText GetNodeTitle() const { return NodeTitle; }
	virtual void SetNodeTitle(const FText& InTitle) { NodeTitle = InTitle; }
	virtual FLinearColor GetBackgroundColor() const { return NodeColor; }
	virtual bool CanCreateConnection(UGenericDataNode* Other, FString& ErrorMessage);
	virtual bool CanCreateConnectionTo(UGenericDataNode* Other, int32 NumOfChildren, FString& ErrorMessage);
	virtual bool CanCreateConnectionFrom(UGenericDataNode* Other, int32 NumOfParents, FString& ErrorMessage);
#endif
};
