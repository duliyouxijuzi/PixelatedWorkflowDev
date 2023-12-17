// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PixelatedWorkflowStruct.h"
#include "UObject/NoExportTypes.h"
#include "SingleAnimationPlayData.h"
#include "PixelatedWorkflowStruct.h"
#include "PixelatedAsset.generated.h"

class UPixelatedWorkflowSettings;
/**
 * 
 */
UCLASS(BlueprintType)
class PIXELATEDWORKFLOW_API UPixelatedAsset : public UObject
{
	GENERATED_BODY()
private:
	TArray<FString> CustomPixelatedNames;
	TArray<FString> CustomAnimationNames;
	const UPixelatedWorkflowSettings* PixelatedWorkflowSettings = GetDefault<UPixelatedWorkflowSettings>();
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (DisplayName = "PixelatedName", GetOptions="GetPixelatedNameOptions"))
		FString PixelatedName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (DisplayName = "AnimationName", GetOptions="GetAnimationNameOptions"))
		FString AnimationName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (DisplayName = "PixUnit", UIMin = "1", UIMax = "16"))
		int32 PixelatedUnit = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (DisplayName = "Size", UIMin = "1", UIMax = "1024"))
		FIntPoint PixelatedCellSize = FIntPoint(64, 64);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (DisplayName = "PivotUV", UIMin = "0.0", UIMax = "1.0"))
		FVector2D PixelatedPivotUV = FVector2D(0.5, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (DisplayName = "FrameRate", UIMin = "1", UIMax = "60"))
		int32 PixelatedFrameRate = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting", meta = (DisplayName = "Rotation", UIMin = "0.0", UIMax = "360.0"))
		FRotator PixelatedRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model", meta = (DisplayName = "Model Setting"))
		FPixelatedModelSetting ModelSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (DisplayName = "Animation Setting"))
		FSingleAnimationPlayData AnimationPlayData;

public:
	FString GetPixelatedAssetFileName();

	UFUNCTION()
		TArray<FString> GetPixelatedNameOptions() const {return CustomPixelatedNames;}
	UFUNCTION()
		TArray<FString> GetAnimationNameOptions() const {return CustomAnimationNames;}

	UFUNCTION(CallInEditor)
		void ResetPixelatedAsset();

	float GetAnimationDuration();
	float GetPerFrameDuration() const;
	float GetPreFrameDuration(int32 Index) const;
	float GetCurFrameDuration(int32 Index) const;

	int32 GetCellTotalNums();

	FVector2D GetPixelatedCellSize() { return  PixelatedCellSize; }

	void CalculateCellLayout(int32& RowNums, int32& ColumnNums, int32& TexWidth, int32& TexHeight);
};
