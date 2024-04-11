// Copyright (C) RedCraft86. All Rights Reserved.

#include "ExpressiveTextData.h"
#include "FRSettings.h"
#include "FaF_Rev.h"

FExpressiveTextData::FExpressiveTextData() : bUseAsset(false), ComplexText({}), TextAsset(nullptr)
{
	ComplexText.DefaultFontSize = 24;
	ComplexText.WidthSettings.Value = 0.0f;
	ComplexText.WidthSettings.ValueType = EExpressiveTextWidthType::Pixels;
	ComplexText.DefaultStyle = UFRSettings::GetConst()->DefaultExpressiveTextStyle.LoadSynchronous();
}

int64 FExpressiveTextData::CalcChecksum() const
{
	int64 Result = ComplexText.CalcChecksum();
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
		else SMART_LOG(Error, TEXT("Text asset selected but no text asset provided."))
	}
	else
	{
		Result.SetFields(ComplexText);
		if (!Result.GetDefaultStyle())
		{
			Result.SetDefaultStyle(UFRSettings::GetConst()->DefaultExpressiveTextStyle.LoadSynchronous());
		}
	}
	return Result;
}
