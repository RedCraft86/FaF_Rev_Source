// Copyright (C) RedCraft86. All Rights Reserved.

#include "ExpressiveTextData.h"
#include "GTSettings.h"
#include "GTRuntime.h"

FExpressiveTextData::FExpressiveTextData() : bUseAsset(false), TextFields({}), TextAsset(nullptr)
{
	TextFields.DefaultFontSize = 24;
	TextFields.WrapSettings.Value = 0.0f;
	TextFields.WrapSettings.ValueType = EExpressiveTextWrapMode::WrapAtPixelCount;
	TextFields.DefaultStyle = UGTSettings::GetConst()->DefaultTextStyle.LoadSynchronous();
}

int64 FExpressiveTextData::CalcChecksum() const
{
	int64 Result = TextFields.CalcChecksum();
	Result = HashCombine(Result, GetTypeHash(bUseAsset));
	Result = HashCombine(Result, GetTypeHash(TextAsset));
	if (TextAsset) Result = HashCombine(Result, TextAsset->CalcChecksum());
	return Result;
}

FExpressiveText FExpressiveTextData::GetExpressiveText()
{
	FExpressiveText Result;
	if (bUseAsset)
	{
		if (TextAsset) Result.SetFields(TextAsset->Fields);
		else UE_LOG(GTRuntime, Error, TEXT("Supposed to use text asset but the asset is null."))
	}
	else
	{
		Result.SetFields(TextFields);
		if (!Result.GetDefaultStyle())
		{
			TextFields.DefaultStyle = UGTSettings::Get()->DefaultTextStyle.LoadSynchronous();
			Result.SetDefaultStyle(TextFields.DefaultStyle);
		}
	}
	
	return Result;
}

void FExpressiveTextData::SetText(const FText& InText, const bool bUseFieldsFromAsset)
{
	if (bUseFieldsFromAsset && TextAsset)
	{
		SetTextFields(TextAsset->Fields);
	}

	bUseAsset = false;
	TextFields.Text = InText;
}

void FExpressiveTextData::SetTextFields(const FExpressiveTextFields& InFields)
{
	bUseAsset = false;
	TextFields = InFields;
}

void FExpressiveTextData::SetTextAsset(UExpressiveTextAsset* InAsset)
{
	bUseAsset = true;
	TextAsset = InAsset;
}

FExpressiveText UExprTextDataFunctionLibrary::GetExpressiveText(FExpressiveTextData& InData)
{
	return InData.GetExpressiveText();
}

void UExprTextDataFunctionLibrary::SetText(FExpressiveTextData& InData, const FText InText, const bool bUseFieldsFromAsset)
{
	InData.SetText(InText, bUseFieldsFromAsset);
}

void UExprTextDataFunctionLibrary::SetTextFields(FExpressiveTextData& InData, const FExpressiveTextFields& InFields)
{
	InData.SetTextFields(InFields);
}

void UExprTextDataFunctionLibrary::SetTextAsset(FExpressiveTextData& InData, UExpressiveTextAsset* InAsset)
{
	InData.SetTextAsset(InAsset);
}
