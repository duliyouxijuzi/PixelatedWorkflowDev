// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PixelatedWorkflowStruct.h"
#include "PixelatedWorkflowSettings.generated.h"


/**
 * 
 */
UCLASS(config = Game, defaultconfig)
class PIXELATEDWORKFLOW_API UPixelatedWorkflowSettings : public UObject
{
	GENERATED_BODY()
public:
	UPixelatedWorkflowSettings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Global Settings", Meta  = (DisplayName="Base"))
		FPixelatedBaseSettings PixelatedBaseSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Global Settings", Meta  = (DisplayName="Animation"))
		FPixelatedAnimationSettings PixelatedAnimationSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Global Settings", Meta  = (DisplayName="Path"))
		FPixelatedPathSettings PixelatedPathSettings;

};
