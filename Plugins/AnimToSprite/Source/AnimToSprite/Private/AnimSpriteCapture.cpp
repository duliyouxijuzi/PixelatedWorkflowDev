// Copyright 2023 JunkGames. All Rights Reserved.

#include "AnimSpriteCapture.h"
#include "Animation/AnimationAsset.h"
#include "AnimToSpriteBPLibrary.h"
#include "AnimToSpriteSettings.h"

float FAnimSpriteCaptureSettings::GetAnimationPlayLength()const
{
	if (bUseFixedAnimationLength)
	{
		return FixedAnimationLength;
	}
	if (IsValid(Animation.AnimToPlay))
	{
		const float PlayRate = FMath::Max(Animation.SavedPlayRate, 0.1f);
		return Animation.AnimToPlay->GetPlayLength()/PlayRate;
	}

	return 0.0f;
}

void FAnimSpriteCaptureSettings::CalcLayoutFromNumCells(int NumCells, int& Columns, int& TexWidth, int& TexHeight)const
{
	int CellResX, CellResY;
	GetCellResolution(CellResX, CellResY);
	static const int MAX_TEXWIDTH = 4096;

	if (LayoutOptions.bPreferSquareLayout)
	{
		int MaxWidth = MAX_TEXWIDTH;

		do
		{
			Columns = FMath::Min(NumCells, MaxWidth / CellResX);
			const int Rows = ((NumCells % Columns) == 0) ? NumCells / Columns : (NumCells / Columns + 1);

			const int RequiredWidth = Columns * CellResX;
			const int RequiredHeight = Rows * CellResY;

			UAnimToSpriteBPLibrary::RoundUpToPowerOfTwo(RequiredWidth, RequiredHeight, TexWidth, TexHeight);

			MaxWidth /= 2;
		} while (TexWidth >= TexHeight * 4);
	}
	else
	{
		Columns = FMath::Min(NumCells, MAX_TEXWIDTH / CellResX);
		const int Rows = ((NumCells % Columns) == 0) ? NumCells / Columns : (NumCells / Columns + 1);

		const int RequiredWidth = Columns * CellResX;
		const int RequiredHeight = Rows * CellResY;

		UAnimToSpriteBPLibrary::RoundUpToPowerOfTwo(RequiredWidth, RequiredHeight, TexWidth, TexHeight);
	}
}

void FAnimSpriteCaptureSettings::GetAdditionalCaptures(TMap<FString, FAdditionalCaptureSettings>& OutAdditionalCaptures)const
{
	OutAdditionalCaptures = AdditionalCaptureSettings;
	
	const TMap<EAnimToSpritePresetCaptureType, FAnimToSpritePresetCaptureInfo> &PresetSettings = GetDefault<UAnimToSpriteSettings>()->OptionalCapturesPreset;

	for (EAnimToSpritePresetCaptureType CaptureType : PresetCaptureSettings.Presets)
	{
		const FAnimToSpritePresetCaptureInfo* CaptureInfo = PresetSettings.Find(CaptureType);
		if (CaptureInfo)
		{
			const FString& ParameterName = CaptureInfo->ParameterName;
			
			FAdditionalCaptureSettings NewSettings;
			NewSettings.DrawMaterial = Cast<UMaterialInterface>(CaptureInfo->DrawMaterial.TryLoad());
			NewSettings.Paper2DAdditionalTextureIndex = CaptureInfo->Paper2DAdditionalTextureIndex;
			OutAdditionalCaptures.Add(ParameterName, NewSettings);
		}
	}
}



