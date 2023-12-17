// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PixelatedCaptureActor.generated.h"

class UPixelatedAsset;

UCLASS()
class PIXELATEDWORKFLOW_API APixelatedCaptureActor : public AActor
{
	GENERATED_BODY()

private:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* CaptureRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class USceneCaptureComponent2D* NormalCapture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class USceneCaptureComponent2D* BaseColorCapture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class UTextRenderComponent* ResolutionText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* CellsRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class UDirectionalLightComponent* DirectionalLight;

private:
	// 行数
	int32 RowNum;
	// 列数
	int32 ColumnNum;
	// 生成的Tex宽度
	int32 TexWidth;
	// 生成的Tex高度
	int32 TexHeight;

	TArray<TSharedPtr<class UBoxComponent*>> CellBoxList;
	TArray<TSharedPtr<class USkeletalMeshComponent*>> PoseSKCMList;
	TArray<TSharedPtr<AActor*>> PostActorList;

public:	
	// Sets default values for this actor's properties
	APixelatedCaptureActor();

	// 像素捕捉过程
	void PixelatedCapture(UPixelatedAsset* PixelatedAsset);
	// 重置像素捕捉
	void ResetCapture();
	// 计算Cell行列
	void CalculateCellRowAndColumn(UPixelatedAsset* PixelatedAsset);
	void CreateAllCellBox(UPixelatedAsset* PixelatedAsset);
	void CreateCellBox(const int32 RowIndex, const int32 ColumnIndex, const FVector2D CellSize);

	void CreateAllPoseObject(UPixelatedAsset* PixelatedAsset);
	void PoseUpdateComponents(UPixelatedAsset* PixelatedAsset, const int32 Index, TArray<TWeakObjectPtr<class USkeletalMeshComponent>> SKCMs);

	void CreateAllPoseObjectByActorClass(UPixelatedAsset* PixelatedAsset);
	void CreatePoseObjectByActorClass(TSubclassOf<AActor> ActorClass, const FVector Location, 
		const FRotator Rotation, const FVector Scale, const FActorSpawnParameters SpawnParams);
	
	void CreateAllPoseObjectBySkeletalMesh(UPixelatedAsset* PixelatedAsset);
	class USkeletalMeshComponent* CreatePoseObjectBySkeletalMesh(const FString Name, TObjectPtr<class USkeletalMesh> SkeletalMesh,
		const FVector Location, const FRotator Rotation, const FVector Scale);
	
	void CreateAllPoseObjectByStaticMesh(UPixelatedAsset* PixelatedAsset);
	
	FVector GetPoseObjectRelativeLocation(UPixelatedAsset* PixelatedAsset, const int32 RowIndex, const int32 ColumnIndex) const;
	FRotator GetPoseObjectRelativeRotation(UPixelatedAsset* PixelatedAsset) const;
	FVector GetPoseObjectRelative3DScale(UPixelatedAsset* PixelatedAsset) const;

	/*void CreateAllPoseObject(const int32 RowIndex, const int32 ColumnIndex, const FVector2D CellSize);*/
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	static APixelatedCaptureActor* GetPixelatedCaptureSpawnActor();
};
