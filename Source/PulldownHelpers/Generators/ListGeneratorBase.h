// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "DataTableEditorUtils.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "ListGeneratorBase.generated.h"

using IDataTableListener = FDataTableEditorUtils::INotifyOnDataTableChanged;

UCLASS(Abstract, NotBlueprintable, NotBlueprintType)
class PULLDOWNHELPERS_API UListGeneratorBase : public UPulldownListGeneratorBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditInstanceOnly, Category = "Pulldown", meta = (DisplayPriority = -1))
		bool bUpdateData;

	UPROPERTY(VisibleInstanceOnly, Category = "Pulldown", meta = (DisplayPriority = -1))
		FName FoundObject;
	
	virtual FName GetDataObjectPropertyName() const { return NAME_None; }
	virtual TSoftObjectPtr<UObject> GetDataObject() const { return nullptr; }

protected:
	
	virtual void RebindSettingsDelegates();
	virtual void OnSettingsChanged(UObject* Object, struct FPropertyChangedEvent& Property);
	
	virtual void UpdateObjectInfo(const UObject* InDataObject) { FoundObject = InDataObject ? InDataObject->GetFName() : NAME_None; }
	virtual TArray<TSharedPtr<FPulldownRow>> OnGenerate(UObject* InDataObject) const { return {}; }

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows(const TArray<UObject*>& OuterObjects, const FStructContainer& StructInstance) const override;
	virtual bool HasSourceAsset() const override { return true; };
	virtual FString GetSourceAssetName() const override { return DataObjectPtr.GetAssetName(); };
	
	UPROPERTY()
		mutable TSoftObjectPtr<UObject> DataObjectPtr;

	UPROPERTY(Transient)
		TArray<FName> PreChangeRowNames;
};

UCLASS(Abstract)
class UTableListGeneratorBase : public UListGeneratorBase, public IDataTableListener
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleInstanceOnly, Category = "Pulldown")
		FName StructType;

protected:

	virtual void UpdateObjectInfo(const UObject* InDataObject) override
	{
		Super::UpdateObjectInfo(InDataObject);
		
		StructType = NAME_None;
		if (const UDataTable* Table = Cast<UDataTable>(InDataObject))
		{
			StructType = Table->RowStruct->GetFName();
		}
	}
	
	virtual void PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	virtual void PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
};