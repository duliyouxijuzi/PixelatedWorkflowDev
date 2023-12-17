// Copyright 2023 JunkGames. All Rights Reserved.

#include "AnimSpriteSet.h"
#include "AnimSpriteData.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/TextureRenderTarget2D.h"

#include "PaperSprite.h"
#include "PaperFlipbook.h"

UAnimSpriteSet::UAnimSpriteSet(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
#if WITH_EDITORONLY_DATA
	,DefaultCaptureSettings(true)
#endif
{
}

void UAnimSpriteSet::AddAnimation
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
	const TMap<FName, FAnimSpriteSocket>& Sockets,
	const TArray<FTransform> &RootMotion,

	const FAnimSpriteCaptureSettings& CaptureSettings
)
{
	FAnimSpriteData* AnimationData = Animations.Find(AnimationName);
	if (!AnimationData)
	{
		AnimationData = &Animations.Add(AnimationName);
	}

	if (!AnimationData)
	{
		// failed ?
		UE_LOG(LogTemp, Warning, TEXT("Failed to add Animation"));
		return;
	}

	AnimationData->BaseColorTexture = BaseColorTexture;
	AnimationData->NormalTexture = NormalTexture;
	AnimationData->AdditionalTextures = AdditionalTextures;
	AnimationData->SpriteSize = SpriteSize;
	AnimationData->Pivot = Pivot;
	AnimationData->CellUVSize = CellUVSize;
	AnimationData->FramesX = FramesX;
	AnimationData->NumFrames = NumFrames;
	AnimationData->FrameRate = FrameRate;
	AnimationData->Sockets = Sockets;

	if (RootMotion.IsEmpty())
	{
		AnimationData->bHasRootMotion = false;
		AnimationData->RootMotion.Empty();
	}
	else
	{
		AnimationData->bHasRootMotion = true;
		AnimationData->RootMotion = RootMotion;
	}

#if WITH_EDITORONLY_DATA
	AnimationData->CaptureSettings = CaptureSettings;

	{
		FAnimSpriteCaptureSettings MergedCaptureSettings;
		MergeCaptureSettings(CaptureSettings, MergedCaptureSettings);
		AnimationData->Rotations = MergedCaptureSettings.Rotations;
//		AnimationData->ModelForwardVector = MergedCaptureSettings.Model.ForwardVector;
		AnimationData->ModelDirection = MergedCaptureSettings.Model.ModelDirection;

		AnimationData->bRotationsSymmetry = AnimationData->CheckRotationsSymmetry();
	}
#endif

	if (Animations.Num() == 1)
	{
		DefaultAnimation = AnimationName;
	}

	MarkPackageDirty();
}

void UAnimSpriteSet::AddAnimationPlain(FName AnimationName)
{
	FAnimSpriteData* AnimationData = Animations.Find(AnimationName);
	if (!AnimationData)
	{
		AnimationData = &Animations.Add(AnimationName);
	}

	if (Animations.Num() == 1)
	{
		DefaultAnimation = AnimationName;
	}

	MarkPackageDirty();
}

UTexture2D* UAnimSpriteSet::FindOrCreateTextureFromRenderTarget(
	class UTextureRenderTarget2D* RenderTarget,
	FName TextureName,
	bool bNormalMap)
{
#if WITH_EDITOR
	if (!IsValid(RenderTarget))
	{
		return nullptr;
	}

	const FString AssetPath = UKismetSystemLibrary::GetPathName(this);
	const FString LongPackagePath = FPackageName::GetLongPackagePath(AssetPath);

	return FindOrCreateTextureFromRenderTargetInternal(RenderTarget,
		LongPackagePath,
		TextureName,
		bNormalMap);
#else
	return nullptr;
#endif
}

UTexture2D* UAnimSpriteSet::FindOrCreateTextureFromRenderTarget_Paper2D(
	class UTextureRenderTarget2D* RenderTarget,
	FName TextureName,
	bool bNormalMap)
{
#if WITH_EDITOR
	if (!IsValid(RenderTarget))
	{
		return nullptr;
	}
		
	return FindOrCreateTextureFromRenderTargetInternal(RenderTarget,
		Paper2DSpriteDirectory(),
		TextureName,
		bNormalMap);
#else
	return nullptr;
#endif
}

