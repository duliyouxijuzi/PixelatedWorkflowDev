// Copyright 2023 JunkGames. All Rights Reserved.

#include "AnimSpriteData.h"

#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"

float FAnimSpriteData::GetDuration()const
{
	return (float)NumFrames/FrameRate;
}

bool FAnimSpriteData::GetSocketTransform(FName SocketName, int FrameIndex, int RotationIndex, FTransform& OutTransform)const
{
	const int ActualRotation = Rotations.IsValidIndex(RotationIndex) ? RotationIndex : 0;

	const FAnimSpriteSocket* SocketData = Sockets.Find(SocketName);

	if (!SocketData)
	{
		return false;
	}

	const int CellIndex = FrameIndex + RotationIndex * NumFrames;

	if (!SocketData->Transforms.IsValidIndex(CellIndex))
	{
		return false;
	}

	OutTransform = SocketData->Transforms[CellIndex];
	return true;
}

bool FAnimSpriteData::GetRootMotion(int FrameIndex, int RotationIndex, FTransform& OutTransform)const
{
	if (!bHasRootMotion)
	{
		return false;
	}

	const int ActualRotation = Rotations.IsValidIndex(RotationIndex) ? RotationIndex : 0;

	const int CellIndex = FrameIndex + RotationIndex * NumFrames;
	if (!RootMotion.IsValidIndex(CellIndex))
	{
		return false;
	
	}
	OutTransform = RootMotion[CellIndex];
	return true;
}

void FAnimSpriteData::SetupMaterialInstanceDynamic(UMaterialInstanceDynamic* MID, float StartTime, int RotationIndex, float PlaySpeed, float StartPositionOffset, bool bLoop)
{
	const FAnimSpriteMaterialInfo MaterialInfo = GetMaterialInfo(StartTime, RotationIndex, PlaySpeed, StartPositionOffset, bLoop);

	if (IsValid(MID))
	{
		MID->SetTextureParameterValue(TEXT("BaseColor"), BaseColorTexture);
		MID->SetTextureParameterValue(TEXT("Normal"), NormalTexture);
		{
			TArray<FName> AdditionalTextureKeys;
			AdditionalTextures.GetKeys(AdditionalTextureKeys);
			for (const FName Key : AdditionalTextureKeys)
			{
				MID->SetTextureParameterValue(Key, AdditionalTextures[Key]);
			}
		}

		MID->SetScalarParameterValue(TEXT("FrameRate"), MaterialInfo.FrameRate);
		MID->SetScalarParameterValue(TEXT("NumColumns"), MaterialInfo.NumColumns);
		MID->SetScalarParameterValue(TEXT("NumFrames"), MaterialInfo.NumFrames);
		MID->SetScalarParameterValue(TEXT("PlaySpeed"), MaterialInfo.PlaySpeed);
		MID->SetScalarParameterValue(TEXT("PlayStartTime"), MaterialInfo.PlayStartTime);
		MID->SetScalarParameterValue(TEXT("StartPositionOffset"), MaterialInfo.StartPositionOffset);
		MID->SetVectorParameterValue(TEXT("UVRect"), FVector4(MaterialInfo.UVOffset, MaterialInfo.UVSize));
		MID->SetVectorParameterValue(TEXT("SpriteRect"), FVector4(MaterialInfo.SpriteSize, MaterialInfo.UVPivot));
		MID->SetScalarParameterValue(TEXT("DesiredRotationIndex"), MaterialInfo.DesiredRotationIndex);
		MID->SetScalarParameterValue(TEXT("MaxRotationIndex"), MaterialInfo.MaxRotationIndex);


		MID->SetScalarParameterValue(TEXT("DoLoop"), MaterialInfo.DoLoop ? 1.0f : 0.0f);
	}
}

bool FAnimSpriteData::CheckRotationsSymmetry()const
{
	const FVector Forward = FVector::XAxisVector;

	FVector Sum = FVector::ZeroVector;
	for (const FRotator R : Rotations)
	{
		FRotator RYaw(0.0f, R.Yaw, 0.0f);
		Sum += RYaw.RotateVector(Forward);
	}

	return Sum.Length() < (0.05f * Rotations.Num());
}

FAnimSpriteMaterialInfo FAnimSpriteData::GetMaterialInfo(float StartTime, int RotationIndex, float PlaySpeed, float StartPositionOffset, bool bLoop)const
{
	FAnimSpriteMaterialInfo Retval;

	Retval.UVOffset = FVector2D::ZeroVector;

	Retval.UVSize = CellUVSize;
	Retval.NumColumns = FramesX;
	Retval.NumFrames = NumFrames;

	Retval.StartPositionOffset = StartPositionOffset;
	Retval.PlayStartTime = StartTime;
	Retval.PlaySpeed = 1.0f;
	Retval.FrameRate = FrameRate;

	Retval.SpriteSize = SpriteSize;
	Retval.UVPivot = Pivot;

	Retval.DesiredRotationIndex = RotationIndex;
	Retval.MaxRotationIndex = FMath::Max(0, Rotations.Num() - 1);

	Retval.DoLoop = bLoop;

	return Retval;
}