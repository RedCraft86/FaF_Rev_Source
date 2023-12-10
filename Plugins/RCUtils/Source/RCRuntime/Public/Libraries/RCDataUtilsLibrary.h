// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "RCEngineTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RCDataUtilsLibrary.generated.h"

UCLASS(DisplayName = "Data Utils Function Library")
class RCRUNTIME_API URCDataUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "DataUtils")
		static TArray<uint8> StringToBinary(const FString& InString);

	UFUNCTION(BlueprintPure, Category = "DataUtils", meta = (AutoCreateRefTerm = "InData"))
		static FString BinaryToString(const TArray<uint8>& InData);

	UFUNCTION(BlueprintPure, Category = "DataUtils")
		static TArray<uint8> HexToBinary(const FString& InHexString);
	
	UFUNCTION(BlueprintPure, Category = "DataUtils", meta = (AutoCreateRefTerm = "InData"))
		static FString BinaryToHex(const TArray<uint8>& InData);
	
	UFUNCTION(BlueprintPure, Category = "DataUtils|Compression", meta = (AutoCreateRefTerm = "InData"))
		static TArray<uint8> CompressArray(TArray<uint8> InData, const ERCArchiveCompressionFormat InFormat);

	UFUNCTION(BlueprintPure, Category = "DataUtils|Compression", meta = (AutoCreateRefTerm = "InData"))
		static TArray<uint8> DecompressArray(TArray<uint8> InData, const ERCArchiveCompressionFormat InFormat);
	
	UFUNCTION(BlueprintPure, Category = "DataUtils|Compression", meta = (AutoCreateRefTerm = "InData"))
		static TArray<uint8> OodleCompressArray(const TArray<uint8>& InData, const ERCOodleCompressor Compressor = ERCOodleCompressor::Kraken, const ERCOodleLevel Level = ERCOodleLevel::SuperFast);

	UFUNCTION(BlueprintPure, Category = "DataUtils|Compression", meta = (AutoCreateRefTerm = "InData"))
		static TArray<uint8> OodleDecompressArray(const TArray<uint8>& InData);

private:

	static FName ConvertArchiveCompressionFormat(const ERCArchiveCompressionFormat InFormat);
	static FOodleDataCompression::ECompressor ConvertOodleCompressor(const ERCOodleCompressor InCompressor);
	static FOodleDataCompression::ECompressionLevel ConvertOodleLevel(const ERCOodleLevel InCompressorLevel);
};