UTexture2D* UAnimSpriteSet::FindOrCreateTextureFromRenderTargetInternal(
	class UTextureRenderTarget2D* RenderTarget,
	const FString &PackageDirectory,
	FName TextureName,
	bool bNormalMap)
{
#if WITH_EDITOR
	if (!IsValid(RenderTarget))
	{
		return nullptr;
	}

	const FString LongPackagePath = PackageDirectory;

	const FString PackagePath = LongPackagePath + TEXT("/") + TextureName.ToString();
	const FString TexturePath = PackagePath + TEXT(".") + TextureName.ToString();

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
#if ENGINE_MINOR_VERSION >= 1
	const FSoftObjectPath SoftTexturePath(TexturePath);
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(SoftTexturePath);
#else
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(*TexturePath);
#endif

	UTexture2D* Texture = nullptr;
	if (AssetData.IsValid())
	{
		// texture found
		Texture = Cast<UTexture2D>(AssetData.GetAsset());

		const ETextureSourceFormat TextureFormat = RenderTarget->GetTextureFormatForConversionToTexture2D();
		RenderTarget->UpdateTexture2D(Texture, TextureFormat);
	}
	else
	{
		// not found -> Create
		Texture = RenderTarget->ConstructTexture2D(CreatePackage(*PackagePath), TextureName.ToString(), RenderTarget->GetMaskedFlags() | RF_Public | RF_Standalone, CTF_Default, nullptr);
	}

	if (!IsValid(Texture))
	{
		return nullptr;
	}

	Texture->CompressionSettings = bNormalMap ? TC_Normalmap : TC_Default;
	Texture->Filter = TextureFilter::TF_Nearest;

	Texture->PostEditChange();
	Texture->UpdateResource();

	Texture->Modify();
	Texture->MarkPackageDirty();

	return Texture;
#else
	return nullptr;
#endif
}

bool UAnimSpriteSet::GetOverrideCaptureSettings(FName AnimationName, FAnimSpriteCaptureSettings& CaptureSettings)const
{
#if WITH_EDITORONLY_DATA
	const FAnimSpriteData* FoundAnimation = Animations.Find(AnimationName);
	if (FoundAnimation)
	{
		CaptureSettings = FoundAnimation->CaptureSettings;
		return true;
	}
	else
	{
		return false;
	}
#else
	return false;
#endif
}

bool UAnimSpriteSet::GetMergedCaptureSettings(FName AnimationName, FAnimSpriteCaptureSettings& CaptureSettings)const
{
#if WITH_EDITORONLY_DATA
	const FAnimSpriteData* FoundAnimation = Animations.Find(AnimationName);
	if (FoundAnimation)
	{
		MergeCaptureSettings( FoundAnimation->CaptureSettings, CaptureSettings);
		return true;
	}
	else
	{
		return false;
	}
#else
	return false;
#endif
}


bool UAnimSpriteSet::GetSpriteData(FName AnimationName, FAnimSpriteData& SpriteData)const
{
	const FAnimSpriteData* FoundAnimation = Animations.Find(AnimationName);
	if (FoundAnimation)
	{
		SpriteData = *FoundAnimation;
		return true;
	}
	else
	{
		return false;
	}
}

#define DO_OVERRIDE(NAME) if(Override.bOverride_ ## NAME) Result. NAME = Override. NAME

void UAnimSpriteSet::MergeCaptureSettings(const FAnimSpriteCaptureSettings& Override, FAnimSpriteCaptureSettings& Result)const
{
#if WITH_EDITOR
	const FAnimSpriteCaptureSettings& Base = DefaultCaptureSettings;

	Result = Base;

	DO_OVERRIDE(CellSizeSettings);

	DO_OVERRIDE(PivotUV);

	DO_OVERRIDE(FrameRate);

	DO_OVERRIDE(SocketNames);
	DO_OVERRIDE(FlattenSocketLocations);

	DO_OVERRIDE(Model);

	DO_OVERRIDE(Rotations);

	DO_OVERRIDE(BorderSettings);

	DO_OVERRIDE(PresetCaptureSettings);
	DO_OVERRIDE(AdditionalCaptureSettings);

	DO_OVERRIDE(LayoutOptions);

	Result.Animation = Override.Animation;
	Result.bUseFixedAnimationLength = Override.bUseFixedAnimationLength;
	Result.FixedAnimationLength = Override.FixedAnimationLength;

#endif
}
#undef DO_OVERRIDE

bool UAnimSpriteSet::SetDefaultAnimationName(FName NewDefaultAnimation)
{
	if (Animations.Contains(NewDefaultAnimation))
	{
		DefaultAnimation = NewDefaultAnimation;
		MarkPackageDirty();
		return true;
	}

	return false;
}

bool UAnimSpriteSet::RemoveAnimation(FName AnimationName)
{
	if (Animations.Contains(AnimationName))
	{
		Animations.Remove(AnimationName);

		if ( DefaultAnimation == AnimationName)
		{
			if (Animations.Num() > 0)
			{
				TArray<FName> Keys;
				Animations.GetKeys(Keys);
				DefaultAnimation = Keys[0];
			}
			else
			{
				DefaultAnimation = NAME_None;
			}
		}
		MarkPackageDirty();
		return true;
	}

	return false;
}

FString UAnimSpriteSet::Paper2DSpriteDirectory()const
{
#if WITH_EDITOR
	const FString AssetPath = UKismetSystemLibrary::GetPathName(this);
	const FString LongPackagePath = FPackageName::GetLongPackagePath(AssetPath);

	return LongPackagePath + "/" + Paper2DSettings.Paper2DSpriteOutputDirectory;
#else
	return FString(TEXT("/"));
#endif
}


