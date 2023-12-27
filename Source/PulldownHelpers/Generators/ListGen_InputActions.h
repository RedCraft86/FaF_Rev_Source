// Copyright (C) RedCraft86 (Tayzar). All Rights Reserved.

#pragma once

#include "Core/GCSettings.h"
#include "ListGeneratorBase.h"
#include "InputMappingContext.h"
#include "TickableEditorObject.h"
#include "ListGen_InputActions.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct FMappingKeyList
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleInstanceOnly, Category = "MappingKeyList")
		TArray<FKey> Keys;

	FMappingKeyList() : Keys({}) {}
	
	void Add(const FKey& InKey)
	{
		Keys.AddUnique(InKey);
	}
};

UCLASS(DisplayName = "List Generator - Player Input")
class UGenerator_PlayerInputList final : public UListGeneratorBase, public FTickableEditorObject
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleInstanceOnly, Category = "Pulldown")
		TMap<FName, FMappingKeyList> ActionEventMappings;

private:
	
	virtual FName GetDataObjectPropertyName() const override { return GET_MEMBER_NAME_CHECKED(UGCSettings, InputMappingContext); }
	virtual TSoftObjectPtr<UObject> GetDataObject() const override { return UGCSettings::Get()->InputMappingContext; }
	virtual void UpdateObjectInfo(const UObject* InDataObject) override
	{
		Super::UpdateObjectInfo(InDataObject);
		if (const UInputMappingContext* Context = Cast<UInputMappingContext>(InDataObject))
		{
			ActionEventMappings.Empty();
			for (const FEnhancedActionKeyMapping& Mapping : Context->GetMappings())
			{
				if (!Mapping.Action || !Mapping.IsPlayerMappable() || Mapping.GetMappingName().IsNone())
					continue;

				ActionEventMappings.FindOrAdd(Mapping.Action.GetFName()).Add(Mapping.Key);
			}
		}
	}
	
	virtual TArray<TSharedPtr<FPulldownRow>> OnGenerate(UObject* InDataObject) const override
	{
		if (!InDataObject)
			return {};

		TArray<TSharedPtr<FPulldownRow>> Rows;
		if (const UInputMappingContext* Context = Cast<UInputMappingContext>(InDataObject))
		{
			for (const FEnhancedActionKeyMapping& Mapping : Context->GetMappings())
			{
				if (!Mapping.Action || !Mapping.IsPlayerMappable() || Mapping.GetMappingName().IsNone())
					continue;

				const FText Tooltip = Mapping.GetDisplayName().IsEmptyOrWhitespace() ?
					FText::FromName(Mapping.GetMappingName()) : Mapping.GetDisplayName();
					
				Rows.Add(MakeShared<FPulldownRow>(Mapping.GetMappingName().ToString(), Tooltip, FText::GetEmpty()));
			}
		}

		return Rows;
	}

	virtual void Tick(float DeltaTime) override
	{
		if (!bInitialized)
		{
			CacheMappings(PreChangeRowNames);
			bInitialized = true;
		}

		UpdateObjectInfo(DataObjectPtr.LoadSynchronous());
	
		TArray<FName> PostChangeRowNames;
		CacheMappings(PostChangeRowNames);
	
		if (NotifyPulldownRowChanged(PreChangeRowNames, PostChangeRowNames))
		{
			PreChangeRowNames = MoveTemp(PostChangeRowNames);
		}
	}
	
	virtual bool IsTickable() const override { return !IsTemplate() && DataObjectPtr.IsValid(); };
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UGenerator_PlayerInputList, STATGROUP_Tickables); };
	
	void CacheMappings(TArray<FName>& Mappings) const
	{
		if (!IsValid(DataObjectPtr.LoadSynchronous()))
			return;

		Mappings.Empty();
		if (const UInputMappingContext* MappingContext = Cast<UInputMappingContext>(DataObjectPtr.LoadSynchronous()))
		{
			for (const FEnhancedActionKeyMapping& Mapping : MappingContext->GetMappings())
			{
				if (!Mapping.Action || !Mapping.IsPlayerMappable() || Mapping.GetMappingName().IsNone())
					continue;
			
				Mappings.Add(Mapping.GetMappingName());
			}
		}
	}

	UPROPERTY(Transient)
		bool bInitialized = false;
};