// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PixelateDataActions.h"

class FPixelatedWorkflowEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RegisterAssetActions();
	void UnregisterAssetActions();

	void RegisterSettings();
	void UnregisterSettings();
private:

	TSharedPtr<FPixelateDataActions> PixelateDataActions;
};
