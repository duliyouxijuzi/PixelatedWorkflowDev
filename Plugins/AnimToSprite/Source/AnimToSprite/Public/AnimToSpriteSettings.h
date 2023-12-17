// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AnimSpriteCapture.h"
#include "AnimToSpriteSettings.generated.h"


USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FAnimToSpritePresetCaptureInfo
{
	GENERATED_BODY()

public:
	FAnimToSpritePresetCaptureInfo()
	{
	}

	FAnimToSpritePresetCaptureInfo(const FString ParamName,
		int Paper2DTextureIndex,
		const FSoftObjectPath &MaterialPath)
		:ParameterName(ParamName)
		,Paper2DAdditionalTextureIndex(Paper2DTextureIndex)
		,DrawMaterial(MaterialPath)
	{
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture")
	FString ParameterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture")
	int Paper2DAdditionalTextureIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (AllowedClasses = "/Script/Engine.MaterialInterface"))
	FSoftObjectPath DrawMaterial;
};
/**
 * 
 */
UCLASS(config=Engine, defaultconfig)
class ANIMTOSPRITE_API UAnimToSpriteSettings : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "SpriteCapture")
	TMap<EAnimToSpritePresetCaptureType, FAnimToSpritePresetCaptureInfo> OptionalCapturesPreset;

public:
	UAnimToSpriteSettings();
};
