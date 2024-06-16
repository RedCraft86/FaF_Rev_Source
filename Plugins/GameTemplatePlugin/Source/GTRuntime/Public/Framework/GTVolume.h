// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "GTActor.h"
#include "ActorIconData.h"
#include "GameFramework/Volume.h"
#include "GTVolume.generated.h"

UCLASS()
class GTRUNTIME_API AGTVolume : public AVolume
{
	GENERATED_BODY()

public:

	AGTVolume();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Subobjects")
		TObjectPtr<UBillboardComponent> IconComponent;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Enable State Changed")
		FActorEnableChangedSignature OnEnableStateChangedBP;
	
	UFUNCTION(BlueprintCallable, Category = "Actor")
		void SetEnabled(const bool bInEnabled);
	
	UFUNCTION(BlueprintPure, Category = "Actor")
		bool IsEnabled() const { return bEnabled; }
	
	UFUNCTION(BlueprintPure, Category = "Actor")
		FGuid GetRuntimeGuid() const { return RuntimeGuid; }

	DECLARE_MULTICAST_DELEGATE_OneParam(FActorEnableChanged, const bool);
	FActorEnableChanged OnEnableStateChangedEvent;

protected:

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (DisplayPriority = -10))
		bool bEnabled;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Settings", meta = (DisplayPriority = -10))
		FGTActorIconData Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor", NonPIEDuplicateTransient, TextExportTransient, NonTransactional)
		FGuid RuntimeGuid;
	
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnEnableStateChanged(const bool bIsEnabled);
	
#if WITH_EDITOR
private:
	TSoftObjectPtr<UTexture2D> BaseSprite;
#endif
};
