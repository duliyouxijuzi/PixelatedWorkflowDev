// Copyright 2023 JunkGames. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimToSpriteBPLibrary.generated.h"


UCLASS()
class UAnimToSpriteBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, CallInEditor, Category = AnimSprite)
	static void ResizeRenderTarget(class UTextureRenderTarget2D* RenderTarget, int SizeX, int SizeY);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = AnimSprite)
	static void RoundUpToPowerOfTwo(int X, int Y, int& NewX, int& NewY);

	UFUNCTION(BlueprintPure, CallInEditor, Category = AnimSprite, DisplayName="GetCellSize")
	static FVector2D GetCaptureSettingsCellSize(const struct FAnimSpriteCaptureSettings& CaptureSettings);

	UFUNCTION(BlueprintPure, CallInEditor, Category = AnimSprite, DisplayName = "GetResolution")
	static void GetCaptureSettingsResolution(const struct FAnimSpriteCaptureSettings& CaptureSettings, int &ResolutionX, int &ResolutionY);

	UFUNCTION(BlueprintPure, CallInEditor, Category = AnimSprite, DisplayName = "CalcTextureSize")
	static void CalcTextureSizeFromCaptureSettings(const struct FAnimSpriteCaptureSettings& CaptureSettings, int NumCells, int& Columns, int& TexWidth, int& TexHeight);

	UFUNCTION(BlueprintPure, CallInEditor, Category = AnimSprite, DisplayName = "GetNumRotations")
	static int GetCaptureSettingsNumRotations(const struct FAnimSpriteCaptureSettings& CaptureSettings);

	UFUNCTION(BlueprintPure, CallInEditor, Category = AnimSprite, DisplayName = "GetRotation")
	static FRotator GetCaptureSettingsRotation(const struct FAnimSpriteCaptureSettings& CaptureSettings, int Index);

	UFUNCTION(BlueprintPure, CallInEditor, Category = AnimSprite, DisplayName = "GetPlayLength")
	static float GetAnimationPlayLength(const struct FAnimSpriteCaptureSettings& CaptureSettings);

	UFUNCTION(BlueprintPure, CallInEditor, Category = AnimSprite, DisplayName = "GetAdditionalCaptures")
	static void GetCaptureSettingsAdditionalCaptures(const struct FAnimSpriteCaptureSettings& CaptureSettings, TMap<FString, FAdditionalCaptureSettings>& OutAdditionalCaptures);


	UFUNCTION(BlueprintCallable, CallInEditor, Category = AnimSprite, DisplayName = "SetupSpriteMaterial")
	static void SetupSpriteMaterial(class UMaterialInstanceDynamic* MID, UPARAM(ref) struct FAnimSpriteData& SpriteData, float StartTime, int RotationIndex, float PlaySpeed, float StartPositionOffset, bool bLoop);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = AnimSprite)
	static FTransform ExtractRootMotion(class USkeletalMeshComponent* SkeletalMeshComponent);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = AnimSprite)
	static void SetupSkeletalMeshComponentForCapture(class USkeletalMeshComponent* SkeletalMeshComponent);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = AnimSprite)
	static void TickSkeletalMeshComponent(class USkeletalMeshComponent* SkeletalMeshComponent, float DeltaTime);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = AnimSprite)
	static class USkeletalMeshComponent *FindLeaderSkeletalMeshComponent(class AActor *Target);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = AnimSprite)
	static void TickNiagaraComponent(class UNiagaraComponent* NiagaraComponent, float DeltaTime, int Iterations);
};
