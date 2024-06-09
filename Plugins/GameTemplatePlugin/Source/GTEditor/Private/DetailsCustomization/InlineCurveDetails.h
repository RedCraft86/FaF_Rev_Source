// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Data/InlineCurves.h"
#include "IDetailPropertyRow.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"

class FInlineCurveDetails final : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FInlineCurveDetails>();
	}

private:

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override {}
	
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		const TSharedPtr<IPropertyHandle> CurveHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FInlineFloatCurve, Curve));
		StructBuilder.AddProperty(CurveHandle.ToSharedRef()).DisplayName(StructPropertyHandle->GetPropertyDisplayName());
	}
};
