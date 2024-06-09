// Copyright (C) 2016 jinyuliao.
// Edited by RedCraft86.

#pragma once

class FDataGraphAssetCommands final : public TCommands<FDataGraphAssetCommands>
{
public:

	FDataGraphAssetCommands()
		: TCommands("DataGraphEditor",
			NSLOCTEXT("Contexts", "DataGraphEditor", "Data Graph Editor"),
			NAME_None, FAppStyle::GetAppStyleSetName())
	{}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> AutoArrange;
};
