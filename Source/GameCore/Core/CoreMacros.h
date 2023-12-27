// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "CoreLogging.h"

#define WEAK_THIS WeakThis = TWeakObjectPtr<ThisClass>(this)

#define LOG(Verbosity, Format, ...) { UE_LOG(GameCore, Verbosity, Format, __VA_ARGS__); }
#define PRINT(Color, Time, Format, ...) { GEngine->AddOnScreenDebugMessage(-1, Time, FColor::Color, FString::Printf(Format, __VA_ARGS__)); }
#define MESSAGE(Verbosity, Message) { FFrame::KismetExecutionMessage(Message, ELogVerbosity::Verbosity); }

#define UE_REPORT_ERROR(Category, Time, Format, ...) \
	UE_LOG(Category, Error, Format, __VA_ARGS__); \
	PRINT(Red, Time, Format, __VA_ARGS__);

#define UE_REPORT_WARNING(Category, Time, Format, ...) \
	UE_LOG(Category, Warning, Format, __VA_ARGS__); \
	PRINT(Orange, Time, Format, __VA_ARGS__);

#define SETUP_PULLDOWN(ThisStruct) \
	ThisStruct() {} \
	ThisStruct(const FName& InValue) { SelectedValue = InValue; } \
	FORCEINLINE bool operator==(const ThisStruct& Other) const { return SelectedValue == Other.SelectedValue; } \
	FORCEINLINE bool operator!=(const ThisStruct& Other) const { return SelectedValue != Other.SelectedValue; } \
	friend FArchive& operator<<(FArchive& Ar, ThisStruct& Pulldown) \
	{ \
		Ar << Pulldown.SelectedValue; \
		return Ar; \
	} \
	void Set(const FName& InValue) { SelectedValue = InValue; } \
	FORCEINLINE bool IsValid() const { return !SelectedValue.IsNone(); }

#define SETUP_PULLDOWN_HASH(ThisStruct) \
	FORCEINLINE uint32 GetTypeHash(const ThisStruct& Pulldown) { return GetTypeHash(Pulldown.SelectedValue); } 