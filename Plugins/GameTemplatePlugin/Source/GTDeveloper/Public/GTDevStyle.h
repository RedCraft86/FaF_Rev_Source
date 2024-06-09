// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Styling/SlateStyle.h"

class FGTDevStyle final
{
public:

	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();
	static const ISlateStyle& Get();
	static FName GetStyleSetName();

private:

	static TSharedRef<FSlateStyleSet> Create();
	static TSharedPtr<FSlateStyleSet> StyleInstance;
};
