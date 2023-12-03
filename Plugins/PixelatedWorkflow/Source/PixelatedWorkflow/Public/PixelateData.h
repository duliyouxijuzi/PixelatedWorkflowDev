// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <random>
#include "PixelatedWorkflowStruct.h"
#include "UObject/NoExportTypes.h"
#include "PixelateData.generated.h"


/**
 * 
 */
UCLASS(BlueprintType)
class PIXELATEDWORKFLOW_API UPixelateData : public UObject
{
	GENERATED_BODY()
public:
	UPixelateData();

	UFUNCTION(BlueprintCallable)
	float DrawSample();

	UFUNCTION(CallInEditor)
	void LogSample();

	FString GetPixelateDataFileName();

public:
	UPROPERTY(EditAnywhere)
	float Mean;

	UPROPERTY(EditAnywhere)
	float StandardDeviation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Settings", Meta  = (DisplayName="Base"))
		FPixelatedBaseSettings PixelatedBaseSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Settings", Meta  = (DisplayName="Animation"))
		FPixelatedAnimationSettings PixelatedAnimationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Property", Meta  = (DisplayName="Details"))
		FPixelatedAnimationProperty PixelatedAnimationProperty;

private:
	std::mt19937 RandomNumberGenerator;
};
