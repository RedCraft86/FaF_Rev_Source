// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "GTAssetFactory.generated.h"

UCLASS(Abstract)
class GTEDITOR_API UGTAssetFactory : public UFactory
{
	GENERATED_BODY()

public:

	UGTAssetFactory()
	{
		bCreateNew = true;
		bEditAfterNew = true;
		SupportedClass = UDataAsset::StaticClass();
	}

	bool ConfigureProperties() override
	{
		DataClass = UDataAsset::StaticClass();
		return DataClass ? true : false;
	}
	
	FString GetDefaultNewAssetName() const override
	{
		return TEXT("New") + (DataClass ? DataClass->GetName() : GetSupportedClass()->GetName());
	}
	
	UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override
	{
		const UClass* ObjClass = DataClass ? *DataClass : Class;
		check(ObjClass->IsChildOf(SupportedClass));
		return NewObject<UDataAsset>(InParent, ObjClass, Name, Flags | RF_Transactional);
	}
	
protected:

	TSubclassOf<UDataAsset> DataClass;
};
