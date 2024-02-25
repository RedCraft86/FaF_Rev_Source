// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

// ReSharper disable CppParameterMayBeConstPtrOrRef

#include "GCPlayerController.h"
#include "Core/GCSettings.h"
#include "GCPlayerCharacter.h"
#include "Framework/GCGameInstance.h"
#include "UserInterface/GCUserWidget.h"
#include "UserInterface/GCFrameRateWidget.h"
#include "UserInterface/Menus/GCPauseWidget.h"
#include "UserInterface/Gameplay/GCGameplayWidget.h"
#include "UserInterface/Gameplay/GCInventoryWidget.h"
#include "UserInterface/Gameplay/GCNarrativeWidget.h"
#include "UserInterface/Messaging/GCMessageWidget.h"
#include "UserSettings/GCUserSettings.h"
#include "PhotoMode/GCPhotoModeActor.h"
#include "Inventory/GCInspectionActor.h"
#include "Inventory/GCInventoryManager.h"
#include "Narrative/GCNarrativeComponent.h"
#include "Achievement/GCAchievementManager.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"

AGCPlayerController::AGCPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	bShouldPerformFullTickWhenPaused = true;

	AchievementManager = CreateDefaultSubobject<UGCAchievementManager>("AchievementManager");
	NarrativeComponent = CreateDefaultSubobject<UGCNarrativeComponent>("NarrativeComponent");
	InventoryManager = CreateDefaultSubobject<UGCInventoryManager>("InventoryManager");
	
	WidgetClasses = {};
	FrameRateWidgetClass = NULL;

	WidgetObjects = {};
	GameInstance = nullptr;
	FrameRateWidget = nullptr;
	PlayerCharacter = nullptr;
	PhotoModeActor = nullptr;
	InspectionActor = nullptr;

	bInScriptedEvent = false;
	bInInventory = false;
}

AGCPlayerController* AGCPlayerController::Get(const UObject* WorldContext)
{
	return Cast<AGCPlayerController>(UGameplayStatics::GetPlayerController(WorldContext, 0));
}

UGCUserWidget* AGCPlayerController::GetUserWidget(const TSubclassOf<UGCUserWidget> WidgetClass) const
{
	for (UGCUserWidget* Widget : WidgetObjects)
	{
		if (Widget && Widget->IsA(WidgetClass)) return Widget;
	}

	return nullptr;
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void AGCPlayerController::ToggleQuestsHidden()
{
	if (UGCNarrativeWidget* Widget = GetUserWidget<UGCNarrativeWidget>())
	{
		Widget->SetQuestsHidden(!Widget->AreQuestsHidden());
	}
}

void AGCPlayerController::EnterCutscene()
{
	EnterScriptedEvent(true);
	
	if (UGCNarrativeWidget* Widget = GetUserWidget<UGCNarrativeWidget>())
	{
		Widget->SetQuestsHidden(true);
	}
}

void AGCPlayerController::ExitCutscene()
{
	ExitScriptedEvent();
	SetViewTarget(PlayerCharacter);
	PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 0.5f, FLinearColor::Black);
	
	if (UGCNarrativeWidget* Widget = GetUserWidget<UGCNarrativeWidget>())
	{
		Widget->SetQuestsHidden(!Widget->GetNarrative()->IsQuestInProgress(UQuest::StaticClass()));
	}
}

void AGCPlayerController::OpenInventory()
{
	UGCInventoryWidget* Widget = GetUserWidget<UGCInventoryWidget>();
	if (!bInInventory && !Widget->IsInViewport())
	{
		SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, Widget, EMouseLockMode::LockAlways);
		PlayerCharacter->SetActiveState(EGCPlayerActiveState::Inventory);
		Widget->AddWidget([this](){
			bInInventory = true;
		}, true);
	}
}

void AGCPlayerController::CloseInventory()
{
	ExitInventoryInternal(nullptr);
}

void AGCPlayerController::PauseGame()
{
	UGCPauseWidget* PauseWidget = GetUserWidget<UGCPauseWidget>();
	if (!IsPaused() && !PauseWidget->IsInViewport())
	{
		auto PauseFunc = [this, PauseWidget](){
			SetShowMouseCursor(true);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, PauseWidget, EMouseLockMode::LockAlways);
			PauseWidget->AddWidget(nullptr, true);
			GetUserWidget<UGCMessageWidget>()->SetWidgetHidden(true);
			SetPause(true);
		};
		
		if (IsInInventory())
		{
			ExitInventoryInternal(PauseFunc);
		}
		else
		{
			PauseFunc();
		}
	}
}

void AGCPlayerController::UnpauseGame()
{
	if (IsPaused())
	{
		SetPause(false);
		SetShowMouseCursor(false);
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
		GetUserWidget<UGCPauseWidget>()->RemoveWidget(nullptr, true);
		GetUserWidget<UGCMessageWidget>()->SetWidgetHidden(false);
	}
}

void AGCPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GameInstance)
	{
		GameInstance->WorldTick(DeltaTime);
	}
	
	if (IsPaused())
	{
		UpdateCameraManager(DeltaTime);
	}
}

UEnhancedInputLocalPlayerSubsystem* AGCPlayerController::GetInputSubsystem() const
{
	return GetLocalPlayer() ? GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
}

void AGCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = GetPawn<AGCPlayerCharacter>();
	
	GameInstance = GetGameInstance<UGCGameInstance>();
	GameInstance->WorldBeginPlay();
	
	GetWorld()->bIsCameraMoveableWhenPaused = true;
	PlayerCameraManager->SetTickableWhenPaused(true);

	if (!PlayerCharacter || !PlayerCharacter->PhotoModeActor)
	{
		PhotoModeActor = Cast<AGCPhotoModeActor>(UGameplayStatics::GetActorOfClass(
			this, AGCPhotoModeActor::StaticClass()));
	}
	else
	{
		PhotoModeActor = PlayerCharacter->PhotoModeActor;
	}

	if (!PlayerCharacter || !PlayerCharacter->InspectionActor)
	{
		InspectionActor = Cast<AGCInspectionActor>(UGameplayStatics::GetActorOfClass(
			this, AGCInspectionActor::StaticClass()));
	}
	else
	{
		InspectionActor = PlayerCharacter->InspectionActor;
	}
	
	for (const TSubclassOf<UGCUserWidget>& WidgetClass : WidgetClasses)
	{
		WidgetObjects.Add(UGCUserWidget::Create(this, WidgetClass));
	}
	
	FrameRateWidget = CreateWidget<UGCFrameRateWidget>(this, FrameRateWidgetClass);
	FrameRateWidget->SetVisibility(ESlateVisibility::Collapsed);
	FrameRateWidget->AddToViewport(20);

	InventoryManager->OnEquip.AddUObject(this, &AGCPlayerController::OnItemEquipped);
	InventoryManager->OnUnequip.AddUObject(this, &AGCPlayerController::OnItemUnequipped);

	NarrativeComponent->OnDialogueBegan.AddDynamic(this, &AGCPlayerController::OnDialogueStarted);
	NarrativeComponent->OnDialogueFinished.AddDynamic(this, &AGCPlayerController::OnDialogueFinished);
	
	if (UGCUserSettings* Settings = UGCUserSettings::Get())
	{
		Settings->OnDynamicApply.AddUObject(this, &AGCPlayerController::OnDynamicSettingsApply);
		OnDynamicSettingsApply(Settings);
	}

	if (const UInputMappingContext* MappingContext = UGCSettings::Get()->InputMappingContext.LoadSynchronous())
	{
		GetInputSubsystem()->AddMappingContext(MappingContext, 0);
	}
}

void AGCPlayerController::ExitScriptedEvent()
{
	if (bInScriptedEvent)
	{
		bInScriptedEvent = false;

		if (AActor* EquippedActor = InventoryManager->GetActiveEquipment().Actor)
		{
			EquippedActor->SetActorHiddenInGame(false);
		}
		
		if (PlayerCharacter->GetActiveState() != EGCPlayerActiveState::Loading) 
		{
			PlayerCharacter->SetActiveState(EGCPlayerActiveState::Normal);
		}
	}
}

void AGCPlayerController::EnterScriptedEvent(const bool bHidePlayer)
{
	if (!bInScriptedEvent)
	{
		bInScriptedEvent = true;

		if (bHidePlayer)
		{
			if (AActor* EquippedActor = InventoryManager->GetActiveEquipment().Actor)
			{
				EquippedActor->SetActorHiddenInGame(true);
			}
		}
		
		if (bInInventory)
		{
			ExitInventoryInternal([this](){
			   PlayerCharacter->SetActiveState(EGCPlayerActiveState::Cutscene);
			});
		}
		else
		{
			PlayerCharacter->SetActiveState(EGCPlayerActiveState::Cutscene);
		}
	}
}

void AGCPlayerController::ExitInventoryInternal(const TFunction<void()>& OnFinished, const bool bImmediate)
{
	if (bInInventory)
	{
		SetShowMouseCursor(false);
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
		if (PlayerCharacter->GetActiveState() != EGCPlayerActiveState::Loading) 
		{
			PlayerCharacter->SetActiveState(EGCPlayerActiveState::Normal);
		}
		GetUserWidget<UGCInventoryWidget>()->RemoveWidget([this, OnFinished](){
			bInInventory = false;
			if (OnFinished)
				OnFinished();
		}, !bImmediate);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AGCPlayerController::OnItemEquipped(AActor* Actor)
{
	Actor->AttachToComponent(PlayerCharacter->EquipmentRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void AGCPlayerController::OnItemUnequipped(AActor* Actor)
{
	Actor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	Actor->K2_DestroyActor();
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void AGCPlayerController::OnDynamicSettingsApply(UGCUserSettings* InUserSettings)
{
	if (const UGCSettings* Settings = UGCSettings::Get())
	{
		UKismetMaterialLibrary::SetScalarParameterValue(this, Settings->MainRenderingMPC.LoadSynchronous(),
			Settings->BrightnessParamName, InUserSettings->GetBrightness() * 0.01f);
	}

	if (FrameRateWidget)
	{
		FrameRateWidget->SetVisibility(InUserSettings->GetShowFrameRate() ?
			ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void AGCPlayerController::OnDialogueStarted(UDialogue* Dialogue)
{
	EnterScriptedEvent(false);
	GCCharacter::OnEnteredDialogue(Dialogue->Instigator.Get());
	PlayerCharacter->SetLockOnTarget(GCCharacter::GetLookAtComponent(Dialogue->Instigator.Get()));
	GetUserWidget<UGCGameplayWidget>()->SetWidgetHidden(true);
	GetUserWidget<UGCMessageWidget>()->SetWidgetHidden(true);
	
	SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, GetUserWidget<UGCNarrativeWidget>(),
		EMouseLockMode::LockAlways, false);
}

void AGCPlayerController::OnDialogueFinished(UDialogue* Dialogue)
{
	ExitScriptedEvent();
	GCCharacter::OnExitedDialogue(Dialogue->Instigator.Get());
	PlayerCharacter->SetLockOnTarget(nullptr);
	GetUserWidget<UGCGameplayWidget>()->SetWidgetHidden(false);
	GetUserWidget<UGCMessageWidget>()->SetWidgetHidden(false);
	
	SetShowMouseCursor(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
}
