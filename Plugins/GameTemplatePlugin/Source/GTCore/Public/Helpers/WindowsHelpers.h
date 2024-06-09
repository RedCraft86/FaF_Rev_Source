// Copyright (C) RedCraft86. All Rights Reserved.

#pragma once
#pragma warning(disable : 4668)

#include "Data/MiscTypes.h"
#include "shlobj.h"

namespace WindowsHelpers
{
	static EWindowsDialogueResult OpenDialogue(const FString& Title, const FString& Message,
			const EWindowsDialogueType Type, const EWindowsDialogueIcon Icon)
	{
		return static_cast<EWindowsDialogueResult>(MessageBox(nullptr, *Message, *Title,
			static_cast<uint8>(Type) | static_cast<uint8>(Icon)));
	}
	
	static EMouseButtons::Type GetMouseButton(const FKey& InKey)
	{
		if (InKey.IsMouseButton())
		{
			if (InKey == EKeys::LeftMouseButton)	return EMouseButtons::Left;
			if (InKey == EKeys::RightMouseButton)	return EMouseButtons::Right;
			if (InKey == EKeys::MiddleMouseButton)	return EMouseButtons::Middle;
			if (InKey == EKeys::ThumbMouseButton)	return EMouseButtons::Thumb01;
			if (InKey == EKeys::ThumbMouseButton2)	return EMouseButtons::Thumb02;
		}
	
		return EMouseButtons::Invalid;
	}
	
	static void GetKeyCode(const FKey& InKey, int32& KeyCode, int32& CharCode)
	{
		const uint32* KeyCodePtr;
		const uint32* CharCodePtr;
		FInputKeyManager::Get().GetCodesFromKey(InKey, KeyCodePtr, CharCodePtr);
		KeyCode = KeyCodePtr ? static_cast<int32>(*KeyCodePtr) : 0;
		CharCode = CharCodePtr ? static_cast<int32>(*CharCodePtr) : 0;
	}
	
	static void VirtualKey(const FKey& InKey, const EVirtualKeyTriggerType TriggerType, const bool bRepeat)
	{
		if (!FSlateApplication::IsInitialized() || !InKey.IsValid()) return;
		const TSharedPtr<GenericApplication> PlatformApp = FSlateApplication::Get().GetPlatformApplication();
		const TSharedRef<FGenericApplicationMessageHandler> MsgHandler = PlatformApp->GetMessageHandler();
		if (InKey.IsMouseButton())
		{
			const EMouseButtons::Type MouseButton = GetMouseButton(InKey);
			switch(TriggerType)
			{
			case EVirtualKeyTriggerType::Click:
				MsgHandler->OnMouseDown(PlatformApp->GetWindowUnderCursor(), MouseButton);
				MsgHandler->OnMouseUp(MouseButton);
				break;

			case EVirtualKeyTriggerType::DoubleClick:
				MsgHandler->OnMouseDown(PlatformApp->GetWindowUnderCursor(), MouseButton);
				MsgHandler->OnMouseUp(MouseButton);
				MsgHandler->OnMouseDown(PlatformApp->GetWindowUnderCursor(), MouseButton);
				MsgHandler->OnMouseUp(MouseButton);
				break;

			case EVirtualKeyTriggerType::Press:
				MsgHandler->OnMouseDown(PlatformApp->GetWindowUnderCursor(), MouseButton);
				break;

			case EVirtualKeyTriggerType::Release:
				MsgHandler->OnMouseUp(MouseButton);
				break;
			}
		}
		else
		{
			int32 KeyCode, CharCode = 0;
			GetKeyCode(InKey, KeyCode, CharCode);
			switch(TriggerType)
			{
			case EVirtualKeyTriggerType::Click:
				MsgHandler->OnKeyDown(KeyCode, CharCode, false);
				MsgHandler->OnKeyChar(static_cast<TCHAR>(CharCode),false);
				MsgHandler->OnKeyUp(KeyCode, CharCode, false);
				break;

			case EVirtualKeyTriggerType::DoubleClick:
				MsgHandler->OnKeyDown(KeyCode, CharCode, false);
				MsgHandler->OnKeyChar(static_cast<TCHAR>(CharCode),false);
				MsgHandler->OnKeyUp(KeyCode, CharCode, false);
				MsgHandler->OnKeyDown(KeyCode, CharCode, false);
				MsgHandler->OnKeyChar(static_cast<TCHAR>(CharCode),false);
				MsgHandler->OnKeyUp(KeyCode, CharCode, false);

			case EVirtualKeyTriggerType::Press:
				MsgHandler->OnKeyDown(KeyCode, CharCode, bRepeat);
				MsgHandler->OnKeyChar(static_cast<TCHAR>(CharCode),bRepeat);
				break;

			case EVirtualKeyTriggerType::Release:
				MsgHandler->OnKeyUp(KeyCode, CharCode, false);
				break;
			}
		}
	}
}
