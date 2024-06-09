// Copyright (C) RedCraft86. All Rights Reserved.

#include "Widgets/GamejoltVersionWidget.h"

#include "GamejoltSubsystem.h"
#include "GeneralProjectSettings.h"
#include "HttpModule.h"
#include "JsonObjectWrapper.h"
#include "Components/SpinBox.h"
#include "Components/Button.h"
#include "Components/MultiLineEditableTextBox.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

// ReSharper disable once CppMemberFunctionMayBeConst
void UGTGamejoltVersionWidget::MajorButtonEvent()
{
	VersionMajor->SetValue(VersionMajor->GetValue() + 1);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGTGamejoltVersionWidget::MinorButtonEvent()
{
	VersionMinor->SetValue(VersionMinor->GetValue() + 1);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGTGamejoltVersionWidget::PatchButtonEvent()
{
	VersionPatch->SetValue(VersionPatch->GetValue() + 1);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGTGamejoltVersionWidget::BuildButtonEvent()
{
	VersionBuild->SetValue(VersionBuild->GetValue() + 1);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGTGamejoltVersionWidget::FetchVersionEvent()
{
	LoadingBlur->SetVisibility(ESlateVisibility::Visible);
	if (const UGamejoltSubsystem* Subsystem = UGamejoltSubsystem::Get())
	{
		Subsystem->GetData(UGamejoltSubsystem::GetGamejoltVersionDataKey(), false,
			[this](const FGamejoltResponse& Response, const FString& Data)
			{
				if (Response.bSuccess)
				{
					const FIntVector4 Version = UGamejoltSubsystem::FormatGamejoltVersionData(Data);
					VersionMajor->SetValue(Version.X);
					VersionMinor->SetValue(Version.Y);
					VersionPatch->SetValue(Version.Z);
					VersionBuild->SetValue(Version.W);

					UpdateLocalVersion(FString::Printf(TEXT("%d.%d.%d.%d"),
						Version.X, Version.Y, Version.Z, Version.W));
				}
				else
				{
					ErrorMessageBox->SetText(FText::FromString(Response.ErrorMsg));
					ErrorScreen->SetVisibility(ESlateVisibility::Visible);
				}
				
				LoadingBlur->SetVisibility(ESlateVisibility::Collapsed);
			});
	}
	else
	{
		ErrorMessageBox->SetText(INVTEXT("Gamejolt Subsystem not loaded"));
		ErrorScreen->SetVisibility(ESlateVisibility::Visible);
		LoadingBlur->SetVisibility(ESlateVisibility::Collapsed);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGTGamejoltVersionWidget::UpdateVersionEvent()
{
	LoadingBlur->SetVisibility(ESlateVisibility::Visible);
	const FString VerStr = FString::Printf(TEXT("%d.%d.%d.%d"),
		(int32)VersionMajor->GetValue(), (int32)VersionMinor->GetValue(),
		(int32)VersionPatch->GetValue(), (int32)VersionBuild->GetValue());
	
	UpdateLocalVersion(VerStr);
	if (const UGamejoltSubsystem* Subsystem = UGamejoltSubsystem::Get())
	{
		Subsystem->SetData(UGamejoltSubsystem::GetGamejoltVersionDataKey(), VerStr, false,
			[this](const FGamejoltResponse& Response)
			{
				if (!Response.bSuccess)
				{
					ErrorMessageBox->SetText(FText::FromString(Response.ErrorMsg));
					ErrorScreen->SetVisibility(ESlateVisibility::Visible);
				}

				LoadingBlur->SetVisibility(ESlateVisibility::Collapsed);
			});
	}
	else
	{
		ErrorMessageBox->SetText(INVTEXT("Gamejolt Subsystem not loaded"));
		ErrorScreen->SetVisibility(ESlateVisibility::Visible);
		LoadingBlur->SetVisibility(ESlateVisibility::Collapsed);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGTGamejoltVersionWidget::CloseErrorEvent()
{
	ErrorScreen->SetVisibility(ESlateVisibility::Collapsed);
}

void UGTGamejoltVersionWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (VersionMajorButton) VersionMajorButton->OnClicked.RemoveDynamic(this, &UGTGamejoltVersionWidget::MajorButtonEvent);
	if (VersionMinorButton) VersionMinorButton->OnClicked.RemoveDynamic(this, &UGTGamejoltVersionWidget::MinorButtonEvent);
	if (VersionPatchButton) VersionPatchButton->OnClicked.RemoveDynamic(this, &UGTGamejoltVersionWidget::PatchButtonEvent);
	if (VersionBuildButton) VersionBuildButton->OnClicked.RemoveDynamic(this, &UGTGamejoltVersionWidget::BuildButtonEvent);
	if (FetchVersion) UpdateVersion->OnClicked.RemoveDynamic(this, &UGTGamejoltVersionWidget::FetchVersionEvent);
	if (UpdateVersion) UpdateVersion->OnClicked.RemoveDynamic(this, &UGTGamejoltVersionWidget::UpdateVersionEvent);
	if (CloseError) CloseError->OnClicked.RemoveDynamic(this, &UGTGamejoltVersionWidget::CloseErrorEvent);
}

void UGTGamejoltVersionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	VersionMajorButton->OnClicked.AddDynamic(this, &UGTGamejoltVersionWidget::MajorButtonEvent);
	VersionMinorButton->OnClicked.AddDynamic(this, &UGTGamejoltVersionWidget::MinorButtonEvent);
	VersionPatchButton->OnClicked.AddDynamic(this, &UGTGamejoltVersionWidget::PatchButtonEvent);
	VersionBuildButton->OnClicked.AddDynamic(this, &UGTGamejoltVersionWidget::BuildButtonEvent);
	FetchVersion->OnClicked.AddDynamic(this, &UGTGamejoltVersionWidget::FetchVersionEvent);
	UpdateVersion->OnClicked.AddDynamic(this, &UGTGamejoltVersionWidget::UpdateVersionEvent);
	CloseError->OnClicked.AddDynamic(this, &UGTGamejoltVersionWidget::CloseErrorEvent);
	LoadingBlur->SetVisibility(ESlateVisibility::Collapsed);
	ErrorScreen->SetVisibility(ESlateVisibility::Collapsed);
	FetchVersionEvent();
}

void UGTGamejoltVersionWidget::UpdateLocalVersion(const FString& Version) const
{
	FString PathL, PathR;
	FPaths::GetProjectFilePath().Split(TEXT("/"), &PathL, &PathR, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	PathL = PathL / TEXT("Config/DefaultGame.ini");

	GConfig->SetString(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion"), *Version, PathL);
	GetMutableDefault<UGeneralProjectSettings>()->ProjectVersion = Version;
	GConfig->Flush(false, PathL);
}
