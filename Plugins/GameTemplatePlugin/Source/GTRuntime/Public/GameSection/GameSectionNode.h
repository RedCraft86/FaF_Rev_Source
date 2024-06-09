// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GenericDataNode.h"
#include "GameSectionNode.generated.h"

UCLASS(NotBlueprintable)
class GTRUNTIME_API UGameSectionNode : public UGenericDataNode
{
	GENERATED_BODY()

public:

	UGameSectionNode();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SectionData", meta = (DisplayPriority = -1))
		FText DisplayLabel;

#if WITH_EDITOR
protected:
	virtual FText GetNodeTitle() const { return DisplayLabel; }
	virtual void SetNodeTitle(const FText& InTitle) { DisplayLabel = InTitle; }
	virtual bool CanEditChange(const FProperty* InProperty) const override
	{
		return Super::CanEditChange(InProperty) && !(
			InProperty->GetName() == GET_MEMBER_NAME_CHECKED(ThisClass, NodeTitle)
			|| InProperty->GetName() == GET_MEMBER_NAME_CHECKED(ThisClass, NodeColor)
			|| InProperty->GetName() == GET_MEMBER_NAME_CHECKED(ThisClass, ChildLimit)
			|| InProperty->GetName() == GET_MEMBER_NAME_CHECKED(ThisClass, ParentLimit)
		);
	}
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		if (DisplayLabel.IsEmptyOrWhitespace()) DisplayLabel = NSLOCTEXT("GameSection", "Unknown", "Unknown Section");
	}
#endif
};
