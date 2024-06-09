// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#if UE_BUILD_SHIPPING
#include "GeneralProjectSettings.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#endif

void UGTGameInstance::SetUnlitViewMode(const bool bUnlit)
{
	const UWorld* World = GetWorld();
	if (UGameViewportClient* Viewport = World ? World->GetGameViewport() : nullptr)
	{
		CachedVMI = bUnlit ? VMI_Unlit : VMI_Lit;
		Viewport->ViewModeIndex = CachedVMI;
	}
}

void UGTGameInstance::OnWorldBeginPlay(UWorld* InWorld)
{
	if (UGameViewportClient* Viewport = InWorld->GetGameViewport())
	{
		Viewport->ViewModeIndex = CachedVMI;
	}
	
	WorldBeginPlay();
}

void UGTGameInstance::OnWorldTick(const float DeltaTime)
{
	WorldTick(DeltaTime);
}

void UGTGameInstance::Init()
{
	Super::Init();
#if UE_BUILD_SHIPPING
	// Only one instance of the game can be initialized!
	if (const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>())
	{
		const FString LockFilePath = FPlatformProcess::UserTempDir() + ProjectSettings->ProjectID.ToString();
		if (!IFileManager::Get().CreateFileWriter(*LockFilePath, 0))
		{
			FPlatformApplicationMisc::RequestMinimize();
			FPlatformMisc::RequestExit(false);
		}
	}
#endif
}

UGTGameInstance* UGTGameInstance::GetGameInstanceSmart(const UObject* WorldContextObject, const TSubclassOf<UGTGameInstance> Class)
{
	UGTGameInstance* Obj = Cast<UGTGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	return Obj && Obj->IsA(Class) ? Obj : nullptr;
}

EGTValidPins UGTGameInstance::GetGameInstanceChecked(UGTGameInstance*& OutObject, const UObject* WorldContextObject, const TSubclassOf<UGTGameInstance> Class)
{
	OutObject = GetGameInstanceSmart(WorldContextObject, Class);
	return IsValid(OutObject) ? EGTValidPins::Valid : EGTValidPins::NotValid;
}
