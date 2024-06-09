// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#define LOG_THIS_LINE(Category, Verbosity) \
	UE_LOG(Category, Verbosity, TEXT("[File: %hs] [Line: %d]"), __FILE__, __LINE__);

#define UE_PRINT(Time, Color, Format, ...) \
	GEngine->AddOnScreenDebugMessage(-1, Time, FColor::Color, FString::Printf(Format, __VA_ARGS__));

#define UE_LOG_KISMET(Verbosity, Format, ...) \
	FFrame::KismetExecutionMessage(*FString::Printf(Format, __VA_ARGS__), ELogVerbosity::Verbosity);

/* Reports - Print + Basic Log */
#define UE_LOG_MESSAGE(CategoryName, Time, Format, ...) \
	UE_PRINT(Time, Green, Format, __VA_ARGS__) \
	UE_LOG(CategoryName, Log, Format, __VA_ARGS__)

#define UE_LOG_WARNING(CategoryName, Time, Format, ...) \
	UE_PRINT(Time, Orange, Format, __VA_ARGS__) \
	UE_LOG(CategoryName, Warning, Format, __VA_ARGS__)

#define UE_LOG_ERROR(CategoryName, Time, Format, ...) \
	UE_PRINT(Time, Red, Format, __VA_ARGS__) \
	UE_LOG(CategoryName, Error, Format, __VA_ARGS__)
/* ~Reports - Print + Basic Log */

/* Reports - Print + Kismet Log */
#define UE_KLOG_MESSAGE(Time, Format, ...) \
	UE_PRINT(Time, Green, Format, __VA_ARGS__) \
	UE_LOG_KISMET(Log, Format, __VA_ARGS__)

#define UE_KLOG_WARNING(Time, Format, ...) \
	UE_PRINT(Time, Orange, Format, __VA_ARGS__) \
	UE_LOG_KISMET(Warning, Format, __VA_ARGS__)

#define UE_KLOG_ERROR(Time, Format, ...) \
	UE_PRINT(Time, Red, Format, __VA_ARGS__) \
	UE_LOG_KISMET(Error, Format, __VA_ARGS__)
/* ~Reports - Print + Kismet Log */
