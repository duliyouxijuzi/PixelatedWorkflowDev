// Fill out your copyright notice in the Description page of Project Settings.


#include "PixelatedAsset.h"
#include "PixelatedWorkflowSettings.h"

void UPixelatedAsset::ResetPixelatedAsset()
{
	CustomPixelatedNames = PixelatedWorkflowSettings->CustomPixelatedNames;
	CustomAnimationNames = PixelatedWorkflowSettings->CustomAnimationNames;
	PixelatedUnit = PixelatedWorkflowSettings->PixelatedUnit;
	PixelatedCellSize = PixelatedWorkflowSettings->PixelatedCellSize;
	PixelatedPivotUV = PixelatedWorkflowSettings->PixelatedPivotUV;
	PixelatedFrameRate = PixelatedWorkflowSettings->PixelatedFrameRate;
	PixelatedRotation = PixelatedWorkflowSettings->PixelatedRotation;
}

float UPixelatedAsset::GetAnimationDuration()
{
	float PlayLength = AnimationPlayData.AnimToPlay->GetPlayLength() / FMath::Max(AnimationPlayData.SavedPlayRate, 0.01);
	return PlayLength;
}

float UPixelatedAsset::GetPerFrameDuration() const
{
	 
	return AnimationPlayData.SavedPlayRate / PixelatedFrameRate;
}

float UPixelatedAsset::GetPreFrameDuration(int32 Index) const
{
	int32 PreIndex = FMath::Max(Index - 1, 0);
	float FrameDuration = PreIndex / PixelatedFrameRate;
	return FrameDuration * AnimationPlayData.SavedPlayRate + AnimationPlayData.SavedPosition;
}

float UPixelatedAsset::GetCurFrameDuration(int32 Index) const
{
	float FrameDuration = Index / PixelatedFrameRate;
	return FrameDuration * AnimationPlayData.SavedPlayRate + AnimationPlayData.SavedPosition;
}

int32 UPixelatedAsset::GetCellTotalNums()
{
	float PlayLength = GetAnimationDuration();
	return FMath::Max(FMath::RoundToInt(PlayLength * PixelatedFrameRate), 1);
}

void UPixelatedAsset::CalculateCellLayout(int32& RowNums, int32& ColumnNums, int32& TexWidth, int32& TexHeight)
{
	int32 TotalNums = GetCellTotalNums();
	int MaxWidth = 4096;
	do
	{
		ColumnNums = FMath::Min(TotalNums, FMath::CeilToInt(MaxWidth / (float) PixelatedCellSize.X));
		RowNums = FMath::CeilToInt( TotalNums / (float)ColumnNums);

		TexWidth = FMath::RoundUpToPowerOfTwo( ColumnNums * PixelatedCellSize.X);
		TexHeight = FMath::RoundUpToPowerOfTwo(RowNums * PixelatedCellSize.Y);

		UE_LOG(LogTemp, Log, TEXT("CalculateCellLayout: MaxWidth = %d	ColumnNums = %d		RowNums = %d	TexWidth = %d	TexHeight = %d."), 
			MaxWidth, ColumnNums, RowNums, TexWidth, TexHeight);
		MaxWidth /= 2;
	} while (TexWidth >= TexHeight * 4);
}


FString UPixelatedAsset::GetPixelatedAssetFileName()
{
	FString PixelatedName_ = PixelatedName.IsEmpty() ? TEXT("None") : PixelatedName;
	FString AnimationName_ = AnimationName.IsEmpty() ? TEXT("None") : AnimationName;
	return TEXT("PA_")+ PixelatedName_ + TEXT("_") + AnimationName_;
}
