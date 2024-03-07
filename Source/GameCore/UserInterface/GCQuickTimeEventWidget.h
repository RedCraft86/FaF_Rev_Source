// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "GCUserWidget.h"
#include "GCQuickTimeEventWidget.generated.h"

UCLASS(Abstract)
class GAMECORE_API UGCQuickTimeKeyWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UGCQuickTimeEventWidget;
	
public:

	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* KeyText;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "QTEWidget")
		class UGCQuickTimeEventManager* Manager;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "QTEWidget")
		FString KeyID;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "QTEWidget")
		FKey Key;

	UFUNCTION(BlueprintImplementableEvent)
		void OnSuccess();

	UFUNCTION(BlueprintImplementableEvent)
		void OnFailed();

private:

	void Setup(UGCQuickTimeEventManager* InManager, const FString& InID);
};

UCLASS(Abstract)
class GAMECORE_API UGCQuickTimeEventWidget : public UGCUserWidget
{
	GENERATED_BODY()

	friend class UGCQuickTimeEventManager;

public:

	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* KeySlots;

	UPROPERTY(Transient, meta = (BindWidget))
		class UProgressBar* FillBar;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "QTEWidget")
		UGCQuickTimeEventManager* Manager;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TSubclassOf<UGCQuickTimeKeyWidget> KeyWidgetClass;

private:

	UPROPERTY(Transient)
		TMap<FString, UGCQuickTimeKeyWidget*> KeyWidgetMap;

	void Setup(UGCQuickTimeEventManager* InManager);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	
	UFUNCTION() void OnKeyAdd(const FString& InID);
	UFUNCTION() void OnKeyRemove(const FString& InID);
};
