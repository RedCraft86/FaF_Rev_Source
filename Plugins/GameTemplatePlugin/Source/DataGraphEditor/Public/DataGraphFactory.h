// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

#include "Factories/Factory.h"
#include "DataGraphFactory.generated.h"

UCLASS()
class UDataGraphFactory final : public UFactory
{
	GENERATED_BODY()

public:

	UDataGraphFactory();

	bool ConfigureProperties() override;
	FString GetDefaultNewAssetName() const override;
	UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

private:

	TSubclassOf<class UGenericDataGraph> GraphClass;
};
