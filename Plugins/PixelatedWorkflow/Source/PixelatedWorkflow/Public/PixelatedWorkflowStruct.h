// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SingleAnimationPlayData.h"
#include "GameFramework/Actor.h"
#include "PixelatedWorkflowStruct.generated.h"

/**
 * 
 */

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

/**
 * 
 */
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
	bool IsValid()const;
	void Clear();
	bool operator==(const FPixelatedModelSetting& Other)const;
};

/**************************struct-end***************************************/
