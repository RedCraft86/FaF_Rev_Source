// Copyright (C) RedCraft86. All Rights Reserved.

#include "GTDevStyle.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FGTDevStyle::StyleInstance = nullptr;

void FGTDevStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FGTDevStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FGTDevStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FGTDevStyle::Get()
{
	return *StyleInstance;
}

FName FGTDevStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("GTDevStyle"));
	return StyleSetName;
}

const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef<FSlateStyleSet> FGTDevStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("GameTemplatePlugin"))->GetBaseDir() / TEXT("Resources/Icons"));

	Style->Set("GTDevTools.RestartEditor", new IMAGE_BRUSH_SVG(TEXT("RestartEditor"), Icon20x20));
	Style->Set("GTDevTools.ChannelPacker", new IMAGE_BRUSH_SVG(TEXT("ChannelPacker"), Icon20x20));
	Style->Set("GTDevTools.TodoTaskList", new IMAGE_BRUSH_SVG(TEXT("TodoTaskList"), Icon20x20));
	Style->Set("GTDevTools.StaticMeshBaker", new IMAGE_BRUSH_SVG(TEXT("StaticMeshBaker"), Icon20x20));
	Style->Set("GTDevTools.StaticMeshMerger", new IMAGE_BRUSH_SVG(TEXT("StaticMeshMerger"), Icon20x20));
	Style->Set("GTDevTools.StaticMeshInstancer", new IMAGE_BRUSH_SVG(TEXT("StaticMeshInstancer"), Icon20x20));
	return Style;
}
