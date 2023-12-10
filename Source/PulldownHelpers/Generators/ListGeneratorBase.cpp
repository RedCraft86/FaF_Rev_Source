// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "ListGeneratorBase.h"
#include "Core/GCSettings.h"

void UListGeneratorBase::RebindSettingsDelegates()
{
	UGCSettings* Settings = UGCSettings::Get();
	Settings->OnSettingChanged().RemoveAll(this);
	Settings->OnSettingChanged().AddUObject(this, &UListGeneratorBase::OnSettingsChanged);
	
	DataObjectPtr = GetDataObject();
	UpdateObjectInfo(DataObjectPtr.LoadSynchronous());
	NotifyPulldownContentsSourceChanged();
}

void UListGeneratorBase::OnSettingsChanged(UObject* Object, struct FPropertyChangedEvent& Property)
{
	if (!Object || GetDataObjectPropertyName().IsNone() || Property.GetPropertyName() != GetDataObjectPropertyName())
		return;

	DataObjectPtr = GetDataObject();
	UpdateObjectInfo(DataObjectPtr.LoadSynchronous());
	NotifyPulldownContentsSourceChanged();
}

void UListGeneratorBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UListGeneratorBase, bUpdateData) && bUpdateData)
	{
		bUpdateData = false;
		RebindSettingsDelegates();
	}
}

void UListGeneratorBase::PostInitProperties()
{
	Super::PostInitProperties();
	if (GIsEditor)
	{
		RebindSettingsDelegates();
	}
	else
	{
		if (UGCSettings* Settings = UGCSettings::Get())
		{
			Settings->OnSettingChanged().RemoveAll(this);
		}
	}
}

void UListGeneratorBase::BeginDestroy()
{
	if (UGCSettings* Settings = UGCSettings::Get())
	{
		Settings->OnSettingChanged().RemoveAll(this);
	}

	Super::BeginDestroy();
}

TArray<TSharedPtr<FPulldownRow>> UListGeneratorBase::GetPulldownRows(const TArray<UObject*>& OuterObjects, const FStructContainer& StructInstance) const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows = Super::GetPulldownRows(OuterObjects, StructInstance);
	PulldownRows.Append(OnGenerate(DataObjectPtr.LoadSynchronous()));
	return PulldownRows;
}

void UTableListGeneratorBase::PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
	if (!Changed || Changed != DataObjectPtr.LoadSynchronous() ||
		Info != FDataTableEditorUtils::EDataTableChangeInfo::RowList)
		return;
	
	PreChangeRowNames = Changed->GetRowNames();
}

void UTableListGeneratorBase::PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
	if (!Changed || Changed != DataObjectPtr.LoadSynchronous() ||
		Info != FDataTableEditorUtils::EDataTableChangeInfo::RowList)
		return;
	
	const TArray<FName> PostChangeRowNames = Changed->GetRowNames();
	if (NotifyPulldownRowChanged(PreChangeRowNames, PostChangeRowNames))
	{
		PreChangeRowNames.Empty();
	}
}
