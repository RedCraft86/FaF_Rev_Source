// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GTDataUtilsLibrary.generated.h"

UCLASS(DisplayName = "Data Utils Library")
class GTCORE_API UGTDataUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "DataUtils")
		static FString EncryptAES(const FString& String, const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "DataUtils")
		static FString DecryptAES(const FString& String, const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "DataUtils")
		static TArray<uint8> CompressBytes(const TArray<uint8>& InBytes);

	UFUNCTION(BlueprintCallable, Category = "DataUtils")
		static TArray<uint8> DecompressBytes(const TArray<uint8>& InBytes);
};
