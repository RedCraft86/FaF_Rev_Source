// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GameSection/GameSectionManagerBase.h"
#include "GameSectionManager.generated.h"

UCLASS()
class FAF_REV_API UGameSectionManager : public UGameSectionManagerBase
{
	GENERATED_BODY()

public:

private:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return UGameInstanceSubsystem::ShouldCreateSubsystem(Outer); }
};
