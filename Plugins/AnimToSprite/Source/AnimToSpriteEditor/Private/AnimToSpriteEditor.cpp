// Copyright 2023 JunkGames. All Rights Reserved.

#include "AnimToSpriteEditor.h"

#include "ISettingsModule.h"
#include "AnimToSpriteSettings.h"

#define LOCTEXT_NAMESPACE "FAnimToSpriteEditorModule"

void FAnimToSpriteEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "AnimToSprite",
			LOCTEXT("A2SSettingsName", "AnimToSprite"),
			LOCTEXT("A2SSettingsDescription", "AnimToSprite settings"),
			GetMutableDefault<UAnimToSpriteSettings>()
		);
	}
}

void FAnimToSpriteEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "AnimToSprite");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAnimToSpriteEditorModule, AnimToSpriteEditor)