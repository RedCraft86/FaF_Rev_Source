// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCAchievementManager.h"
#include "Player/GCPlayerController.h"
#include "SaveSystem/GCSaveManager.h"
#include "SaveSystem/GCSaveObjects.h"
#include "Core/GCSettings.h"

UGCAchievementManager::UGCAchievementManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

UGCAchievementManager* UGCAchievementManager::Get(const UObject* WorldContext)
{
	const AGCPlayerController* PC = AGCPlayerController::Get(WorldContext);
	return PC ? PC->GetAchievementManager() : nullptr;
}

void UGCAchievementManager::AddAchievementProgress(const FGCAchievementID InAchievementID, const int32 InProgress) const
{
	if (!InAchievementID.IsValid())
		return;
	
	if (const UGCSaveManager* SaveManager = UGCSaveManager::Get(this))
	{
		const uint8 Initial = SaveManager->GetPersistentData()->Achievements.FindRef(*InAchievementID);
		SaveManager->GetPersistentData()->Achievements.Add(*InAchievementID, Initial + InProgress);
		SaveManager->SavePersistentData(nullptr);

		const FIntPoint Progress = GetAchievementProgress(InAchievementID);
		if (Initial < Progress.Y && Progress.X >= Progress.Y)
		{
			OnAchievementComplete.Broadcast(InAchievementID);
		}
	}
}

FIntPoint UGCAchievementManager::GetAchievementProgress(const FGCAchievementID InAchievementID) const
{
	if (!InAchievementID.IsValid())
	{
		return FIntPoint::ZeroValue;
	}
	
	if (const UGCSaveManager* SaveManager = UGCSaveManager::Get(this))
	{
		const uint8 Max = GetAchievementData(InAchievementID).MaxProgress;
		const uint8 Current = SaveManager->GetPersistentData()->Achievements.FindOrAdd(*InAchievementID, 0);
		return FIntPoint(Current, Max);
	}

	return FIntPoint::ZeroValue;
}

bool UGCAchievementManager::IsAchievementComplete(const FGCAchievementID InAchievementID) const
{
	const FIntPoint Progress = GetAchievementProgress(InAchievementID);
	return Progress.Y > 0 ? Progress.X >= Progress.Y : false;
}

bool UGCAchievementManager::IsValidAchievementID(const FGCAchievementID InAchievementID)
{
	if (!InAchievementID.IsValid())
	{
		return false;
	}
	
	if (const UDataTable* Table = UGCSettings::Get()->AchievementTable.LoadSynchronous())
	{
		return Table->GetRowMap().Contains(*InAchievementID);
	}

	return false;
}

FGCAchievementData UGCAchievementManager::GetAchievementData(const FGCAchievementID InAchievementID)
{
	if (!InAchievementID.IsValid())
	{
		return {};
	}
	
	if (const UDataTable* Table = UGCSettings::Get()->AchievementTable.LoadSynchronous())
	{
		if (const FGCAchievementData* DataPtr = Table->FindRow<FGCAchievementData>(*InAchievementID, StaticClass()->GetName()))
		{
			return *DataPtr;
		}
	}

	return {};
}

bool UGCAchievementManager::IsValidAchievementData(const FGCAchievementData& InItemData)
{
	return InItemData.IsValidData();
}

TArray<FGCAchievementID> UGCAchievementManager::GetSortedAchievementIDs()
{
	TArray<FGCAchievementID> Result;
	if (const UDataTable* Table = UGCSettings::Get()->AchievementTable.LoadSynchronous())
	{
		TArray<FName> Names = Table->GetRowNames();
		Names.Sort([](const FName& A, const FName& B)->bool{ return A.ToString() < B.ToString(); });
		
		for (const FName& Name : Names)
		{
			Result.Add(Name);
		}
	}

	return Result;
}
