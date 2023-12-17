// Fill out your copyright notice in the Description page of Project Settings.


#include "PixelatedAssetFactory.h"
#include "PixelatedAsset.h"

UPixelatedAssetFactory::UPixelatedAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	SupportedClass = UPixelatedAsset::StaticClass();
}


UObject* UPixelatedAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UPixelatedAsset* PixelatedAsset = NewObject<UPixelatedAsset>(InParent, InClass, InName, Flags, Context);
	PixelatedAsset->ResetPixelatedAsset();
	return PixelatedAsset;
}

FString UPixelatedAssetFactory::GetDefaultNewAssetName() const
{
	return TEXT("PixelatedAsset");
}
