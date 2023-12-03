// Copyright Epic Games, Inc. All Rights Reserved.

#include "PixelatedWorkflowEditor.h"
#include "PixelatedWorkflowSettings.h"
#include "ISettingsModule.h"
#include "AssetToolsModule.h"
#include "AssetTypeCategories.h"


#define LOCTEXT_NAMESPACE "FPixelatedWorkflowEditorModule"

void FPixelatedWorkflowEditorModule::StartupModule()
{
	RegisterAssetActions();
	RegisterSettings();
}

void FPixelatedWorkflowEditorModule::ShutdownModule()
{
	UnregisterAssetActions();
	UnregisterSettings();
}

void FPixelatedWorkflowEditorModule::RegisterAssetActions()
{
	// 注册新的 Category
	EAssetTypeCategories::Type Category = FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(
		FName(TEXT("PixelatedWorkflow")), FText::FromString("PixelatedWorkflow"));
	// 注册 AssetTypeActions
	PixelateDataActions = MakeShared<FPixelateDataActions>(Category);
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(PixelateDataActions.ToSharedRef());
}

void FPixelatedWorkflowEditorModule::UnregisterAssetActions()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(PixelateDataActions.ToSharedRef());
	}
}

void FPixelatedWorkflowEditorModule::RegisterSettings()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "PixelatedWorkflow",
			LOCTEXT("ATPSettingsName", "PixelatedWorkflow"),
			LOCTEXT("ATPSettingsDescription", "PixelatedWorkflow settings"),
			GetMutableDefault<UPixelatedWorkflowSettings>()
		);
	}
}

void FPixelatedWorkflowEditorModule::UnregisterSettings()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "PixelatedWorkflow");
	}
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPixelatedWorkflowEditorModule, PixelatedWorkflowEditor)