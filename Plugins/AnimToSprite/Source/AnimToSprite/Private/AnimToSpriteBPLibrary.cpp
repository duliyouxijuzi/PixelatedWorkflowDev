// Copyright 2023 JunkGames. All Rights Reserved.

#include "AnimToSpriteBPLibrary.h"
#include "AnimToSprite.h"
#include "AnimSpriteSet.h"
#include "AnimSpriteCapture.h"
#include "AnimSpriteData.h"

#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"
#include "Animation/AnimInstance.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/SkeletalMesh.h"
#include "NiagaraComponent.h"

#if WITH_EDITOR
#include "Factories/DataAssetFactory.h"
#endif

UAnimToSpriteBPLibrary::UAnimToSpriteBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
/*
void UAnimToSpriteBPLibrary::ForceInitAnimation(class USkeletalMeshComponent* SkeletalMeshComponent)
{
	SkeletalMeshComponent->InitAnim(true);
}
*/

void UAnimToSpriteBPLibrary::ResizeRenderTarget(class UTextureRenderTarget2D* RenderTarget, int SizeX, int SizeY)
{
	RenderTarget->ResizeTarget(SizeX, SizeY);
}

void UAnimToSpriteBPLibrary::RoundUpToPowerOfTwo(int X, int Y, int& NewX, int& NewY)
{
	NewX = FMath::RoundUpToPowerOfTwo(X);
	NewY = FMath::RoundUpToPowerOfTwo(Y);
}


FVector2D UAnimToSpriteBPLibrary::GetCaptureSettingsCellSize(const FAnimSpriteCaptureSettings& CaptureSettings)
{
	return CaptureSettings.GetCellSize();
}
void UAnimToSpriteBPLibrary::GetCaptureSettingsResolution(const struct FAnimSpriteCaptureSettings& CaptureSettings, int& ResolutionX, int& ResolutionY)
{
	CaptureSettings.GetCellResolution(ResolutionX, ResolutionY);
}
/*
int UAnimToSpriteBPLibrary::GetCaptureSettingsColumns(const struct FAnimSpriteCaptureSettings& CaptureSettings)
{
	return CaptureSettings.GetColumns();
}
*/
void UAnimToSpriteBPLibrary::CalcTextureSizeFromCaptureSettings(const struct FAnimSpriteCaptureSettings& CaptureSettings, int NumCells, int& Columns, int& TexWidth, int& TexHeight)
{
	CaptureSettings.CalcLayoutFromNumCells(NumCells, Columns, TexWidth, TexHeight);
}

int UAnimToSpriteBPLibrary::GetCaptureSettingsNumRotations(const struct FAnimSpriteCaptureSettings& CaptureSettings)
{
	return CaptureSettings.GetNumRotations();
}
FRotator UAnimToSpriteBPLibrary::GetCaptureSettingsRotation(const struct FAnimSpriteCaptureSettings& CaptureSettings, int Index)
{
	return CaptureSettings.GetRotation(Index);
}

float UAnimToSpriteBPLibrary::GetAnimationPlayLength(const struct FAnimSpriteCaptureSettings& CaptureSettings)
{
	return CaptureSettings.GetAnimationPlayLength();
}

void UAnimToSpriteBPLibrary::GetCaptureSettingsAdditionalCaptures(const struct FAnimSpriteCaptureSettings& CaptureSettings, TMap<FString, FAdditionalCaptureSettings>& OutAdditionalCaptures)
{
	CaptureSettings.GetAdditionalCaptures(OutAdditionalCaptures);
}

void UAnimToSpriteBPLibrary::SetupSpriteMaterial(class UMaterialInstanceDynamic* MID, struct FAnimSpriteData& SpriteData, float StartTime, int RotationIndex, float PlaySpeed, float StartPositionOffset, bool bLoop)
{
	SpriteData.SetupMaterialInstanceDynamic(MID, StartTime, RotationIndex, PlaySpeed, StartPositionOffset, bLoop);
}


FTransform UAnimToSpriteBPLibrary::ExtractRootMotion(class USkeletalMeshComponent* SkeletalMeshComponent)
{
	FRootMotionMovementParams RootMotionParam = SkeletalMeshComponent->ConsumeRootMotion();
	return RootMotionParam.GetRootMotionTransform();
}


void UAnimToSpriteBPLibrary::SetupSkeletalMeshComponentForCapture(class USkeletalMeshComponent* SkeletalMeshComponent)
{
	if (!IsValid(SkeletalMeshComponent))
	{
		return;
	}
#if ENGINE_MINOR_VERSION >= 1
	if (!IsValid(SkeletalMeshComponent->GetSkeletalMeshAsset()))
#else
	if (!IsValid(SkeletalMeshComponent->SkeletalMesh))
#endif
	{
		return;
	}
	SkeletalMeshComponent->InitAnim(true);

	SkeletalMeshComponent->EnableExternalUpdate(true);
	SkeletalMeshComponent->EnableExternalTickRateControl(true);
	SkeletalMeshComponent->SetExternalInterpolationAlpha(1.0f);
//	SkeletalMeshComponent->SetExternalTickRate(1.0f);
	if (IsValid(SkeletalMeshComponent->GetAnimInstance()))
	{
		SkeletalMeshComponent->GetAnimInstance()->RootMotionMode = ERootMotionMode::RootMotionFromEverything;
	}
}

void UAnimToSpriteBPLibrary::TickSkeletalMeshComponent(class USkeletalMeshComponent* SkeletalMeshComponent, float DeltaTime)
{
//	SkeletalMeshComponent->TickPose(DeltaTime, true);
	if (!IsValid(SkeletalMeshComponent))
	{
		return;
	}
#if ENGINE_MINOR_VERSION >= 1
	if (!IsValid(SkeletalMeshComponent->GetSkeletalMeshAsset()))
#else
	if (!IsValid(SkeletalMeshComponent->SkeletalMesh))
#endif
	{
		return;
	}
	SkeletalMeshComponent->TickAnimation(DeltaTime, true);
	SkeletalMeshComponent->RefreshBoneTransforms();
}

class USkeletalMeshComponent* UAnimToSpriteBPLibrary::FindLeaderSkeletalMeshComponent(class AActor* Target)
{
	USkeletalMeshComponent* Leader = nullptr;
	USkeletalMeshComponent* Topmost = nullptr;

	TArray<USkeletalMeshComponent*> SMCs;
	Target->GetComponents<USkeletalMeshComponent>(SMCs);

	int MinDepth = 1000;
	TArray<USceneComponent*> Parents;
	for (USkeletalMeshComponent *SMC : SMCs)
	{
		Leader = Cast<USkeletalMeshComponent>(SMC->LeaderPoseComponent);
		if (IsValid(Leader))
		{
			return Leader;
		}

		SMC->GetParentComponents(Parents);
		int Depth = Parents.Num();
		if (Depth < MinDepth)
		{
			MinDepth = Depth;
			Topmost = SMC;
		}
	}

	return Topmost;
}


void UAnimToSpriteBPLibrary::TickNiagaraComponent(class UNiagaraComponent* NiagaraComponent, float DeltaTime, int Iterations)
{
	if (!NiagaraComponent->GetForceSolo())
	{
		UE_LOG(LogTemp, Warning, TEXT("NiagaraComponent needs to be ForceSolo mode"));
		return;
	}

	for (int i = 0; i < Iterations; ++i)
	{
		NiagaraComponent->TickComponent(DeltaTime, ELevelTick::LEVELTICK_All, nullptr);
	}
}
