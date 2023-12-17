// Copyright 2023 JunkGames. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SingleAnimationPlayData.h"
#include "AnimSpriteCapture.generated.h"

UENUM(BlueprintType)
enum class ECaptureModelType : uint8
{
	ActorClass,
	SkeletalMesh,
	StaticMesh
};

UENUM(BlueprintType)
enum class ECaptureModelDirection : uint8
{
	YPositive UMETA(DisplayName="Y+"),
	YNegative UMETA(DisplayName = "Y-"),
	XPositive UMETA(DisplayName = "X+"),
	XNegative UMETA(DisplayName = "X-")
};

USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FCaptureModelSetting
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture")
	ECaptureModelType ModelType = ECaptureModelType::SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture")
	ECaptureModelDirection ModelDirection = ECaptureModelDirection::YPositive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "ModelType==ECaptureModelType::ActorClass"))
	TSubclassOf<AActor> ActorClass=nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "ModelType==ECaptureModelType::SkeletalMesh"))
	TObjectPtr<class USkeletalMesh> SkeletalMesh=nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "ModelType==ECaptureModelType::StaticMesh"))
	TObjectPtr<class UStaticMesh> StaticMesh = nullptr;

public:
	bool IsValid()const
	{
		switch (ModelType)
		{
		case ECaptureModelType::ActorClass:
			return ActorClass != nullptr;
		case ECaptureModelType::SkeletalMesh:
			return SkeletalMesh != nullptr;
		case ECaptureModelType::StaticMesh:
			return StaticMesh != nullptr;
		default:
			return false;
		}
	}
	void Clear()
	{
		ActorClass = nullptr;
		SkeletalMesh = nullptr;
	}

	bool operator==(const FCaptureModelSetting& Other)const
	{
		if (ModelType != Other.ModelType)
		{
			return false;
		}

		switch (ModelType)
		{
		case ECaptureModelType::ActorClass:
			return ActorClass == Other.ActorClass;
		case ECaptureModelType::SkeletalMesh:
			return SkeletalMesh == Other.SkeletalMesh;
		case ECaptureModelType::StaticMesh:
			return StaticMesh == Other.StaticMesh;
		default:
			return false;
		}
	}
};

USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FAnimSpriteBorderSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpriteCapture")
	bool bDrawOutlines = false;

	UPROPERTY(EditANywhere, BlueprintReadWrite, Category = "SpriteCapture", meta=(EditCondition="bDrawOutlines"))
	FLinearColor OutlineColor = FLinearColor::Black;

	UPROPERTY(EditANywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition="bDrawOutlines"))
	float DepthGapThreshold = 20.0f;

	UPROPERTY(EditANywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bDrawOutlines"))
	float MaxDepth = 4000.0f;
};

UENUM(BlueprintType)
enum class ECellSizeSettingType : uint8
{
	BySize,
	ByResolution
};

USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FAnimSpriteCellSizeSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture")
	ECellSizeSettingType Type = ECellSizeSettingType::BySize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture")
	float PixelSize = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "Type==ECellSizeSettingType::BySize"))
	FVector2D CellSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "Type==ECellSizeSettingType::ByResolution"))
	int ResolutionX=64;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "Type==ECellSizeSettingType::ByResolution"))
	int ResolutionY=64;

public:
	FAnimSpriteCellSizeSettings()
		:CellSize(200.0f, 200.0f)
	{
	}
	FVector2D GetSize()const
	{
		switch (Type)
		{
		case ECellSizeSettingType::BySize:
			return CellSize;
		case ECellSizeSettingType::ByResolution:
			return FVector2D(PixelSize * ResolutionX, PixelSize * ResolutionY);
		default:
			break;
		}
		return FVector2D::One();
	}
	void GetResolution(int& X, int& Y)const
	{
		switch (Type)
		{
		case ECellSizeSettingType::BySize:
			X = FMath::CeilToInt(CellSize.X / PixelSize);
			Y = FMath::CeilToInt(CellSize.Y / PixelSize);
			break;
		case ECellSizeSettingType::ByResolution:
			X = ResolutionX;
			Y = ResolutionY;
			break;
		default:
			X = Y = 1;
			break;
		}
	}
};


UENUM(BlueprintType)
enum class EAnimToSpritePresetCaptureType : uint8
{
	None UMETA(DisplayName="(Choose Capture Type)"),
	SRM UMETA(DisplayName = "SRM (#1)"),				// Specular, Roughness, Metallic
	SeparateOutline UMETA(DisplayName = "SeparateOutline (#2)"),	// Separate Outline
	Emissive UMETA(DisplayName = "Emissive (#3)")			// Emissive
};

USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FOptionalPresetCaptureSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture")
	TSet<EAnimToSpritePresetCaptureType> Presets;
};

USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FAdditionalCaptureSettings
{
	GENERATED_BODY()

public:
	FAdditionalCaptureSettings()
		:DrawMaterial(nullptr)
		, Paper2DAdditionalTextureIndex(1)
	{
	}

	FAdditionalCaptureSettings(class UMaterialInterface* Material, int Paper2DIndex)
		:DrawMaterial(Material)
		, Paper2DAdditionalTextureIndex(Paper2DIndex)
	{
	}

public:
	// PostProcess material for drawing texture
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture")
	TObjectPtr<class UMaterialInterface> DrawMaterial;

	// index for paper 2d sprite additional texture. #0 is reserved for normalmap
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture")
	int Paper2DAdditionalTextureIndex=1;
};

USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FAnimSpriteLayoutOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpriteCapture")
	bool bPreferSquareLayout = false;
};

USTRUCT(BlueprintType)
struct ANIMTOSPRITE_API FAnimSpriteCaptureSettings
{
	GENERATED_BODY()

public:
	FAnimSpriteCaptureSettings()
		:PivotUV(0.5f, 0.95f)
	{
		Rotations.SetNum(1);
		Rotations[0] = FRotator::ZeroRotator;
	}

	FAnimSpriteCaptureSettings(bool bAllOverride)
		:bOverride_CellSizeSettings(bAllOverride)
		, bOverride_PivotUV(bAllOverride)
		, PivotUV(0.5f, 0.95f)
		, bOverride_FrameRate(bAllOverride)
		, bOverride_SocketNames(bAllOverride)
		, bOverride_FlattenSocketLocations(bAllOverride)
		, bOverride_Model(bAllOverride)
		, bOverride_Rotations(bAllOverride)
		, bOverride_BorderSettings(bAllOverride)
		, bOverride_PresetCaptureSettings(bAllOverride)
		, bOverride_AdditionalCaptureSettings(bAllOverride)
		, bOverride_LayoutOptions(bAllOverride)
	{
		Rotations.SetNum(1);
		Rotations[0] = FRotator::ZeroRotator;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_CellSizeSettings = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bOverride_CellSizeSettings"))
	FAnimSpriteCellSizeSettings CellSizeSettings;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_PivotUV = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bOverride_PivotUV"))
	FVector2D PivotUV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_FrameRate = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bOverride_FrameRate"))
	float FrameRate = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_SocketNames = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bOverride_SocketNames"))
	TArray<FName> SocketNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_FlattenSocketLocations = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bOverride_FlattenSocketLocations"))
	bool FlattenSocketLocations = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_Model = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bOverride_Model"))
	FCaptureModelSetting Model;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture")
	FSingleAnimationPlayData Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bUseFixedAnimationLength = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bUseFixedAnimationLength"))
	float FixedAnimationLength=1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_Rotations = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bOverride_Rotations"))
	TArray<FRotator> Rotations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_BorderSettings = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bOverride_BorderSettings"))
	FAnimSpriteBorderSettings BorderSettings;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_PresetCaptureSettings = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bOverride_PresetCaptureSettings", DisplayName = "Optional Captures"))
	FOptionalPresetCaptureSettings PresetCaptureSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_AdditionalCaptureSettings = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bOverride_AdditionalCaptureSettings", DisplayName = "Custom Captures"))
	TMap<FString, FAdditionalCaptureSettings> AdditionalCaptureSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (PinHiddenByDefault, InlineEditConditionToggle))
	bool bOverride_LayoutOptions = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpriteCapture", meta = (EditCondition = "bOverride_LayoutOptions"))
	FAnimSpriteLayoutOptions LayoutOptions;

public:
	FVector2D GetCellSize()const
	{
		return CellSizeSettings.GetSize();
	}
	void GetCellResolution(int& X, int& Y)const
	{
		CellSizeSettings.GetResolution(X, Y);
	}
	int GetNumRotations()const
	{
		return FMath::Max(1, Rotations.Num());
	}
	FRotator GetRotation(int Index)const
	{
		return Rotations.IsValidIndex(Index) ? Rotations[Index] : FRotator::ZeroRotator;
	}
	float GetAnimationPlayLength()const;

	void CalcLayoutFromNumCells(int NumCells, int& Columns, int& TexWidth, int& TexHeight)const;

	void GetAdditionalCaptures(TMap<FString, FAdditionalCaptureSettings>& OutAdditionalCaptures)const;
};

