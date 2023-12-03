// Fill out your copyright notice in the Description page of Project Settings.


#include "PixelateData.h"

UPixelateData::UPixelateData()
	: Mean(0.5f)
	, StandardDeviation(0.2f)
{}

float UPixelateData::DrawSample()
{
	return std::normal_distribution<>(Mean, StandardDeviation)(RandomNumberGenerator);
}

void UPixelateData::LogSample()
{
	UE_LOG(LogTemp, Log, TEXT("%f"), DrawSample())
}

FString UPixelateData::GetPixelateDataFileName()
{
	
	FString NewObjectName = TEXT("PD_") + UEnum::GetDisplayValueAsText(PixelatedBaseSettings.PixelatedName).ToString() + TEXT("_") +
		UEnum::GetDisplayValueAsText(PixelatedAnimationSettings.AnimationName).ToString();
	return NewObjectName;
}
