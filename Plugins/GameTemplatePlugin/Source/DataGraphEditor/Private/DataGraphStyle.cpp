// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#include "DataGraphStyle.h"
#include "Styling/SlateStyleMacros.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FDataGraphStyle::StyleInstance = nullptr;

void FDataGraphStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FDataGraphStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FDataGraphStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FDataGraphStyle::Get()
{
	return *StyleInstance;
}

FName FDataGraphStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("DataGraphStyle"));
	return StyleSetName;
}

const FVector2D Icon64x64(64.0f, 64.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef<FSlateStyleSet> FDataGraphStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("GameTemplatePlugin"))->GetBaseDir() / TEXT("Resources/Icons"));

	Style->Set("ClassThumbnail.GenericDataGraph", new IMAGE_BRUSH_SVG(TEXT("GenericDataGraph"), Icon64x64));
	Style->Set("DataGraph.AutoArrangeNodes", new IMAGE_BRUSH_SVG(TEXT("GenericDataArrange"), Icon20x20));
	return Style;
}
