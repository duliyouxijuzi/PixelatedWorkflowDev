// Copyright 2023 JunkGames. All Rights Reserved.

#include "AnimToSpriteEditorBPLibrary.h"

#include "AnimSpriteSetFactory.h"

UAnimSpriteSetFactory* UAnimToSpriteEditorBPLibrary::CreateSpriteSetFactory()
{
	return NewObject<UAnimSpriteSetFactory>();
}