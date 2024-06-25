// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "PulldownTypes.generated.h"

USTRUCT()
struct GTCORE_API FPulldownEdData final
{
	GENERATED_BODY()
#if WITH_EDITORONLY_DATA
	friend struct FStringPulldown;
	friend class FStringPulldownDetails;
	FPulldownEdData() : Options({}) {}

	void ClearOptions() { Options.Empty(); }
	void ReserveOptions(const int32 Number) { Options.Reserve(Number); }
	void RemoveOption(const FString& InOption) { Options.Remove(InOption); }
	void AddOption(const TPair<FString, FString>& InPair) { AddOption(InPair.Key, InPair.Value); }
	void AddOption(const FString& InOption, const FString& InTooltip) { Options.Add(InOption, InTooltip); }
	FString FindTooltip(const FString& InOption) const { return Options.FindRef(InOption); }
	bool HasOption(const FString& InOption) const { return Options.Contains(InOption); }
	
	void MarkOptionsChanged() { OnListChanged.ExecuteIfBound(this); }

private:
	TSortedMap<FString, FString> Options;
	TDelegate<void(FPulldownEdData*)> OnListChanged;
#else
	FPulldownEdData() {}
#endif
};

#define STRING_PULLDOWN_OVERRIDES(Type) \
	FORCEINLINE virtual Type& operator=(const FString& String) { Value = String; return *this; } \
	FORCEINLINE bool operator==(const Type& Other) const { return Value.Equals(Other.Value); } \
	FORCEINLINE bool operator!=(const Type& Other) const { return !(*this == Other); }

USTRUCT(BlueprintInternalUseOnly)
struct GTCORE_API FStringPulldown
{
	GENERATED_BODY()

	FStringPulldown() : Value(TEXT("None")) {}
	FStringPulldown(const FString& InValue) : Value(InValue) {}
	virtual ~FStringPulldown() = default;
	STRING_PULLDOWN_OVERRIDES(FStringPulldown)
	FORCEINLINE FString operator*() const { return Value; }
	
	FORCEINLINE FString Get() const { return Value; }
	FORCEINLINE bool IsEmpty() const { return Value.IsEmpty(); }
	FORCEINLINE bool IsNone() const { return Value == TEXT("None"); }
	FORCEINLINE bool IsValid() const { return !IsEmpty() && !IsNone(); }
	FORCEINLINE virtual void Set(const FString& InString) { Value = InString; }

#if WITH_EDITORONLY_DATA
	FPulldownEdData EdData;
	virtual TSortedMap<FString, FString> GetPulldownOptions() { return EdData.Options; }
#endif
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown", meta = (AllowPrivateAccess = true))
		FString Value;
};
