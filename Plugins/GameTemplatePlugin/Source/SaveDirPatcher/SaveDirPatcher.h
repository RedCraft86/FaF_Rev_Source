// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

#define CUSTOM_PATH "RedCraft86_Games"

class FSaveDirPatcherModule final : public IModuleInterface
{
#if UE_BUILD_SHIPPING	
	virtual void StartupModule() override
	{
		FString Arg;
		FParse::Value(FCommandLine::Get(), TEXT("UserDir="), Arg);
		if (Arg.IsEmpty())
		{
			FString Patches = TEXT("0");
			FFileHelper::LoadFileToString(Patches, *(FPaths::ProjectUserDir() / TEXT("Saved/Logs/Patches")));
			const uint8 NumPatches = FCString::Atoi(*Patches);
			if (NumPatches < 5)
			{
				FString GameDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
				GameDir.RemoveFromEnd(TEXT("/")); GameDir += TEXT(".exe");
				if (!FPaths::FileExists(GameDir))
				{
					GameDir = FWindowsPlatformProcess::ExecutablePath();
				}
				FPaths::NormalizeDirectoryName(GameDir);

				Arg = TEXT("-UserDir=") + FPaths::Combine(FPlatformProcess::UserSettingsDir(), TEXT(CUSTOM_PATH), FApp::GetProjectName());
				FPlatformProcess::CreateProc(*GameDir, *Arg, true, false, false, nullptr, 0, nullptr, nullptr);

				FFileHelper::SaveStringToFile(FString::FromInt(NumPatches + 1), *(FPaths::ProjectUserDir() / TEXT("Saved/Logs/Patches")));
				FPlatformMisc::RequestExit(false);
				return;
			}
		}
		
		const FString BasePath = FPaths::Combine(FPlatformProcess::UserSettingsDir(), FApp::GetProjectName());
		IFileManager& Manager = IFileManager::Get();

		Manager.DeleteDirectory(*(BasePath / TEXT("Saved/Config/CrashReportClient/")), true, true);
		Manager.DeleteDirectory(*(BasePath / TEXT("Saved/Logs/")), true, true);

		TArray<FString> InnerFiles;
		Manager.FindFilesRecursive(InnerFiles, *BasePath, nullptr, true, false);
		if (InnerFiles.IsEmpty())
		{
			Manager.DeleteDirectory(*BasePath, true, true);
		}
	}
#endif
};