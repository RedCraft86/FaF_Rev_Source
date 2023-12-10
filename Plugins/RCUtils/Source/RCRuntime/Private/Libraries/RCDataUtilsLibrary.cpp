// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#include "RCDataUtilsLibrary.h"
#include "Compression/OodleDataCompressionUtil.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"

TArray<uint8> URCDataUtilsLibrary::StringToBinary(const FString& InString)
{
	TArray<uint8> Bytes;
	Bytes.SetNumUninitialized(InString.Len());
	StringToBytes(InString, Bytes.GetData(), InString.Len());
	return Bytes;
}

FString URCDataUtilsLibrary::BinaryToString(const TArray<uint8>& InData)
{
	return BytesToString(InData.GetData(), InData.Num());
}

TArray<uint8> URCDataUtilsLibrary::HexToBinary(const FString& InHexString)
{
	TArray<uint8> Bytes;
	HexToBytes(InHexString, Bytes.GetData());
	return Bytes;
}

FString URCDataUtilsLibrary::BinaryToHex(const TArray<uint8>& InData)
{
	return BytesToHex(InData.GetData(), InData.Num());
}

TArray<uint8> URCDataUtilsLibrary::CompressArray(TArray<uint8> InData, const ERCArchiveCompressionFormat InFormat)
{
	TArray<uint8> CompressedData;
	FArchiveSaveCompressedProxy Compressor(CompressedData, ConvertArchiveCompressionFormat(InFormat));
	if (Compressor.GetError())
	{
		return InData;
	}

	Compressor << InData;
	Compressor.Flush();
	
	Compressor.FlushCache();
	Compressor.Close();

	return CompressedData;
}

TArray<uint8> URCDataUtilsLibrary::DecompressArray(TArray<uint8> InData, const ERCArchiveCompressionFormat InFormat)
{
	FArchiveLoadCompressedProxy Decompressor(InData, ConvertArchiveCompressionFormat(InFormat));
	if (Decompressor.GetError())
	{
		return InData;
	}

	TArray<uint8> DecompressedData;
	Decompressor << DecompressedData;

	Decompressor.FlushCache();
	Decompressor.Close();

	return DecompressedData;
}

TArray<uint8> URCDataUtilsLibrary::OodleCompressArray(const TArray<uint8>& InData, const ERCOodleCompressor Compressor, const ERCOodleLevel Level)
{
	TArray<uint8> CompressedData;
	if (!FOodleCompressedArray::CompressTArray(CompressedData, InData, ConvertOodleCompressor(Compressor), ConvertOodleLevel(Level)))
	{
		return InData;	
	}
	
	return CompressedData;
}

TArray<uint8> URCDataUtilsLibrary::OodleDecompressArray(const TArray<uint8>& InData)
{
	TArray<uint8> DecompressedData;
	if (!FOodleCompressedArray::DecompressToTArray(DecompressedData, InData))
	{
		return InData;	
	}
	
	return DecompressedData;
}

FName URCDataUtilsLibrary::ConvertArchiveCompressionFormat(const ERCArchiveCompressionFormat InFormat)
{
	switch (InFormat)
	{
	case ERCArchiveCompressionFormat::Zlib:
		return NAME_Zlib;
		
	case ERCArchiveCompressionFormat::Gzip:
		return NAME_Gzip;

	default: return NAME_Zlib;
	}
}

FOodleDataCompression::ECompressor URCDataUtilsLibrary::ConvertOodleCompressor(const ERCOodleCompressor InCompressor)
{
	switch (InCompressor)
	{
	case ERCOodleCompressor::Selkie:
		return FOodleDataCompression::ECompressor::Selkie;

	case ERCOodleCompressor::Mermaid:
		return FOodleDataCompression::ECompressor::Mermaid;

	case ERCOodleCompressor::Kraken:
		return FOodleDataCompression::ECompressor::Kraken;

	case ERCOodleCompressor::Leviathan:
		return FOodleDataCompression::ECompressor::Leviathan;

	default: return FOodleDataCompression::ECompressor::Kraken;
	}
}

FOodleDataCompression::ECompressionLevel URCDataUtilsLibrary::ConvertOodleLevel(const ERCOodleLevel InCompressorLevel)
{
	switch (InCompressorLevel)
	{
	case ERCOodleLevel::HyperFast4:
		return FOodleDataCompression::ECompressionLevel::HyperFast4;

	case ERCOodleLevel::HyperFast3:
		return FOodleDataCompression::ECompressionLevel::HyperFast3;

	case ERCOodleLevel::HyperFast2:
		return FOodleDataCompression::ECompressionLevel::HyperFast2;

	case ERCOodleLevel::HyperFast1:
		return FOodleDataCompression::ECompressionLevel::HyperFast1;

	case ERCOodleLevel::SuperFast:
		return FOodleDataCompression::ECompressionLevel::SuperFast;

	case ERCOodleLevel::VeryFast:
		return FOodleDataCompression::ECompressionLevel::VeryFast;

	case ERCOodleLevel::Fast:
		return FOodleDataCompression::ECompressionLevel::Fast;

	case ERCOodleLevel::Normal:
		return FOodleDataCompression::ECompressionLevel::Normal;

	case ERCOodleLevel::Optimal1:
		return FOodleDataCompression::ECompressionLevel::Optimal1;

	case ERCOodleLevel::Optimal2:
		return FOodleDataCompression::ECompressionLevel::Optimal2;

	case ERCOodleLevel::Optimal3:
		return FOodleDataCompression::ECompressionLevel::Optimal3;

	case ERCOodleLevel::Optimal4:
		return FOodleDataCompression::ECompressionLevel::Optimal4;

	default: return FOodleDataCompression::ECompressionLevel::SuperFast;
	}
}
