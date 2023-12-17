// Copyright 2023 JunkGames. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimSpriteCapture.h"

#include "AnimSpriteData.generated.h"

USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FAnimSpriteSocket
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sprite")
	TArray<FTransform> Transforms;
};


USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FAnimSpriteMaterialInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	FVector2D UVOffset;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	FVector2D UVSize;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	int NumColumns;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	int NumFrames;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	FVector2D SpriteSize;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	FVector2D UVPivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	float StartPositionOffset = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	float PlayStartTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	float PlaySpeed = 1.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	float FrameRate = 8.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	int MaxRotationIndex = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	int DesiredRotationIndex = 0;
//	int CellIndexOffset = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	bool DoLoop = true;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FAnimSpriteData
{
	GENERATED_BODY()
public:

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category=SpriteCapture)
	FAnimSpriteCaptureSettings CaptureSettings;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Sprite)
	TObjectPtr<class UTexture2D> BaseColorTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	TObjectPtr<class UTexture2D> NormalTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	TMap<FName, class UTexture2D*> AdditionalTextures;

	// original size (when captured)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	FVector2D SpriteSize;
	// pivot in UV space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	FVector2D Pivot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	FVector2D CellUVSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	int FramesX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	int NumFrames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	float FrameRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	TMap<FName, FAnimSpriteSocket> Sockets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	TArray<FRotator> Rotations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	bool bRotationsSymmetry = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	bool bHasRootMotion = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	TArray<FTransform> RootMotion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sprite)
	ECaptureModelDirection ModelDirection = ECaptureModelDirection::YPositive;
	//FVector ModelForwardVector = FVector::YAxisVector;

public:
	FAnimSpriteData()
		:SpriteSize(200.0f, 200.0f)
		,Pivot(0.5f, 0.9f)
		,CellUVSize(0.1f, 0.1f)
		,FramesX(8)
		,NumFrames(1)
		,FrameRate(8.0f)
	{
		Rotations.SetNum(1);
		Rotations[0] = FRotator::ZeroRotator;
	}

public:
	float GetDuration()const;

	int GetNumRotations()const
	{
		return FMath::Max(Rotations.Num(), 1);
	}

	bool GetSocketTransform(FName SocketName, int FrameIndex, int RotationIndex, FTransform &OutTransform)const;
	bool GetRootMotion(int FrameIndex, int RotationIndex, FTransform& OutTransform)const;

	void SetupMaterialInstanceDynamic(class UMaterialInstanceDynamic *MID, float StartTime, int RotationIndex, float PlaySpeed, float StartPositionOffset, bool bLoop);
		
	bool CheckRotationsSymmetry()const;

private:
	FAnimSpriteMaterialInfo GetMaterialInfo(float StartTime, int RotationIndex, float PlaySpeed, float StartPositionOffset, bool bLoop)const;
};


