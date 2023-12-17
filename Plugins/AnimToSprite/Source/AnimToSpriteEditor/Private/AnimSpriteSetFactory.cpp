// Copyright 2023 JunkGames. All Rights Reserved.


#include "AnimSpriteSetFactory.h"
#include "AnimSpriteSet.h"

UAnimSpriteSetFactory::UAnimSpriteSetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UAnimSpriteSet::StaticClass();
}

bool UAnimSpriteSetFactory::DoesSupportClass(UClass* Class)
{
	return Class == UAnimSpriteSet::StaticClass();
}

UClass* UAnimSpriteSetFactory::ResolveSupportedClass()
{
	return UAnimSpriteSet::StaticClass();
}

UObject* UAnimSpriteSetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UAnimSpriteSet>(InParent, InClass, InName, Flags);
}