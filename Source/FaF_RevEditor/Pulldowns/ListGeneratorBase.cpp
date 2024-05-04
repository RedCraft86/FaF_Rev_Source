// Copyright (C) RedCraft86. All Rights Reserved.

#include "ListGeneratorBase.h"
#include "FRSettings.h"

TArray<TSharedPtr<FPulldownRow>> USimpleListGenerator::GetPulldownRows(const TArray<UObject*>& OuterObjects, const FStructContainer& StructInstance) const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows = Super::GetPulldownRows(OuterObjects, StructInstance);
	PulldownRows.Append(OnGenerate());
	return PulldownRows;
}

void UObjectListGenerator::BeginDestroy()
{
	if (UFRSettings* Settings = FRSettings)
	{
		Settings->OnSettingChanged().RemoveAll(this);
	}

	Super::BeginDestroy();
}

void UObjectListGenerator::PostInitProperties()
{
	Super::PostInitProperties();
	if (GIsEditor)
	{
		RebindSettingsDelegates();
	}
	else if (UFRSettings* Settings = FRSettings)
	{
		Settings->OnSettingChanged().RemoveAll(this);
	}
}

void UObjectListGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (bUpdateData)
	{
		bUpdateData = false;
		RebindSettingsDelegates();
	}
}

void UObjectListGenerator::OnSettingsChanged(UObject* Object, FPropertyChangedEvent& Property)
{
	if (IsValid(Object) && !GetDataObjectSettingName().IsNone()
		&& Property.GetPropertyName() == GetDataObjectSettingName())
	{
		DataObjectPtr = GetDataObject();
		UpdateObjectInfo(DataObjectPtr.LoadSynchronous());
		NotifyPulldownContentsSourceChanged();
	}
}

void UObjectListGenerator::RebindSettingsDelegates()
{
	UFRSettings* Settings = FRSettings;
	Settings->OnSettingChanged().RemoveAll(this);
	Settings->OnSettingChanged().AddUObject(this, &UObjectListGenerator::OnSettingsChanged);
	
	DataObjectPtr = GetDataObject();
	UpdateObjectInfo(DataObjectPtr.LoadSynchronous());
	NotifyPulldownContentsSourceChanged();
}

void UTableListGenerator::UpdateObjectInfo(const UObject* InDataObject)
{
	Super::UpdateObjectInfo(InDataObject);
	
	StructType = NAME_None;
	if (GetDataObj(const UDataTable, Table))
	{
		StructType = Table->RowStruct->GetFName();
	}
}

void UTableListGenerator::PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
	if (!IsValid(Changed) || Changed != DataObjectPtr.LoadSynchronous()) return;
	if (Info == FDataTableEditorUtils::EDataTableChangeInfo::RowList)
	{
		PreChangeRowNames = Changed->GetRowNames();
	}
}

void UTableListGenerator::PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
	if (!IsValid(Changed) || Changed != DataObjectPtr.LoadSynchronous()) return;
	if (Info == FDataTableEditorUtils::EDataTableChangeInfo::RowList)
	{
		const TArray<FName> PostChangeRowNames = Changed->GetRowNames();
		if (NotifyPulldownRowChanged(PreChangeRowNames, PostChangeRowNames))
		{
			PreChangeRowNames.Empty();
		}
	}
}