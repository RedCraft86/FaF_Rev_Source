// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTEditorStyle.h"
#include "Styling/SlateStyleMacros.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FGTEditorStyle::StyleInstance = nullptr;

void FGTEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FGTEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FGTEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FGTEditorStyle::Get()
{
	return *StyleInstance;
}

FName FGTEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("GTEditorStyle"));
	return StyleSetName;
}

const FVector2D Icon64x64(64.0f, 64.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef<FSlateStyleSet> FGTEditorStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("GameTemplatePlugin"))->GetBaseDir() / TEXT("Resources/Icons"));

	Style->Set("SmallIcon.InventoryGrid", new IMAGE_BRUSH_SVG(TEXT("InventoryGridSmall"), Icon20x20));
	Style->Set("ClassThumbnail.InventoryItemDataBase", new IMAGE_BRUSH_SVG(TEXT("InventoryGrid"), Icon64x64));
	Style->Set("ClassThumbnail.MusicDataBase", new IMAGE_BRUSH_SVG(TEXT("MusicData"), Icon64x64));
	return Style;
}
