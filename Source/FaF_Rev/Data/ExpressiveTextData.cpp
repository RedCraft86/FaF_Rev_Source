// Copyright (C) RedCraft86. All Rights Reserved.

#include "ExpressiveTextData.h"
#include "FRSettings.h"
#include "FaF_Rev.h"

FExpressiveTextData::FExpressiveTextData() : bUseAsset(false), TextFields({}), TextAsset(nullptr)
{
	TextFields.DefaultFontSize = 24;
	TextFields.WidthSettings.Value = 0.0f;
	TextFields.WidthSettings.ValueType = EExpressiveTextWidthType::Pixels;
	TextFields.DefaultStyle = UFRSettings::GetConst()->DefaultTextStyle.LoadSynchronous();
}

int64 FExpressiveTextData::CalcChecksum() const
{
	int64 Result = TextFields.CalcChecksum();
	Result = HashCombine(Result, GetTypeHash(bUseAsset));
	Result = HashCombine(Result, GetTypeHash(TextAsset));
	if (TextAsset) Result = HashCombine(Result, TextAsset->CalcChecksum());
	return Result;
}

FExpressiveText FExpressiveTextData::GetExpressiveText() const
{
	FExpressiveText Result;
	if (bUseAsset)
	{
		if (TextAsset) Result.SetFields(TextAsset->Fields);
		else SMART_LOG(Error, TEXT("Supposed to use text asset but the asset is null."))
	}
	else
	{
		Result.SetFields(TextFields);
		if (!Result.GetDefaultStyle())
		{
			Result.SetDefaultStyle(UFRSettings::GetConst()->DefaultTextStyle.LoadSynchronous());
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
