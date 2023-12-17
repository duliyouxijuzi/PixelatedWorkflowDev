// Copyright 2023 JunkGames. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FAnimToSpriteModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
