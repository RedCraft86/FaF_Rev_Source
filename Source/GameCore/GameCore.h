// Copyright (C) Tayzar Linn. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FGameCoreModule final : public FDefaultGameModuleImpl
{
private:
	
public:

	/* IModuleInterface implementation. */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FString Encrypt(const FString& String)
	{
		if (String.IsEmpty()) return TEXT("");

		FString Str = String;
		Str.Append(TEXT("OWS#@!"));

		uint32 Size = Str.Len();
		Size = Size + (FAES::AESBlockSize - (Size % FAES::AESBlockSize));
		uint8* Byte = new uint8[Size];

		if (StringToBytes(Str, Byte, Size)) {

			FAES::EncryptData(Byte, Size, TCHAR_TO_ANSI(FMD5::HashAnsiString(*DefaultAES).GetCharArray().GetData()));
			Str = FString::FromHexBlob(Byte, Size);

			delete Byte;
			return Str;
		}

		delete Byte;
		return TEXT("");
	}

	static FString Decrypt(const FString& String)
	{
		if (String.IsEmpty()) return TEXT("");

		FString Str = String;

		uint32 Size = Str.Len();
		Size = Size + (FAES::AESBlockSize - (Size % FAES::AESBlockSize));
		uint8* Byte = new uint8[Size];

		if (FString::ToHexBlob(Str, Byte, Size)) {

			FAES::DecryptData(Byte, Size, TCHAR_TO_ANSI(FMD5::HashAnsiString(*DefaultAES).GetCharArray().GetData()));
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
