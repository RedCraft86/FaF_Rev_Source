﻿// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

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

	UFUNCTION(BlueprintImplementableEvent)
		void OnSetup(UPanelSlot* InSlot);
	
private:

	void Setup(UGCQuickTimeEventManager* InManager, const FString& InID, UPanelSlot* InSlot);
};

UCLASS(Abstract)
class GAMECORE_API UGCQuickTimeEventWidget : public UGCUserWidget
{
	GENERATED_BODY()

	friend class UGCQuickTimeEventManager;

public:

	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* KeySlots;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "QTEWidget", meta = (BindWidget))
		class UProgressBar* FillBar;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "QTEWidget")
		UGCQuickTimeEventManager* Manager;

	UPROPERTY(EditAnywhere, Category = "Settings")
		TSubclassOf<UGCQuickTimeKeyWidget> KeyWidgetClass;

	UFUNCTION(BlueprintImplementableEvent)
		void OnKeySuccess(const FString& ID);

	UFUNCTION(BlueprintImplementableEvent)
		void OnKeyFailed(const FString& ID);

private:

	UPROPERTY(Transient)
		TMap<FString, UGCQuickTimeKeyWidget*> KeyWidgetMap;
	
	float TargetPercent = 0.5f;

	void Setup(UGCQuickTimeEventManager* InManager);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION() void OnFailed();
	UFUNCTION() void OnSuccess();
	
	UFUNCTION() void OnKeyAdd(const FString& InID);
	UFUNCTION() void OnKeyRemove(const FString& InID);
};