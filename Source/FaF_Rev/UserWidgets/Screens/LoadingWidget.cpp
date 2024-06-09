// Copyright (C) RedCraft86. All Rights Reserved.

#include "LoadingWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetSwitcher.h"
#include "FRGameMode.h"

ULoadingWidgetBase::ULoadingWidgetBase(const FObjectInitializer& ObjectInitializer)
	: UGTUserWidget(ObjectInitializer), LoadingLabel(nullptr), LoadingBar(nullptr), BackgroundImage(nullptr)
	, TipLabel(nullptr), TipText(nullptr), bUnloading(false), TotalObjs(0), Background(nullptr)
{
	ZOrder = 98;
	bAutoAdd = false;
}

void ULoadingWidgetBase::SlowTick()
{
	for (int i = LoadingObjs.Num() - 1; i >= 0; i--)
	{
		if (!LoadingObjs[i].IsValid() || LoadingObjs[i].IsAssetLoaded())
		{
			LoadingObjs.RemoveAt(i);
		}
	}

	Update(false);
}

void ULoadingWidgetBase::Update(const bool bFinish) const
{
	if (bFinish || TotalObjs == 0)
	{
		LoadingLabel->SetText(INVTEXT("Loading Finished..."));
		LoadingBar->SetPercent(1.0f);
	}
	else if (bUnloading)
	{
		LoadingLabel->SetText(INVTEXT("Unloading Current Level..."));
		LoadingBar->SetPercent(0.015f);
	}
	else if (LoadingObjs.IsEmpty())
	{
		GetWorld()->GetTimerManager().PauseTimer(SlowTickHandle);
		LoadingLabel->SetText(INVTEXT("Initializing Objects..."));
		LoadingBar->SetPercent(0.98f);
	}
	else
	{
		const int32 LoadedObjs = TotalObjs - LoadingObjs.Num();
		const float Percent = 0.1f + 0.88f * LoadedObjs / TotalObjs;
		LoadingLabel->SetText(FText::FromString(FString::Printf(
			TEXT("%.2f%% (%d/%d Objects Loaded)"), Percent * 100.0f, LoadedObjs, TotalObjs)));
		LoadingBar->SetPercent(Percent);
	}
}

void ULoadingWidgetBase::SetMinimalMode(const bool bInMinimalMode) const
{
	SwitcherWidget->SetActiveWidgetIndex(bInMinimalMode ? 1 : 0);
}

void ULoadingWidgetBase::FinishLoading(const TFunction<void()>& OnFinished)
{
	if (SwitcherWidget->GetActiveWidgetIndex() == 0)
	{
		GetWorld()->GetTimerManager().PauseTimer(SlowTickHandle);
		Background = nullptr;
		LoadingObjs.Empty();
		Update(true);
	}

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, [this, OnFinished]()
	{
		RemoveWidget(OnFinished);
		GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::GameOnly);
	}, 0.5f, false);
}

void ULoadingWidgetBase::BeginLoading(const TSet<FAssetData>& InObjects, const TSoftObjectPtr<UTexture2D>& InBackground, const TPair<FString, FText>& InTip)
{
	if (SwitcherWidget->GetActiveWidgetIndex() == 0)
	{
		bUnloading = true;
		TotalObjs = InObjects.Num();
		LoadingObjs = InObjects.Array();
		Background = InBackground.IsNull() ? DefaultBackground.LoadSynchronous() : InBackground.LoadSynchronous();
	
		LoadingBar->SetPercent(0.0f);
		LoadingLabel->SetText(INVTEXT("Starting Load..."));
		BackgroundImage->SetBrushFromTexture(Background);
		TipLabel->SetText(FText::FromString(InTip.Key));
		TipText->SetText(InTip.Value);

		Update(false);
		if (TotalObjs > 0) GetWorld()->GetTimerManager().UnPauseTimer(SlowTickHandle);
	}
	
	AddWidget(nullptr);
	GetGameMode<AFRGameModeBase>()->SetGameInputMode(EGameInputMode::UI_Only, true);
}

void ULoadingWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	GetWorld()->GetTimerManager().SetTimer(SlowTickHandle, this, &ULoadingWidgetBase::SlowTick, 0.05f, true);
	GetWorld()->GetTimerManager().PauseTimer(SlowTickHandle);
}

void ULoadingWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();
	BackgroundImage->SetBrushFromTexture(DefaultBackground.LoadSynchronous());
}
