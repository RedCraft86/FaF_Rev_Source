// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

namespace DataGraphColors
{
	namespace NodeBody
	{
		static constexpr FLinearColor Default(0.1f, 0.1f, 0.1f);
		static constexpr FLinearColor Root(0.5f, 0.5f, 0.5f, 0.1f);
		static constexpr FLinearColor Error(1.0f, 0.0f, 0.0f);
	}

	namespace NodeBorder
	{
		static constexpr FLinearColor Inactive(0.08f, 0.08f, 0.08f);
		static constexpr FLinearColor Root(0.2f, 0.2f, 0.2f, 0.2f);
		static constexpr FLinearColor Selected(1.00f, 0.08f, 0.08f);
		static constexpr FLinearColor ActiveDebugging(1.0f, 1.0f, 0.0f);
		static constexpr FLinearColor InactiveDebugging(0.4f, 0.4f, 0.0f);
		static constexpr FLinearColor HighlightAbortRange0(0.0f, 0.22f, 0.4f);
		static constexpr FLinearColor HighlightAbortRange1(0.0f, 0.4f, 0.22f);
		static constexpr FLinearColor Disconnected(0.f, 0.f, 0.f);
		static constexpr FLinearColor BrokenWithParent(1.f, 0.f, 1.f);
		static constexpr FLinearColor QuickFind(0.f, 0.8f, 0.f);
	}

	namespace Pin
	{
		static constexpr FLinearColor Diff(0.9f, 0.2f, 0.15f);
		static constexpr FLinearColor Hover(1.0f, 0.7f, 0.0f);
		static constexpr FLinearColor Default(0.02f, 0.02f, 0.02f);
		static constexpr FLinearColor SingleNode(0.02f, 0.02f, 0.02f);
	}
	
	namespace Connection
	{
		static constexpr FLinearColor Default(1.0f, 1.0f, 1.0f);
	}

	namespace Action
	{
		static constexpr FLinearColor DragMarker(1.0f, 1.0f, 0.2f);
	}
}
