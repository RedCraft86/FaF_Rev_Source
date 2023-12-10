// Copyright (C) Tayzar (RedCraft86). All Rights Reserved.

#pragma once

#include "GCSequenceTypes.h"
#include "Core/GCSettings.h"
#include "Engine/DataAsset.h"
#include "GCSequenceOrderAsset.generated.h"

UCLASS(NotBlueprintable, BlueprintType, DisplayName = "Sequence Order Asset")
class GAMECORE_API UGCSequenceOrderAsset final : public UDataAsset
{
	GENERATED_BODY()

public:

	UGCSequenceOrderAsset()
	{
#if WITH_EDITOR
		FillMissingEntries();
#endif
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SequenceOrder", EditFixedSize, meta = (NoElementDuplicate))
		TArray<FGCSequenceID> OrderedSequences;

	FGCSequenceID GetLastID(const FGCSequenceID& InCurrent) const
	{
		return GetSequenceAtIndex(OrderedSequences.Find(InCurrent) - 1);
	}

	FGCSequenceID GetNextID(const FGCSequenceID& InCurrent) const
	{
		return GetSequenceAtIndex(OrderedSequences.Find(InCurrent) + 1);
	}

	int32 GetPercentByID(const FGCSequenceID& InSequence) const
	{
		const int32 ClampedIdx = FMath::Clamp(OrderedSequences.Find(InSequence), 0, OrderedSequences.Num());
		return FMath::TruncToInt32(100.0f * (ClampedIdx + 1.0f) / (float)OrderedSequences.Num());
	}
	
private:
	
	FGCSequenceID GetSequenceAtIndex(const int32 InIndex) const
	{
		const int32 ClampedIdx = FMath::Clamp(InIndex, 0, OrderedSequences.Num() - 1);
		return OrderedSequences.IsValidIndex(ClampedIdx) ? OrderedSequences[ClampedIdx] : FGCSequenceID();
	}

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "SequenceOrder", meta = (DisplayPriority = -1))
		bool bRecheck = false;
#endif
#if WITH_EDITOR
	static TArray<FName> GetDataTableRows()
	{
		if (const UDataTable* DataTable = UGCSettings::Get()->GameSequenceTable.LoadSynchronous())
		{
			return DataTable->GetRowNames();
		}
		
		return {};
	}
	
	void FixBadEntries()
	{
		const TArray<FName> Rows = GetDataTableRows();
		if (Rows.IsEmpty())
		{
			OrderedSequences.Empty();
			return;
		}

		TArray<FGCSequenceID> Result = {};
		for (const FGCSequenceID& ID : OrderedSequences)
		{
			if (Rows.Contains(*ID) && !Result.Contains(ID))
			{
				Result.Add(ID);
			}
		}

		OrderedSequences = Result;
	}
	
	void FillMissingEntries()
	{
		const TArray<FName> Rows = GetDataTableRows();
		if (Rows.IsEmpty())
			return;
		
		for (const FName& Row : Rows)
		{
			if (!OrderedSequences.Contains(Row))
			{
				OrderedSequences.Add(Row);
			}
		}
	}

	// ReSharper disable once CppParameterMayBeConstPtrOrRef
	void OnSettingsChanged(UObject* Object, struct FPropertyChangedEvent& Property)
	{
		if (Property.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UGCSettings, GameSequenceTable))
		{
			FixBadEntries();
			FillMissingEntries();
		}
	}

	virtual void PostInitProperties() override
	{
		Super::PostInitProperties();
		
		if (FApp::IsGame())
		{
			UGCSettings::Get()->OnSettingChanged().RemoveAll(this);
		}
		else
		{
			UGCSettings::Get()->OnSettingChanged().AddUObject(this, &UGCSequenceOrderAsset::OnSettingsChanged);
		}
	}
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
#if WITH_EDITORONLY_DATA
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UGCSequenceOrderAsset, bRecheck))
		{
			FixBadEntries();
			FillMissingEntries();
			bRecheck = false;
		}
#endif
	}
#endif
};
