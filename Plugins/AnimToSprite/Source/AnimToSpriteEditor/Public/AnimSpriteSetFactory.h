// Copyright 2023 JunkGames. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AnimSpriteSetFactory.generated.h"

/**
 * 
 */
UCLASS()
class ANIMTOSPRITEEDITOR_API UAnimSpriteSetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	virtual bool DoesSupportClass(UClass* Class) override;
	virtual UClass* ResolveSupportedClass() override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)override;
};
