// Copyright (C) RedCraft86. All Rights Reserved.

#include "SaveObjects.h"
#include "Serialization/BufferArchive.h"
#include "Compression/OodleDataCompressionUtil.h"
#include "FRSettings.h"

FString USaveObjectBase::GetSavePath() const
{
	return FPaths::ProjectSavedDir() / TEXT("SaveGame/") + (FRSettings->bIsDemo ? TEXT("Demo") : TEXT("")) / GetSaveFileName();
}

void USaveObjectBase::SaveToFile(const TFunction<void(const ESaveGameError)>& Callback)
{
	FBufferArchive ToBinary(true);
	SerializeData(ToBinary);

	TArray<uint8> CompressedData;
	if (!FOodleCompressedArray::CompressTArray(CompressedData, ToBinary,
		FOodleDataCompression::ECompressor::Kraken,
		FOodleDataCompression::ECompressionLevel::SuperFast))
	{
		LastError = ESaveGameError::Compress;
		if (Callback) Callback(LastError);

		SMART_LOG(Error, TEXT("Failed to save data: Compression error"));
		return;
	}

	ToBinary.FlushCache();
	ToBinary.Close();

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, Callback, CompressedData](){
		if (!FFileHelper::SaveArrayToFile(CompressedData, *GetSavePath()))
		{
			LastError = ESaveGameError::FileWrite;
			if (Callback) Callback(LastError);

			SMART_LOG(Error, TEXT("Failed to save data: File Write error"));
			return;
		}
		
		LastError = ESaveGameError::None;
		if (Callback) Callback(LastError);
	});
}

void USaveObjectBase::LoadFromFile(const TFunction<void(const ESaveGameError)>& Callback)
{
	if (!FPaths::FileExists(GetSavePath()))
	{
		SaveToFile(Callback);
		return;
	}
	
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, Callback](){
		TArray<uint8> CompressedData;
		if (!FFileHelper::LoadFileToArray(CompressedData, *GetSavePath()))
		{
			LastError = ESaveGameError::FileRead;
			if (Callback) Callback(LastError);

			SMART_LOG(Error, TEXT("Failed to load data: File Read error"));
			return;
		}
		
		AsyncTask(ENamedThreads::GameThread, [this, Callback, CompressedData](){
			TArray<uint8> UncompressedData;
			if (!FOodleCompressedArray::DecompressToTArray(UncompressedData, CompressedData))
			{
				LastError = ESaveGameError::Decompress;
				if (Callback) Callback(LastError);

				SMART_LOG(Error, TEXT("Failed to load data: Decompression error"));
				return;
			}
			
			FMemoryReader FromBinary(UncompressedData, true);
			FromBinary.Seek(0);
			
			SerializeData(FromBinary);

			FromBinary.FlushCache();
			FromBinary.Close();

			LastError = ESaveGameError::None;
			if (Callback) Callback(LastError);
		});
	});
}

void USaveObjectBase::DeleteFile()
{
	IFileManager::Get().Delete(*GetSavePath());
	LastError = ESaveGameError::None;
}

void UGameSaveObject::DeleteFile()
{
	Super::DeleteFile();
	PlayTime = 0.0f;
	Sequence.Empty();
	Inventory.Empty();
}

void UGameSaveObject::SerializeData(FArchive& Ar)
{
	Ar << PlayTime;
	Ar << Sequence;
	Ar << Inventory;
}

void UGlobalSaveObject::DeleteFile()
{
	SMART_LOG(Error, TEXT("Cannot delete global persistent data."));
}

void UGlobalSaveObject::SerializeData(FArchive& Ar)
{
	Ar << Endings;
	Ar << Content;
}
