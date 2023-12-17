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
	/// <summary>
	/// Setting
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Setting", meta = (DisplayName = "PixUnit", UIMin = "1", UIMax = "16"))
		int32 PixelatedUnit = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Setting", meta = (DisplayName = "Size", UIMin = "1", UIMax = "1024"))
		FIntPoint PixelatedCellSize = FIntPoint(64, 64);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Setting", meta = (DisplayName = "PivotUV", UIMin = "0.0", UIMax = "1.0"))
		FVector2D PixelatedPivotUV = FVector2D(0.5, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Setting", meta = (DisplayName = "FrameRate", UIMin = "1", UIMax = "60"))
		int32 PixelatedFrameRate = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Setting", meta = (DisplayName = "CustomPixelatedName"))
		TArray<FString> CustomPixelatedNames = {"None"};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Setting", meta = (DisplayName = "CustomAnimationName"))
		TArray<FString> CustomAnimationNames = { "None" };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Setting", meta = (DisplayName = "Rotation", UIMin = "0.0", UIMax = "360.0"))
		FRotator PixelatedRotation;

	/// <summary>
	/// Path
	/// </summary>
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Path", meta = (PinHiddenByDefault, InlineEditConditionToggle))
		bool bSetPixelatedAssetPath = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Path", meta = (DisplayName = "AssetPath", EditCondition = "bSetPixelatedAssetPath", RelativeToGameContentDir))
		FDirectoryPath PixelatedAssetPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Path", meta = (PinHiddenByDefault, InlineEditConditionToggle))
		bool bSetGenerateTexturePath = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Path", meta = (DisplayName = "GenerateTexturePath", EditCondition = "bSetGenerateTexturePath", RelativeToGameContentDir))
		FDirectoryPath PixelatedGenerateTexturePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Path", meta = (PinHiddenByDefault, InlineEditConditionToggle))
		bool bSetOutputPngPath = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Path", meta = (DisplayName = "OutputPngPath", EditCondition = "bSetOutputPngPath", RelativeToGameContentDir))
		FDirectoryPath PixelatedOutputPngPath;
};
