// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SingleAnimationPlayData.h"
#include "GameFramework/Actor.h"
#include "PixelatedWorkflowStruct.generated.h"

/**
 * 
 */
/**************************enum-start***************************************/
// PixelatedName
UENUM(BlueprintType)
enum class EPixelatedName : uint8
{
	Cat UMETA(DisplayName = "Cat"),
	Dog UMETA(DisplayName = "Dog"),
	Rat UMETA(DisplayName = "Rat")
};

// AnimationName
UENUM(BlueprintType)
enum class EAnimationName : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Walk UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run")
};

// ModelType
UENUM(BlueprintType)
enum class EPixelatedModelType : uint8
{
	ActorClass,
	SkeletalMesh,
	StaticMesh
};

//ModelDirection
UENUM(BlueprintType)
enum class EPixelatedModelDirection : uint8
{
	YPositive UMETA(DisplayName="Y+"),
	YNegative UMETA(DisplayName = "Y-"),
	XPositive UMETA(DisplayName = "X+"),
	XNegative UMETA(DisplayName = "X-")
};

/**************************enum-end***************************************/





/**
 * 
 */
/**************************struct-start***************************************/
USTRUCT(BlueprintType)
struct PIXELATEDWORKFLOW_API FPixelatedModelSetting
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPixelatedModelType ModelType = EPixelatedModelType::SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPixelatedModelDirection ModelDirection = EPixelatedModelDirection::YPositive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "ModelType==EPixelatedModelType::ActorClass"))
	TSubclassOf<AActor> ActorClass=nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "ModelType==EPixelatedModelType::SkeletalMesh"))
	TObjectPtr<class USkeletalMesh> SkeletalMesh=nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "ModelType==EPixelatedModelType::StaticMesh"))
	TObjectPtr<class UStaticMesh> StaticMesh = nullptr;

public:
	bool IsValid()const
	{
		switch (ModelType)
		{
		case EPixelatedModelType::ActorClass:
			return ActorClass != nullptr;
		case EPixelatedModelType::SkeletalMesh:
			return SkeletalMesh != nullptr;
		case EPixelatedModelType::StaticMesh:
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

	bool operator==(const FPixelatedModelSetting& Other)const
	{
		if (ModelType != Other.ModelType)
		{
			return false;
		}

		switch (ModelType)
		{
		case EPixelatedModelType::ActorClass:
			return ActorClass == Other.ActorClass;
		case EPixelatedModelType::SkeletalMesh:
			return SkeletalMesh == Other.SkeletalMesh;
		case EPixelatedModelType::StaticMesh:
			return StaticMesh == Other.StaticMesh;
		default:
			return false;
		}
	}
};

// AnimationProperty
USTRUCT(BlueprintType)
struct PIXELATEDWORKFLOW_API FPixelatedAnimationProperty
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FPixelatedModelSetting Model;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSingleAnimationPlayData Animation;

};

/**************************settings***************************************/
//PixelatedBase
USTRUCT(BlueprintType)
struct PIXELATEDWORKFLOW_API FPixelatedBaseSettings
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Name"))
		EPixelatedName PixelatedName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="PixUnit", UIMin = "1", UIMax = "16"))
		int32 PixelatedUnit = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Size", UIMin = "1", UIMax = "1024"))
		FIntPoint PixelatedCellSize = FIntPoint(64, 64);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="PivotUV", UIMin = "0.0", UIMax = "1.0"))
		FVector2D PixelatedPivotUV = FVector2D(0.5, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="FrameRate", UIMin = "1", UIMax = "60"))
		int32 PixelatedFrameRate = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Rotation", UIMin = "0.0", UIMax = "360.0"))
		FRotator PixelatedRotations;

};

// Animation
USTRUCT(BlueprintType)
struct PIXELATEDWORKFLOW_API FPixelatedAnimationSettings
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Animation Name"))
		EAnimationName AnimationName;

};

// Path Settings
USTRUCT(BlueprintType)
struct PIXELATEDWORKFLOW_API FPixelatedPathSettings
{
	GENERATED_BODY()

private:
	bool isGenerateDirectory = true;
public:
	FPixelatedPathSettings();
	void GenerateDirectory(FString DirPath);
	UPROPERTY(EditAnywhere,meta=(DisplayName="Data Path", ContentDir))
		FDirectoryPath PixelatedDataPath;
	
	UPROPERTY(EditAnywhere,meta=(DisplayName="Generate Texture Path", ContentDir))
		FDirectoryPath PixelatedGenerateTexturePath;

	UPROPERTY(EditAnywhere,meta=(DisplayName="Output Png Path", RelativeToGameContentDir))
		FDirectoryPath PixelatedOutputPngPath;

};
/**************************struct-end***************************************/