bool UAnimSpriteSet::InitializePaper2DSprite(UPaperSprite* PaperSprite, FName AnimationName, int RotationIndex, int FrameIndex)
{
#if WITH_EDITOR
	if (!IsValid(PaperSprite))
	{
		return false;
	}

	FAnimSpriteData* AnimData = Animations.Find(AnimationName);
	if (!AnimData)
	{
		return false;
	}

	FAnimSpriteCaptureSettings CaptureSettings;
	GetMergedCaptureSettings(AnimationName, CaptureSettings);

	FSpriteAssetInitParameters SpriteParameters;
	SpriteParameters.Texture = AnimData->BaseColorTexture;
	int Width, Height;
	CaptureSettings.GetCellResolution(Width, Height);

	const int CellIndex = AnimData->NumFrames * RotationIndex + FrameIndex;
	const int Column = CellIndex % AnimData->FramesX;
	const int Row = CellIndex / AnimData->FramesX;

	SpriteParameters.Dimension = FIntPoint(Width, Height);
	SpriteParameters.Offset = FIntPoint(Width, Height) * FIntPoint(Column, Row);
	SpriteParameters.SetPixelsPerUnrealUnit(1.0f / CaptureSettings.CellSizeSettings.PixelSize);
	//	SpriteParameters.DefaultMaterialOverride = Paper2DSettings.Material;
	//	SpriteParameters.AlternateMaterialOverride = Paper2DSettings.Material;
	SpriteParameters.AdditionalTextures.Add(AnimData->NormalTexture);
	{
		TArray<FName> AdditionalTextureKeys;
		AnimData->AdditionalTextures.GetKeys(AdditionalTextureKeys);

		TMap<FString, FAdditionalCaptureSettings> AdditionalCaptureSettings;
		CaptureSettings.GetAdditionalCaptures(AdditionalCaptureSettings);

		for (const FName Key : AdditionalTextureKeys)
		{
			const FAdditionalCaptureSettings *Setting = AdditionalCaptureSettings.Find(Key.ToString());
			if (Setting)
			{
				SpriteParameters.AdditionalTextures.SetNum( FMath::Max(SpriteParameters.AdditionalTextures.Num(), Setting->Paper2DAdditionalTextureIndex + 1));
				SpriteParameters.AdditionalTextures[Setting->Paper2DAdditionalTextureIndex] = AnimData->AdditionalTextures[Key];
			}
		}
	}

	PaperSprite->InitializeSprite(SpriteParameters, false);

	FVector2D Pivot(Width * Column + AnimData->Pivot.X * Width, Height * Row + AnimData->Pivot.Y * Height);
	PaperSprite->SetPivotMode(ESpritePivotMode::Custom, Pivot);

	TArray<FName> SocketKeys;
	AnimData->Sockets.GetKeys(SocketKeys);

	FArrayProperty* SocketProperty = FindFProperty<FArrayProperty>(UPaperSprite::StaticClass(), TEXT("Sockets"));// CastField<FArrayProperty>(UPaperSprite::StaticClass()->FindPropertyByName(TEXT("Sockets")));
	TArray<FPaperSpriteSocket>* SocketsPtr = SocketProperty->ContainerPtrToValuePtr<TArray<FPaperSpriteSocket>>(PaperSprite);
	TArray<FPaperSpriteSocket> Sockets;
	for (FName SocketName : SocketKeys)
	{
		const FAnimSpriteSocket& Socket = AnimData->Sockets[SocketName];
		if (Socket.Transforms.IsValidIndex(CellIndex))
		{
			FTransform TmpTransform = Socket.Transforms[CellIndex];
			FPaperSpriteSocket S;
			S.SocketName = SocketName;
			FVector Location = TmpTransform.GetLocation() * SpriteParameters.PixelsPerUnrealUnit;
			TmpTransform.SetLocation(Location);
			FTransform Rot;
			Rot.SetFromMatrix(FRotationMatrix(FRotator::MakeFromEuler(FVector(0.0f, 0.0f, -90.0f))));
			TmpTransform = TmpTransform * Rot;
			S.LocalTransform = TmpTransform;
			Sockets.Add(S);
		}
	}
	SocketsPtr->Empty();
	SocketsPtr->Append(Sockets);

	PaperSprite->Modify();

	return true;
#else
	return false;
#endif
}





bool UAnimSpriteSet::UpdatePaper2DFlipbook(class UPaperFlipbook* Flipbook, const TArray<class UPaperSprite*>& Sprites, float FramesPerSecond)
{
#if WITH_EDITOR
	FScopedFlipbookMutator EditLock(Flipbook);

	EditLock.FramesPerSecond = FramesPerSecond;
	EditLock.KeyFrames.SetNum(Sprites.Num());
	for (int i = 0; i < Sprites.Num(); ++i)
	{
		EditLock.KeyFrames[i].Sprite = Sprites[i];
		EditLock.KeyFrames[i].FrameRun = 1;
	}
	Flipbook->Modify();

	return true;
#else
	return false;
#endif
}