// Copyright 2023 JunkGames. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimToSpriteEditorBPLibrary.generated.h"


UCLASS()
class UAnimToSpriteEditorBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, CallInEditor, Category = AnimSprite)
	static class UAnimSpriteSetFactory* CreateSpriteSetFactory();
};
