// Copyright 2023 JunkGames. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "AnimSpriteData.h"
#include "AnimSpriteCapture.h"
#include "AnimSpriteSet.generated.h"

USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FAnimSpriteToPaper2DInfo
{
	GENERATED_BODY()
public:
	FAnimSpriteToPaper2DInfo()
		:Paper2DSpriteOutputDirectory(TEXT("Paper2D"))
	{
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprite")
	bool bOutputPaper2DAssets = false;

	UPROPERTY(EditAnywhere, Category="Sprite")
	FString Paper2DSpriteOutputDirectory;
};

/**
 * 
 */
UCLASS(BlueprintType)
class ANIMTOSPRITE_API UAnimSpriteSet : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprite")
	FName DefaultAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprite")
	TMap<FName, FAnimSpriteData> Animations;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category="SpriteCapture")
	FAnimSpriteCaptureSettings DefaultCaptureSettings;

	UPROPERTY(EditAnywhere, Category = "SpriteCapture")
	FAnimSpriteToPaper2DInfo Paper2DSettings;
#endif

public:
	UAnimSpriteSet(const class FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Sprite")
	void AddAnimation
	(
		FName AnimationName,
		class UTexture2D* BaseColorTexture,
		class UTexture2D* NormalTexture,
		const TMap<FName, class UTexture2D*> &AdditionalTextures,
		FVector2D SpriteSize,
		FVector2D Pivot,
		FVector2D CellUVSize,
		int FramesX,
		int NumFrames,
		float FrameRate,
		const TMap<FName, FAnimSpriteSocket> &Sockets,
		const TArray<FTransform> &RootMotion,

		const FAnimSpriteCaptureSettings &CaptureSettings
	);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Sprite")
	void AddAnimationPlain(FName AnimationName);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Sprite")
	class UTexture2D* FindOrCreateTextureFromRenderTarget(
		class UTextureRenderTarget2D* RenderTarget,
		FName TextureName,
		bool bNormalMap);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Sprite")
	class UTexture2D* FindOrCreateTextureFromRenderTarget_Paper2D(
		class UTextureRenderTarget2D* RenderTarget,
		FName TextureName,
		bool bNormalMap);

private:
	class UTexture2D* FindOrCreateTextureFromRenderTargetInternal(
		class UTextureRenderTarget2D* RenderTarget,
		const FString &PackageDirectory,
		FName TextureName,
		bool bNormalMap);

public:
	UFUNCTION(BlueprintPure, CallInEditor, Category = "Sprite")
	bool GetOverrideCaptureSettings(FName AnimationName, FAnimSpriteCaptureSettings& CaptureSettings)const;

	UFUNCTION(BlueprintPure, CallInEditor, Category = "Sprite")
	bool GetMergedCaptureSettings(FName AnimationName, FAnimSpriteCaptureSettings& CaptureSettings)const;

	UFUNCTION(BlueprintPure, CallInEditor, Category = "Sprite")
	bool GetSpriteData(FName AnimationName, FAnimSpriteData& SpriteData)const;

	UFUNCTION(BlueprintPure, CallInEditor, Category = "Sprite")
	void MergeCaptureSettings(const FAnimSpriteCaptureSettings& Override, FAnimSpriteCaptureSettings& Result)const;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Sprite")
	bool SetDefaultAnimationName(FName NewDefaultAnimation);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Sprite")
	bool RemoveAnimation(FName AnimationName);


	UFUNCTION(BlueprintPure, CallInEditor, Category="Sprite")
	bool RequiresPaper2DAssets()const {
#if WITH_EDITOR
		return Paper2DSettings.bOutputPaper2DAssets;
#else
		return false;
#endif
	}

	UFUNCTION(BlueprintPure, CallInEditor, Category = "Sprite")
	FString Paper2DSpriteDirectory()const;
		
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Sprite")
	bool InitializePaper2DSprite(class UPaperSprite *PaperSprite, FName AnimationName, int RotationIndex, int FrameIndex);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Sprite")
	bool UpdatePaper2DFlipbook(class UPaperFlipbook* Flipbook, const TArray<class UPaperSprite*>& Sprites, float FramesPerSecond);
};
