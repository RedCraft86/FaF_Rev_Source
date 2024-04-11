// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Handles/ExpressiveText.h"
#include "Asset/ExpressiveTextAsset.h"
#include "ExpressiveTextData.generated.h"

// ExpressiveTextWidthSettings -> Value -> Add Metadata: ClampMin = 0.0f, UIMin = 0.0f
// ExpressiveTextFields -> DefaultFontSize -> Add Metadata: EditCondition = "UseDefaultFontSize"
// ExpressiveTextFields -> UseDefaultFontSize -> Add Metadata: InlineEditConditionToggle

USTRUCT(BlueprintType)
struct FAF_REV_API FExpressiveTextData
{
	GENERATED_BODY()

	FExpressiveTextData();

	int64 CalcChecksum() const;
	FExpressiveText GetExpressiveText() const;
	
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ExpressiveText", meta = (AllowPrivateAccess = true))
		bool bUseAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ExpressiveText", meta = (EditCondition = "!bUseAsset", EditConditionHides, AllowPrivateAccess = true, ShowOnlyInnerProperties))
		FExpressiveTextFields ComplexText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ExpressiveText", meta = (EditCondition = "bUseAsset", EditConditionHides, AllowPrivateAccess = true))
		UExpressiveTextAsset* TextAsset;
};
