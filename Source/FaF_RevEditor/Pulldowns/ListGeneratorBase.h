// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "DataTableEditorUtils.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "ListGeneratorBase.generated.h"

UCLASS(Abstract, NotBlueprintable, NotBlueprintType)
class USimpleListGenerator : public UPulldownListGeneratorBase
{
	GENERATED_BODY()

protected:
	
	virtual TArray<TSharedPtr<FPulldownRow>> OnGenerate() const { return {}; }
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows(const TArray<UObject*>& OuterObjects, const FStructContainer& StructInstance) const override;
	
};

#define GetDataObj(Type, Name) Type* Name = Cast<Type>(DataObjectPtr.LoadSynchronous())

UCLASS(Abstract, NotBlueprintable, NotBlueprintType)
class UObjectListGenerator : public USimpleListGenerator
{
	GENERATED_BODY()

public:

	UPROPERTY(EditInstanceOnly, Category = "Pulldown", meta = (DisplayPriority = -1))
		bool bUpdateData;

	UPROPERTY(VisibleInstanceOnly, Category = "Pulldown", meta = (DisplayPriority = -1))
		FName FoundObject;
	
	virtual FName GetDataObjectSettingName() const { return NAME_None; }
	virtual TSoftObjectPtr<UObject> GetDataObject() const { return nullptr; }

protected:
	
	virtual void BeginDestroy() override;
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnSettingsChanged(UObject* Object, FPropertyChangedEvent& Property);
	virtual void RebindSettingsDelegates();
	
	virtual bool HasSourceAsset() const override { return true; }
	virtual FString GetSourceAssetName() const override { return DataObjectPtr.GetAssetName(); }
	virtual void UpdateObjectInfo(const UObject* InObj) { FoundObject = InObj ? InObj->GetFName() : NAME_None; }
	
	UPROPERTY(Transient) TSoftObjectPtr<UObject> DataObjectPtr;
	UPROPERTY(Transient) TArray<FName> PreChangeRowNames;
};

UCLASS(Abstract)
class UTableListGenerator : public UObjectListGenerator, public FDataTableEditorUtils::INotifyOnDataTableChanged
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleInstanceOnly, Category = "Pulldown")
		FName StructType;

protected:

	virtual void UpdateObjectInfo(const UObject* InDataObject) override;
	virtual void PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	virtual void PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
};