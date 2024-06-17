// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "PulldownTypes.generated.h"

USTRUCT()
struct GTCORE_API FPulldownOption final
{
	GENERATED_BODY()
#if WITH_EDITORONLY_DATA
	UPROPERTY() FString Option;
	UPROPERTY() FString Tooltip;
	FPulldownOption() : Option(TEXT("")), Tooltip(TEXT("")) {}
	FPulldownOption(const FString& InOption, const FString& InTooltip)
		: Option(InOption), Tooltip(InTooltip) {}

	FORCEINLINE bool operator==(const FPulldownOption& Other) const
	{
		return Option.Equals(Other.Option);
	}

	FORCEINLINE bool operator!=(const FPulldownOption& Other) const
	{
		return !(*this == Other);
	}
#else
	FPulldownOption() {}
#endif
};

USTRUCT(BlueprintInternalUseOnly)
struct GTCORE_API FStringPulldown
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
		FString Value;

	FStringPulldown() : Value(TEXT("")) {}
	virtual ~FStringPulldown() = default;

#if WITH_EDITORONLY_DATA
	virtual TArray<FPulldownOption> GetPulldownOptions() { return {}; }
#endif
};

USTRUCT(BlueprintInternalUseOnly)
struct GTCORE_API FStringListPulldown : public FStringPulldown
{
	GENERATED_BODY()
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient, meta = (TransientToolProperty)) TArray<FPulldownOption> Options = {};
	virtual TArray<FPulldownOption> GetPulldownOptions() override { return Options; }
#endif
};

USTRUCT(BlueprintInternalUseOnly)
struct GTCORE_API FNamePulldown
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
		FName Value;

	FNamePulldown() : Value(NAME_None) {}
	virtual ~FNamePulldown() = default;

#if WITH_EDITORONLY_DATA
	virtual TArray<FPulldownOption> GetPulldownOptions() { return {}; }
#endif
};

USTRUCT(BlueprintInternalUseOnly)
struct GTCORE_API FNameListPulldown : public FNamePulldown
{
	GENERATED_BODY()
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient, meta = (TransientToolProperty)) TArray<FPulldownOption> Options = {};
	virtual TArray<FPulldownOption> GetPulldownOptions() override { return Options; }
#endif
};
