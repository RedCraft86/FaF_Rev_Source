// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "PinEnums.h"
#include "GTGameMode.h"
#include "FRGameMode.generated.h"

#define FRGamemode(Context) AGTGameModeBase::Get<AFRGameModeBase>(Context)

UCLASS(Abstract)
class FAF_REV_API AFRGameModeBase final : public AGTGameModeBase
{
	GENERATED_BODY()

public:

	AFRGameModeBase();

	UPROPERTY(Transient)
		class APhotoModeActor* PhotoModeActor;

	UPROPERTY(Transient)
		class AInspectionActor* InspectionActor;

public: // Shortcuts

	UFUNCTION(BlueprintCallable, Category = "Game", meta = (WorldContext = "WorldContextObject", DynamicOutputParam = "ReturnValue", DeterminesOutputType = "Class", ExpandEnumAsExecs = "ReturnValue"))
		static EFoundPins GetGameWidget(UGTUserWidget*& FoundWidget, const UObject* WorldContextObject, UPARAM(meta = (AllowAbstract = false)) const TSubclassOf<UGTUserWidget> Class, const FGameplayTag FilterTag);
};
