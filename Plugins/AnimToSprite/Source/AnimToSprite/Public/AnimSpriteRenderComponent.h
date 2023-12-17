// Copyright 2023 JunkGames. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AnimSpriteData.h"
#include "AnimSpriteRenderComponent.generated.h"

UENUM(BlueprintType)
enum class EAnimSpritePlayMode : uint8
{
	UIWidget,
	SceneQuad
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpriteAnimationFinished, FName, AnimName);

UCLASS(BlueprintType)
class ANIMTOSPRITE_API UAnimSpritePlayer : public UObject
{
	GENERATED_BODY()

private:
	struct FAnimFrameTimestamp
	{
		float CheckTime = 0.0f;
		int Loop = 0;
		int Frame = 0;
		FName AnimationName = NAME_None;
	};
	struct FAnimSpritePlayContext
	{
		float PlayStartTime=0.0f;
		float PlaySpeed=1.0f;
		float StartPositionOffset=0.0f;
		int RotationIndex=0;
		bool bReverseX=false;
		bool bLoop=true;
		bool bPause = false;

		FName AnimationName = NAME_None;

		// finished callback
		bool bFinishedCallbackDone = false;
		
		// for tick and root motion
		FAnimFrameTimestamp FrameTimeStamp;
		FTransform AccumulatedRootMotion=FTransform::Identity;
	} PlaybackContext;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprite")
	EAnimSpritePlayMode PlayMode=EAnimSpritePlayMode::SceneQuad;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprite")
	class UAnimSpriteSet* AnimationSet=nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprite")
	class UMaterialInstanceDynamic* MID=nullptr;

	UPROPERTY(BlueprintAssignable)
	FOnSpriteAnimationFinished OnAnimationFinished;

	UPROPERTY(BlueprintReadOnly, Category = "Sprite")
	bool bUseManualInputTime = false;
	UPROPERTY(BlueprintReadOnly, Category = "Sprite")
	float ManualInputTime = 0.0f;

private:
	void CalcFrameForTime(float Time, int& OutLoop, int& OutFrame)const;
	FAnimFrameTimestamp CreateTimestamp()const;

public:
	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void Initialize(EAnimSpritePlayMode Mode, class UMaterialInstanceDynamic *TargetMID, bool bManualTime = false);
	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void SetAnimSpriteSet(class UAnimSpriteSet* AnimSet);

	/** Play sprite animation
	* Returns the expected duration (if loop, duration until first wrapping)
	*/
	UFUNCTION(BlueprintCallable, Category = "Sprite")
	float Play(FName NewAnimation, bool bLoop);
	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void Pause();
	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void Resume();

	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void SetManualTime(float NewTime);

	UFUNCTION(BlueprintPure, Category = "Sprite")
	float GetCurrentTime()const;

	UFUNCTION(BlueprintPure, Category = "Sprite")
	bool HasAnimation(FName AnimationName)const;

	UFUNCTION(BlueprintPure, Category = "Sprite")
	float GetCurrentPosition()const;

	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void SetPlaySpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void SetRotationIndex(int NewIndex);
	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void SetReverseX(bool bNewReverseX);

	UFUNCTION(BlueprintPure, Category = "Sprite")
	bool GetCurrentAnimation(FAnimSpriteData& Animation)const;

	UFUNCTION(BlueprintPure, Category = "Sprite")
	bool GetSocketNames(TArray<FName>& OutSocketNames)const;

	UFUNCTION(BlueprintPure, Category = "Sprite")
	bool GetSocketTransform(const FName SocketName, FTransform& OutTransform)const;


	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void UpdateRootMotion();
	UFUNCTION(BlueprintCallable, Category = "Sprite")
	FTransform ConsumeRootMotion();

	UFUNCTION(BlueprintPure, Category = "Sprite")
	FName GetCurrentAnimationName()const { return PlaybackContext.AnimationName; }

	void TryCastAnimationFinishedCallback();
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ANIMTOSPRITE_API UAnimSpriteRenderComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnimSpriteRenderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnRegister() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void SetAnimSpriteSet(class UAnimSpriteSet* NewAnimationSet, FName InitialAnimation=NAME_None);

	UFUNCTION(BlueprintCallable, Category = "Sprite")
	float Play(FName NewAnimation, bool bLoop);

	UFUNCTION(BlueprintCallable, Category = "Sprite")
	bool PlayIfChanged(FName NewAnimation, bool bLoop, float &RemainedTime);

	UFUNCTION(BlueprintCallable, Category = "Sprite")
	bool HasAnimation(FName AnimationName)const;

	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void SetReverseX(bool bNewReverse, bool bForceApply=false);

	UFUNCTION(BlueprintPure, Category = "Sprite")
	UAnimSpritePlayer* GetPlayer()const { return Player; }

	// Rotation Index suggestion
	bool RotationToIndex(const FVector &ForwardVectorInCameraSpace, int &Index, bool &bReverseX)const;

	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void SetDepthOffset(float NewDepth);

	UFUNCTION()
	void OnAnimFinished(FName AnimationName);


	// USceneComponent interface
	virtual bool HasAnySockets() const override;
	virtual bool DoesSocketExist(FName InSocketName) const override;
	virtual FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace = RTS_World) const override;
	virtual void QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const override;
	// End of USceneComponent interface

private:
	void RemoveUnhandledMeshComponent();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	class UMaterialInterface* SpriteMaterialBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	bool bAutoRotate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	bool bUseSubdividedMesh = false;

	UPROPERTY()
	UAnimSpritePlayer* Player = nullptr;

	UPROPERTY(BlueprintAssignable)
	FOnSpriteAnimationFinished OnAnimationFinished;

private:
	UPROPERTY()
	class UStaticMesh* QuadMesh;
	UPROPERTY()
	class UStaticMesh* QuadSubdivMesh;
	UPROPERTY()
	class UMaterialInstanceDynamic* SpriteMaterial;

	UPROPERTY()
	class UStaticMeshComponent* MeshComponent;
};
