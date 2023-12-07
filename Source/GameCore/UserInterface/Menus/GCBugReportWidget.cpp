// Copyright (C) Tayzar Linn. All Rights Reserved.

#include "GCBugReportWidget.h"
#include "GameJolt/GamejoltAPI.h"
#include "GameJolt/BugReporting.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"

UGCBugReportWidget::UGCBugReportWidget(const FObjectInitializer& ObjectInitializer)
	 : UGCUserWidget(ObjectInitializer)
{
	WidgetZOrder = 13;
	bAutoAddToViewport = false;
	
	CoverPanel = nullptr;
	ErrorPanel = nullptr;
	ErrorText = nullptr;
	ReportButton = nullptr;
	ExitButton = nullptr;
	CategoryBox = nullptr;
	SubmittedAnim = nullptr;
}

FJsonObjectWrapper UGCBugReportWidget::GatherReportData_Implementation() const
{
	return UReportBug::CreateBugReportData(this, TEXT(""), TEXT(""), true, true, true);
}

void UGCBugReportWidget::InitData(AGCPlayerController* Controller)
{
	Super::InitData(Controller);
	ReportButton->OnClicked.AddDynamic(this, &UGCBugReportWidget::ReportButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &UGCBugReportWidget::OnExitClicked);
}

void UGCBugReportWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CoverPanel->SetVisibility(ESlateVisibility::Collapsed);
	ErrorPanel->SetVisibility(ESlateVisibility::Collapsed);
	ErrorText->SetText(FText::GetEmpty());
	ReportButton->SetIsEnabled(true);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGCBugReportWidget::ReportButtonClicked()
{
	ReportButton->SetIsEnabled(false);
	CoverPanel->SetVisibility(ESlateVisibility::Visible);
	ErrorPanel->SetVisibility(ESlateVisibility::Collapsed);
	if (CategoryBox->GetSelectedOption() != TEXT("None"))
	{
		GamejoltAPI::CreateBugReport(GatherReportData(), [this](const bool bSuccess, const bool, const FString& Response)
	   {
		   ReportButton->SetIsEnabled(true);
		   CoverPanel->SetVisibility(ESlateVisibility::Collapsed);
		   if (!bSuccess)
		   {
			   ErrorText->SetText(FText::FromString(Response));
			   ErrorPanel->SetVisibility(ESlateVisibility::Visible);
		   }
		   else
		   {
				PlayAnimation(SubmittedAnim);
				ResetFields();
		   }
	   });
	}
	else
	{
		ReportButton->SetIsEnabled(true);
		CoverPanel->SetVisibility(ESlateVisibility::Collapsed);
		ErrorPanel->SetVisibility(ESlateVisibility::Visible);
		ErrorText->SetText(FText::FromString("No report category selected!"));
	}
}

void UGCBugReportWidget::OnExitClicked()
{
	RemoveWidget(nullptr, true);
	OnExit.Broadcast();
}
