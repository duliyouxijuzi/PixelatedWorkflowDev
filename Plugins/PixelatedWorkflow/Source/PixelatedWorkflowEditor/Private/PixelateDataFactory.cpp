// Fill out your copyright notice in the Description page of Project Settings.


#include "PixelateDataFactory.h"
#include "PixelateData.h"

UPixelateDataFactory::UPixelateDataFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	SupportedClass = UPixelateData::StaticClass();
}


UObject* UPixelateDataFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UPixelateData>(InParent, InClass, InName, Flags, Context);
}

FString UPixelateDataFactory::GetDefaultNewAssetName() const
{
	return TEXT("PixelateData");
}
