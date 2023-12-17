// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimToSpriteSettings.h"

UAnimToSpriteSettings::UAnimToSpriteSettings()
{
	const FSoftObjectPath EmissivePath = FSoftObjectPath("/AnimToSprite/Editor/Materials/AdditionalCapture/PP_Emissive.PP_Emissive");
	const FSoftObjectPath SRMPath = FSoftObjectPath("/AnimToSprite/Editor/Materials/AdditionalCapture/PP_SpecularRoughnessMetallic.PP_SpecularRoughnessMetallic");
	const FSoftObjectPath OutlinePath = FSoftObjectPath("/AnimToSprite/Editor/Materials/AdditionalCapture/PP_SeparateOutline.PP_SeparateOutline");

	OptionalCapturesPreset.Add(EAnimToSpritePresetCaptureType::SRM, FAnimToSpritePresetCaptureInfo(TEXT("SRM"), 1, SRMPath));
	OptionalCapturesPreset.Add(EAnimToSpritePresetCaptureType::Emissive, FAnimToSpritePresetCaptureInfo(TEXT("Emissive"), 3, EmissivePath));
	OptionalCapturesPreset.Add(EAnimToSpritePresetCaptureType::SeparateOutline, FAnimToSpritePresetCaptureInfo(TEXT("SeparateOutline"), 2, OutlinePath));
}