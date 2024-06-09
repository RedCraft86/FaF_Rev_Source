// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Handles/ExpressiveText.h"
#include "Asset/ExpressiveTextAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExpressiveTextData.generated.h"

// ExpressiveTextFields -> DefaultFontSize -> Add Metadata: EditCondition = "UseDefaultFontSize"
// ExpressiveTextFields -> UseDefaultFontSize -> Add Metadata: InlineEditConditionToggle

USTRUCT(BlueprintType)
struct GTRUNTIME_API FExpressiveTextData
{
	GENERATED_BODY()

	FExpressiveTextData();

	int64 CalcChecksum() const;
	FExpressiveText GetExpressiveText();
	void SetText(const FText& InText, const bool bUseFieldsFromAsset = false);
	void SetTextFields(const FExpressiveTextFields& InFields);
	void SetTextAsset(UExpressiveTextAsset* InAsset);
	
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ExpressiveText", meta = (AllowPrivateAccess = true))
		bool bUseAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ExpressiveText", meta = (EditCondition = "!bUseAsset", EditConditionHides, AllowPrivateAccess = true, ShowOnlyInnerProperties))
		FExpressiveTextFields TextFields;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ExpressiveText", meta = (EditCondition = "bUseAsset", EditConditionHides, AllowPrivateAccess = true))
		TObjectPtr<UExpressiveTextAsset> TextAsset;
};

UCLASS(DisplayName = "Expressive Text Data")
class GTRUNTIME_API UExprTextDataFunctionLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Expressive Text Data")
		static FExpressiveText GetExpressiveText(UPARAM(ref) FExpressiveTextData& InData);

	UFUNCTION(BlueprintCallable, Category = "Expressive Text Data")
		static void SetText(UPARAM(ref) FExpressiveTextData& InData, const FText InText, const bool bUseFieldsFromAsset = false);

	UFUNCTION(BlueprintCallable, Category = "Expressive Text Data")
		static void SetTextFields(UPARAM(ref) FExpressiveTextData& InData, const FExpressiveTextFields& InFields);
	
	UFUNCTION(BlueprintCallable, Category = "Expressive Text Data")
		static void SetTextAsset(UPARAM(ref) FExpressiveTextData& InData, UExpressiveTextAsset* InAsset);
};
