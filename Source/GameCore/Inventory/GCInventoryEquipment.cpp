// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#include "GCInventoryEquipment.h"
#include "Compression/OodleDataCompressionUtil.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

AGCInventoryEquipment::AGCInventoryEquipment()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bCanEverSave = false;
	InventoryManager = nullptr;
	bCharging = false;
}

void AGCInventoryEquipment::LoadSaveData(const TArray<uint8>& InData)
{
	if (!bCanEverSave)
		return;
	
	TArray<uint8> BinaryData;
	if (FOodleCompressedArray::DecompressToTArray(BinaryData, InData))
	{
		FMemoryReader FromBinary(BinaryData, true);
		FObjectAndNameAsStringProxyArchive Archive(FromBinary, true);
		Archive.ArIsSaveGame = true;
		Archive.ArNoDelta = true;

		Serialize(Archive);

		FromBinary.FlushCache();
		FromBinary.Close();
	}
}

void AGCInventoryEquipment::GetSaveData(TArray<uint8>& OutData)
{
	if (!bCanEverSave)
		return;
	
	TArray<uint8> BinaryData;
	FMemoryWriter ToBinary(BinaryData, true);
	FObjectAndNameAsStringProxyArchive Archive(ToBinary, true);
	Archive.ArIsSaveGame = true;
	Archive.ArNoDelta = true;

	Serialize(Archive);

	ToBinary.FlushCache();
	ToBinary.Close();

	OutData.Empty();
	FOodleCompressedArray::CompressTArray(OutData, BinaryData,
		FOodleDataCompression::ECompressor::Kraken, FOodleDataCompression::ECompressionLevel::SuperFast);
}

void AGCInventoryEquipment::OnEquip()
{
	EventEquip(InventoryManager);
}

void AGCInventoryEquipment::OnUnequip()
{
	EventUnequip(InventoryManager);
}

void AGCInventoryEquipment::OnToggle()
{
	EventToggle();
}

void AGCInventoryEquipment::OnUpdateChargeState()
{
	EventUpdateChargeState(bCharging);
}
