// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FGameCoreModule final : public FDefaultGameModuleImpl
{
public:

	/* IModuleInterface implementation. */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FString EncryptAES(const FString& String, const FString& Key)
	{
		if (String.IsEmpty()) return TEXT("");

		FString Str = String;
		Str.Append(TEXT("OWS#@!"));

		uint32 Size = Str.Len();
		Size = Size + (FAES::AESBlockSize - (Size % FAES::AESBlockSize));
		uint8* Byte = new uint8[Size];

		if (StringToBytes(Str, Byte, Size)) {

			FAES::EncryptData(Byte, Size, TCHAR_TO_ANSI(FMD5::HashAnsiString(*Key).GetCharArray().GetData()));
			Str = FString::FromHexBlob(Byte, Size);

			delete Byte;
			return Str;
		}

		delete Byte;
		return TEXT("");
	}

	static FString DecryptAES(const FString& String, const FString& Key)
	{
		if (String.IsEmpty()) return TEXT("");

		FString Str = String;

		uint32 Size = Str.Len();
		Size = Size + (FAES::AESBlockSize - (Size % FAES::AESBlockSize));
		uint8* Byte = new uint8[Size];

		if (FString::ToHexBlob(Str, Byte, Size)) {

			FAES::DecryptData(Byte, Size, TCHAR_TO_ANSI(FMD5::HashAnsiString(*Key).GetCharArray().GetData()));
			Str = BytesToString(Byte, Size);

			FString Result, R = TEXT("");
			Str.Split(TEXT("OWS#@!"), &Result, &R, ESearchCase::CaseSensitive);

			delete Byte;
			return Result;
		}

		delete Byte;
		return TEXT("");
	}
};
