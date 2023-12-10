// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "GCSaveObjects.h"
#include "Misc/FileHelper.h"
#include "Compression/OodleDataCompressionUtil.h"
#include "Serialization/BufferArchive.h"
#include "Async/Async.h"
#include "GCSaveTypes.h"

UGCSaveObjectBase::UGCSaveObjectBase()
{
	FileName = TEXT("");
	LastError = EGCSaveGameError::None;
}

void UGCSaveObjectBase::SaveGame(const TFunction<void(const EGCSaveGameError)>& Callback)
{
	OnPreSave();
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this, Callback](float)->bool{
		FBufferArchive ToBinary(true);
		SerializeData(ToBinary);
		
		TArray<uint8> CompressedData;
		if (!FOodleCompressedArray::CompressTArray(CompressedData, ToBinary,
			FOodleDataCompression::ECompressor::Kraken,
			FOodleDataCompression::ECompressionLevel::SuperFast))
		{
			LastError = EGCSaveGameError::Compress;
			if (Callback) { Callback(LastError); }
			return false;
		}

		ToBinary.FlushCache();
		ToBinary.Close();

		AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, Callback, CompressedData](){
			const bool bWasSuccess = FFileHelper::SaveArrayToFile(CompressedData, *GetSaveFilePath());
			LastError = bWasSuccess ? EGCSaveGameError::None : EGCSaveGameError::FileWrite;
			if (Callback) { Callback(LastError); }
		});

		return false;
	}), 0.0f);
}

void UGCSaveObjectBase::LoadGame(const TFunction<void(const EGCSaveGameError)>& Callback)
{
	if (!FPaths::FileExists(GetSaveFilePath()))
	{
		SaveGame(Callback);
		return;
	}
	
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, Callback](){
		TArray<uint8> CompressedData;
		if (!FFileHelper::LoadFileToArray(CompressedData, *GetSaveFilePath()))
		{
			LastError = EGCSaveGameError::FileRead;
			if (Callback) { Callback(LastError); }
			return;
		}
		
		AsyncTask(ENamedThreads::GameThread, [this, Callback, CompressedData](){
			TArray<uint8> UncompressedData;
			if (!FOodleCompressedArray::DecompressToTArray(UncompressedData, CompressedData))
			{
				LastError = EGCSaveGameError::Decompress;
				if (Callback) { Callback(LastError); }
				return;
			}
			
			FMemoryReader FromBinary(UncompressedData, true);
			FromBinary.Seek(0);
			
			SerializeData(FromBinary);

			FromBinary.FlushCache();
			FromBinary.Close();

			LastError = EGCSaveGameError::None;
			if (Callback) { Callback(LastError); }
		});
	});
}

void UGCSaveObjectBase::DeleteGame()
{
	LastError = EGCSaveGameError::None;
	SaveGame(nullptr);
}

UGCSaveObjectGame::UGCSaveObjectGame()
{
	Difficulty = 1;
	GameSequenceID = NAME_None;
	TotalPlayTime = 0.0f;
	StartDate = FDateTime(0);
	EndDate = FDateTime(0);

	bShouldSaveTime = false;
	LastSavedTime = FDateTime(0);
}

void UGCSaveObjectGame::AllowTimeSaving()
{
	if (!bShouldSaveTime)
	{
		bShouldSaveTime = true;
		LastSavedTime = FDateTime::Now();
	}
}

UGCSaveObjectGame* UGCSaveObjectGame::CreateOrLoad(UObject* Outer, const FString& InFileName)
{
	checkf(!InFileName.IsEmpty(), TEXT("Filename cannot be empty."));
	if (UGCSaveObjectGame* SaveObject = NewObject<UGCSaveObjectGame>(Outer))
	{
		SaveObject->FileName = InFileName;
		SaveObject->LoadGame(nullptr);
		return SaveObject;
	}

	checkf(0, TEXT("Failed to create or load a save object for '%s'. How did this even happen?"), *InFileName);
	return nullptr;
}

void UGCSaveObjectGame::OnPreSave()
{
	if (bShouldSaveTime)
	{
		if (LastSavedTime.GetTicks() > 0)
		{
			const FDateTime ThisTime = FDateTime::Now();
			const FTimespan Difference = ThisTime - LastSavedTime;
			LastSavedTime = ThisTime;
		
			TotalPlayTime += Difference.GetTotalSeconds();
		}
		else
		{
			LastSavedTime = FDateTime::Now();
		}
	}
}

UGCSaveObjectPersistent::UGCSaveObjectPersistent()
{
	UnlockedContent = {};
	Achievements = {};
}

UGCSaveObjectPersistent* UGCSaveObjectPersistent::CreateOrLoad(UObject* Outer, const FString& InFileName)
{
	checkf(!InFileName.IsEmpty(), TEXT("Filename cannot be empty."));
	if (UGCSaveObjectPersistent* SaveObject = NewObject<UGCSaveObjectPersistent>(Outer))
	{
		SaveObject->FileName = InFileName;
		SaveObject->LoadGame(nullptr);
		return SaveObject;
	}

	checkf(0, TEXT("Failed to create or load a save object for '%s'. How did this even happen?"), *InFileName);
	return nullptr;
}
