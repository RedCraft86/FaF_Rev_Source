// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "JsonObjectWrapper.h"
#include "EditorUtilityWidget.h"
#include "GamejoltVersionWidget.generated.h"

class USpinBox;
class UButton;

UCLASS(Abstract)
class GTDEVELOPER_API UGTGamejoltVersionWidget final : public UEditorUtilityWidget
{
	GENERATED_BODY()

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<USpinBox> VersionMajor;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> VersionMajorButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<USpinBox> VersionMinor;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> VersionMinorButton;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<USpinBox> VersionPatch;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> VersionPatchButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<USpinBox> VersionBuild;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> VersionBuildButton;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> FetchVersion;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> UpdateVersion;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UWidget> LoadingBlur;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UWidget> ErrorScreen;

	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<class UMultiLineEditableTextBox> ErrorMessageBox;
	
	UPROPERTY(Transient, meta = (BindWidget))
		TObjectPtr<UButton> CloseError;

	UFUNCTION() void MajorButtonEvent();
	UFUNCTION() void MinorButtonEvent();
	UFUNCTION() void PatchButtonEvent();
	UFUNCTION() void BuildButtonEvent();
	UFUNCTION() void FetchVersionEvent();
	UFUNCTION() void UpdateVersionEvent();
	UFUNCTION() void CloseErrorEvent();
	
	virtual void NativeDestruct() override;
	virtual void NativeConstruct() override;
	void UpdateLocalVersion(const FString& Version) const;
};
