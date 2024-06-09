// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "Engine/DataAsset.h"
#include "GenericDataEdge.generated.h"

class UGenericDataNode;
class UGenericDataGraph;

UCLASS(BlueprintType)
class DATAGRAPHRUNTIME_API UGenericDataEdge : public UDataAsset
{
	GENERATED_BODY()

public:

	UGenericDataEdge();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataGraphEdge")
		FGuid UniqueID;

	UPROPERTY(BlueprintReadOnly, Category = "DataGraphEdge")
		TObjectPtr<UGenericDataNode> StartNode;

	UPROPERTY(BlueprintReadOnly, Category = "DataGraphEdge")
		TObjectPtr<UGenericDataNode> EndNode;
	
	UPROPERTY() TObjectPtr<UGenericDataGraph> Graph;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "DataGraphEdge", AdvancedDisplay)
		bool bShouldDrawTitle;
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "DataGraphEdge", AdvancedDisplay)
		FText NodeTitle;

	UPROPERTY(EditDefaultsOnly, Category = "DataGraphEdge", AdvancedDisplay)
		FLinearColor EdgeColor;

public:
#endif

	// By default, it will return the edge's Unique ID.
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "DataGraph|Edge")
		FString GetEdgeName() const;
	virtual FString GetEdgeName_Implementation() const;
	
	UFUNCTION(BlueprintPure, Category = "DataGraph|Edge")
		UGenericDataGraph* GetGraph() const { return Graph; }

#if WITH_EDITOR
	virtual FText GetNodeTitle() const { return NodeTitle; }
	virtual void SetNodeTitle(const FText& InTitle) { NodeTitle = InTitle; }
	virtual FLinearColor GetEdgeColor() const { return EdgeColor; }
#endif
};
