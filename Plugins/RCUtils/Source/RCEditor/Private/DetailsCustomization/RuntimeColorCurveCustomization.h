// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "RCEngineTypes.h"

#include "IDetailPropertyRow.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"

class FRuntimeColorCustomization final : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FRuntimeColorCustomization());
	}

private:

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override {};
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		const TSharedPtr<IPropertyHandle> CurveHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRuntimeLinearColorCurve, Curve));
		StructBuilder.AddProperty(CurveHandle.ToSharedRef()).DisplayName(StructPropertyHandle->GetPropertyDisplayName());
	}
};
