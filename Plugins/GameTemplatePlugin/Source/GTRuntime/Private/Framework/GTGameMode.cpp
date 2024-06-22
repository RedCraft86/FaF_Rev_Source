// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTGameMode.h"
#include "GTUserWidget.h"
#include "GTGameInstance.h"
#include "GTPlayerController.h"
#include "Kismet/GameplayStatics.h"

AGTGameModeBase::AGTGameModeBase() : GameInstance(nullptr)
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
	MusicManagerClass = AMusicManagerBase::StaticClass();
	PlayerControllerClass = AGTPlayerController::StaticClass();
}

void AGTGameModeBase::SetGameInputMode(const EGameInputMode InputMode, const bool bShowMouseCursor, const EMouseLockMode MouseLock, const bool bHideCursorOnCapture, UUserWidget* FocusWidget)
{
	SetInputModeData({InputMode, bShowMouseCursor, MouseLock, bHideCursorOnCapture, FocusWidget});
}

void AGTGameModeBase::SetInputModeData(const FGameInputModeData& InputMode)
{
	if (APlayerController* PC = AGTPlayerController::Get(this))
	{
		InputModeData = InputMode;
		PC->SetShowMouseCursor(InputModeData.ShouldShowMouse());
		PC->SetInputMode(InputModeData.GetInputMode());
		PC->FlushPressedKeys();
	}
}

UGTUserWidget* AGTGameModeBase::FindOrAddWidget(const TSubclassOf<UGTUserWidget> Class, const FGameplayTagContainer InTags)
{
	for (const TObjectPtr<UGTUserWidget>& Obj : WidgetObjs)
	{
		if (Obj && Obj->IsA(Class)) return Obj;
	}
	
	UGTUserWidget* Obj = UGTUserWidget::CreateSmartWidget(this, nullptr, Class, InTags);
	
	if (Obj) WidgetObjs.Add(Obj);
	return Obj;
}

UGTUserWidget* AGTGameModeBase::GetWidget(const TSubclassOf<UGTUserWidget> Class, const FGameplayTag FilterTag)
{
	WidgetObjs.Remove(nullptr);
	for (const TObjectPtr<UGTUserWidget>& Obj : WidgetObjs)
	{
		if (Obj && Obj->IsA(Class))
		{
			if (FilterTag.IsValid())
			{
				return Obj->WidgetTags.HasTagExact(FilterTag) ? Obj : nullptr;
			}

			return Obj;
		}
	}

	return nullptr;
}

void AGTGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GameInstance = GetGameInstance<UGTGameInstance>();
	if (GameInstance) GameInstance->OnWorldBeginPlay(GetWorld());
	for (const TSubclassOf<UGTUserWidget>& Widget : DefaultWidgets)
	{
		FindOrAddWidget(Widget, {});
	}
}

void AGTGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	InputModeData.ClearReferences();
	Super::EndPlay(EndPlayReason);
}

void AGTGameModeBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if (!MusicManagerClass)
	{
		MusicManagerClass = AMusicManagerBase::StaticClass();
	}
	
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	MusicManager = GetWorld()->SpawnActor<AMusicManagerBase>(MusicManagerClass, SpawnInfo);
}

void AGTGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (GameInstance) GameInstance->OnWorldTick(DeltaSeconds);
}

/* Statics */

AGTGameModeBase* AGTGameModeBase::GetGameModeSmart(const UObject* WorldContextObject, const TSubclassOf<AGTGameModeBase> Class)
{
	AGTGameModeBase* Obj = Cast<AGTGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	return Obj && Obj->IsA(Class) ? Obj : nullptr;
}

EGTValidPins AGTGameModeBase::GetGameModeChecked(AGTGameModeBase*& OutObject, const UObject* WorldContextObject, const TSubclassOf<AGTGameModeBase> Class)
{
	OutObject = GetGameModeSmart(WorldContextObject, Class);
	return IsValid(OutObject) ? EGTValidPins::Valid : EGTValidPins::NotValid;
}

EGTFoundPins AGTGameModeBase::GetSmartWidget(UGTUserWidget*& FoundWidget, const UObject* WorldContextObject, const TSubclassOf<UGTUserWidget> Class, const FGameplayTag FilterTag)
{
	AGTGameModeBase* GM = Get<AGTGameModeBase>(WorldContextObject);
	FoundWidget = GM ? GM->GetWidget(Class, FilterTag) : nullptr;
	return FoundWidget ? EGTFoundPins::Found : EGTFoundPins::NotFound;
}
