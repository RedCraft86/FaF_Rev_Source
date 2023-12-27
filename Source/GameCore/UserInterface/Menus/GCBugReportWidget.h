// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "JsonObjectWrapper.h"
#include "UserInterface/GCUserWidget.h"
#include "GCBugReportWidget.generated.h"

UCLASS(Abstract, DisplayName = "Bug Report Widget Base")
class GAMECORE_API UGCBugReportWidget : public UGCUserWidget
{
	GENERATED_BODY()
	
public:

	UGCBugReportWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* CoverPanel;
	
	UPROPERTY(Transient, meta = (BindWidget))
		UPanelWidget* ErrorPanel;

	UPROPERTY(Transient, meta = (BindWidget))
		class UTextBlock* ErrorText;
	
	UPROPERTY(Transient, meta = (BindWidget))
		class UButton* ReportButton;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidget))
		class UComboBoxString* CategoryBox;

	UPROPERTY(Transient, meta = (BindWidget))
		UButton* ExitButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* SubmittedAnim;

	DECLARE_MULTICAST_DELEGATE(FOnBackPressed);
	FOnBackPressed OnExit;

	UFUNCTION(BlueprintImplementableEvent)
		void ResetFields();

	UFUNCTION(BlueprintNativeEvent)
		FJsonObjectWrapper GatherReportData() const;
	FJsonObjectWrapper GatherReportData_Implementation() const;

protected:

	FString LastErrorMsg;

	virtual void InitData(AGCPlayerController* Controller) override;
	virtual void NativeConstruct() override;
	
	UFUNCTION()
		void ReportButtonClicked();

	UFUNCTION()
		void OnExitClicked();
};
