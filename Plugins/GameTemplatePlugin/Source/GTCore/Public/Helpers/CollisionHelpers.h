// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once 

namespace CollisionHelpers
{
	static bool IsValidChannel(const ECollisionChannel InChannel)
	{
		return InChannel != ECC_OverlapAll_Deprecated && InChannel != ECC_MAX; 
	}

	static bool IsHiddenChannel(const ECollisionChannel InChannel)
	{
#if WITH_EDITOR
		if (const UEnum* EnumPtr = StaticEnum<ECollisionChannel>())
		{
			return EnumPtr->HasMetaData(TEXT("Hidden"), EnumPtr->GetIndexByValue(InChannel));
		}
#endif
		return false;
	}

	static void ForEachChannel(const TFunction<void(const ECollisionChannel)>& Func, const bool bIgnoreHidden)
	{
		if (const UEnum* EnumPtr = StaticEnum<ECollisionChannel>())
		{
			for (int32 i = 0; i < EnumPtr->NumEnums(); i++)
			{
#if WITH_EDITOR
				if (bIgnoreHidden && EnumPtr->HasMetaData(TEXT("Hidden"), i)) continue;
#endif
				
				const TEnumAsByte<ECollisionChannel> EnumVal(i);
				if (!IsValidChannel(EnumVal)) continue;
				if (Func) Func(EnumVal);
			}
		}
	}

	static void ForEachChannel(const TFunction<void(const ECollisionChannel, const int32, const UEnum*)>& Func, const bool bIgnoreHidden)
	{
		if (const UEnum* EnumPtr = StaticEnum<ECollisionChannel>())
		{
			for (int32 i = 0; i < EnumPtr->NumEnums(); i++)
			{
#if WITH_EDITOR
				if (bIgnoreHidden && EnumPtr->HasMetaData(TEXT("Hidden"), i)) continue;
#endif
				
				const TEnumAsByte<ECollisionChannel> EnumVal(i);
				if (!IsValidChannel(EnumVal)) continue;
				if (Func) Func(EnumVal, i, EnumPtr);
			}
		}
	}

	static bool IsValidProfile(const FName& ProfileName)
	{
		FCollisionResponseTemplate Template;
		const UCollisionProfile* Profile = UCollisionProfile::Get();
		return Profile ? Profile->GetProfileTemplate(ProfileName, Template) : false;
	}
}
