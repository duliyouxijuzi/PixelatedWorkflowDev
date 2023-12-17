// Copyright 2023 JunkGames. All Rights Reserved.


#include "AnimSpriteRenderComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AnimSpriteSet.h"
#include "AnimSpriteData.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"


void UAnimSpritePlayer::CalcFrameForTime(float Time, int& OutLoop, int& OutFrame)const
{
	FAnimSpriteData Animation;
	const bool bAnimationValid = GetCurrentAnimation(Animation);

	if (!bAnimationValid)
	{
		OutLoop = 0;
		OutFrame = 0;
		return;
	}

	// Base : virtual ZERO time at the first loop
	const float PositionFromBase = PlaybackContext.StartPositionOffset + (Time - PlaybackContext.PlayStartTime) * PlaybackContext.PlaySpeed;
	const int OverallFrame = (int)(PositionFromBase * Animation.FrameRate);
	if (PlaybackContext.bLoop)
	{
		OutLoop = OverallFrame / Animation.NumFrames;
		OutFrame = OverallFrame % Animation.NumFrames;
	}
	else
	{
		OutLoop = 0;
		OutFrame = FMath::Min(Animation.NumFrames-1, OverallFrame);
	}
}

UAnimSpritePlayer::FAnimFrameTimestamp UAnimSpritePlayer::CreateTimestamp()const
{
	FAnimFrameTimestamp Timestamp;

	const float CurrentTime = GetCurrentTime();// GetWorld()->GetTimeSeconds();

	Timestamp.AnimationName = PlaybackContext.AnimationName;
	Timestamp.CheckTime = CurrentTime;
	CalcFrameForTime(CurrentTime, Timestamp.Loop, Timestamp.Frame);

	return Timestamp;
}

void UAnimSpritePlayer::Initialize(EAnimSpritePlayMode Mode, UMaterialInstanceDynamic* TargetMID, bool bManualTime)
{
	if (IsValid(MID))
	{
		TargetMID->CopyParameterOverrides(MID);
	}
	PlayMode = Mode;
	MID = TargetMID;

	bUseManualInputTime = bManualTime;
	ManualInputTime = 0.0f;
}

void UAnimSpritePlayer::SetAnimSpriteSet(class UAnimSpriteSet* AnimSet)
{
	if (IsValid(AnimSet))
	{
		AnimationSet = AnimSet;

		FName DefaultAnimation = AnimSet->DefaultAnimation;
		Play(DefaultAnimation, true);
	}
	else
	{
		AnimationSet = nullptr;
	}
}

float UAnimSpritePlayer::Play(FName NewAnimation, bool bLoop)
{
	if (!IsValid(MID))
	{
		return 0.0f;
	}
	if (!IsValid(AnimationSet))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UAnimSpritePlayer::Play]AnimationSet is not set"));
		return 0.0f;
	}

	UpdateRootMotion();

	FAnimSpriteData* AnimationData = AnimationSet->Animations.Find(NewAnimation);
	if (!AnimationData)
	{
		AnimationData = AnimationSet->Animations.Find(AnimationSet->DefaultAnimation);
		NewAnimation = AnimationSet->DefaultAnimation;
	}
	if (AnimationData)
	{
		const float Time = GetCurrentTime();// GetWorld()->GetTimeSeconds();
		const float StartPositionOffset = 0.0f;
		
		AnimationData->SetupMaterialInstanceDynamic(MID, Time, PlaybackContext.RotationIndex, PlaybackContext.PlaySpeed, StartPositionOffset, bLoop);

		PlaybackContext.PlayStartTime = Time;
		PlaybackContext.StartPositionOffset = StartPositionOffset;
		PlaybackContext.bLoop = bLoop;
		PlaybackContext.bPause = false;
		PlaybackContext.AnimationName = NewAnimation;
		PlaybackContext.bFinishedCallbackDone = false;
				
		UpdateRootMotion();

		static const float SLOW_SPEED = 0.001f;
		if (FMath::Abs(PlaybackContext.PlaySpeed) < SLOW_SPEED)
		{
			return 0.0f;
		}
		else if (PlaybackContext.PlaySpeed > 0.0f)
		{
			return (AnimationData->GetDuration() - StartPositionOffset) / PlaybackContext.PlaySpeed;
		}
		else
		{
			return -StartPositionOffset / PlaybackContext.PlaySpeed;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UAnimSpritePlayer::Play]Animation %s was not found in AnimationSet %s"), *NewAnimation.ToString(), *AnimationSet->GetName());
		return 0.0f;
	}
}


void UAnimSpritePlayer::Pause()
{
	if (PlaybackContext.bPause)
	{
		return;
	}

	PlaybackContext.StartPositionOffset = GetCurrentPosition();
	PlaybackContext.bPause = true;

	if (IsValid(MID))
	{
		MID->SetScalarParameterValue(TEXT("PlaySpeed"), 0.0f);
		MID->SetScalarParameterValue(TEXT("StartPositionOffset"), PlaybackContext.StartPositionOffset);		
	}
}

void UAnimSpritePlayer::Resume()
{
	if (!PlaybackContext.bPause)
	{
		return;
	}

	PlaybackContext.bPause = false;

	const float CurrentTime = GetCurrentTime();// GetWorld()->GetTimeSeconds();
	PlaybackContext.PlayStartTime = CurrentTime;

	if (IsValid(MID))
	{
		MID->SetScalarParameterValue(TEXT("PlaySpeed"), PlaybackContext.PlaySpeed);
		MID->SetScalarParameterValue(TEXT("StartPositionOffset"), PlaybackContext.StartPositionOffset);
		MID->SetScalarParameterValue(TEXT("PlayStartTime"), CurrentTime);
	}

	UpdateRootMotion();
}

void UAnimSpritePlayer::SetManualTime(float NewTime)
{
	ManualInputTime = NewTime;

	if (IsValid(MID))
	{
		MID->SetScalarParameterValue(TEXT("ManualInputTime"), NewTime);
	}
}

float UAnimSpritePlayer::GetCurrentTime()const
{
	if (bUseManualInputTime)
	{
		return ManualInputTime;
	}
	else
	{
		const UWorld* W = GetWorld();
		if (IsValid(W))
		{
			return W->GetTimeSeconds();
		}
		else
		{
			return 0.0f;
		}
	}
}

bool UAnimSpritePlayer::HasAnimation(FName AnimationName)const
{
	if (!IsValid(AnimationSet))
	{
		return false;
	}

	return AnimationSet->Animations.Contains(AnimationName);
}

float UAnimSpritePlayer::GetCurrentPosition()const
{
	if (!IsValid(AnimationSet))
	{
		return 0.0f;
	}

	const FAnimSpriteData* AnimationData = AnimationSet->Animations.Find(PlaybackContext.AnimationName);
	if (AnimationData == nullptr)
	{
		return 0.0f;
	}

	const float CurrentTime = GetCurrentTime();// GetWorld()->GetTimeSeconds();
	const float ElapsedTime = (CurrentTime - PlaybackContext.PlayStartTime) * PlaybackContext.PlaySpeed + PlaybackContext.StartPositionOffset;

	const float AnimDuration = AnimationData->GetDuration();
	const float CurrentPosition = PlaybackContext.bLoop ? FMath::Fmod(ElapsedTime, AnimDuration) : FMath::Clamp(ElapsedTime, 0.0f, AnimDuration);

	return CurrentPosition;
}

void UAnimSpritePlayer::SetPlaySpeed(float NewSpeed)
{
	if (PlaybackContext.PlaySpeed == NewSpeed)
	{
		return;
	}

	// calc current position and save as start position offset
	const float CurrentTime = GetCurrentTime();// GetWorld()->GetTimeSeconds();
	const float ElapsedTime = (CurrentTime - PlaybackContext.PlayStartTime) * PlaybackContext.PlaySpeed + PlaybackContext.StartPositionOffset;

	if (!IsValid(AnimationSet))
	{
		return;
	}
	const FAnimSpriteData* AnimationData = AnimationSet->Animations.Find(PlaybackContext.AnimationName);

	if (AnimationData != nullptr)
	{
		const float AnimDuration = AnimationData->GetDuration();
		const float CurrentPosition = PlaybackContext.bLoop ? FMath::Fmod(ElapsedTime, AnimDuration) : FMath::Clamp(ElapsedTime, 0.0f, AnimDuration);

		PlaybackContext.StartPositionOffset = CurrentPosition;
	}
	else
	{
		PlaybackContext.StartPositionOffset = 0.0f;
	}

	PlaybackContext.PlaySpeed = NewSpeed;

	if (IsValid(MID))
	{
		if (PlaybackContext.bPause)
		{
			MID->SetScalarParameterValue(TEXT("PlaySpeed"), 0.0f);
		}
		else
		{
			MID->SetScalarParameterValue(TEXT("PlaySpeed"), PlaybackContext.PlaySpeed);
		}
		MID->SetScalarParameterValue(TEXT("StartPositionOffset"), PlaybackContext.StartPositionOffset);
	}

	UpdateRootMotion();
}

void UAnimSpritePlayer::SetRotationIndex(int NewIndex)
{
	if (!IsValid(MID))
	{
		return;
	}

	PlaybackContext.RotationIndex = NewIndex;
	MID->SetScalarParameterValue(TEXT("DesiredRotationIndex"), NewIndex);
}

void UAnimSpritePlayer::SetReverseX(bool bNewReverseX)
{
	if (!IsValid(MID))
	{
		return;
	}

	PlaybackContext.bReverseX = bNewReverseX;
	MID->SetScalarParameterValue(TEXT("UScale"), bNewReverseX ? -1.0f : 1.0f);
}

bool UAnimSpritePlayer::GetCurrentAnimation(FAnimSpriteData& Animation)const
{
	if (!IsValid(AnimationSet))
	{
		return false;
	}

	const FAnimSpriteData* AnimationData = AnimationSet->Animations.Find(PlaybackContext.AnimationName);
	if (AnimationData)
	{
		Animation = *AnimationData;
		return true;
	}
	else
	{
		return false;
	}
}

bool UAnimSpritePlayer::GetSocketNames(TArray<FName> &OutSocketNames)const
{
	FAnimSpriteData Animation;
	if (GetCurrentAnimation(Animation))
	{
		Animation.Sockets.GetKeys(OutSocketNames);
		return true;
	}
	else
	{
		return false;
	}
}

bool UAnimSpritePlayer::GetSocketTransform(const FName SocketName, FTransform& OutTransform)const
{
	FAnimSpriteData Animation;
	if (GetCurrentAnimation(Animation))
	{
		const int FrameIndex = FMath::Clamp( FMath::FloorToInt( GetCurrentPosition() * Animation.FrameRate ), 0, Animation.NumFrames-1);
		const int RotationIndex = PlaybackContext.RotationIndex;
		if (Animation.GetSocketTransform(SocketName, FrameIndex, RotationIndex, OutTransform))
		{
			if (PlaybackContext.bReverseX)
			{
				OutTransform = OutTransform * FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(1.0f, -1.0f, 1.0f));
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}


void UAnimSpritePlayer::UpdateRootMotion()
{
	const FAnimFrameTimestamp NewTimestamp = CreateTimestamp();

	if (NewTimestamp.AnimationName == PlaybackContext.FrameTimeStamp.AnimationName)
	{
		// apply root motions
		FAnimFrameTimestamp OldTimestamp = PlaybackContext.FrameTimeStamp;

		FAnimSpriteData Animation;
		if (GetCurrentAnimation(Animation))
		{
			while (OldTimestamp.Loop < NewTimestamp.Loop)
			{
				while (OldTimestamp.Frame < Animation.NumFrames-1)
				{
					// get root motion
					FTransform RootMotion;
					Animation.GetRootMotion(OldTimestamp.Frame + 1, PlaybackContext.RotationIndex, RootMotion);
					PlaybackContext.AccumulatedRootMotion = PlaybackContext.AccumulatedRootMotion * RootMotion;
					OldTimestamp.Frame++;
				}

				OldTimestamp.Loop++;
				OldTimestamp.Frame = 0;
			}

			if (OldTimestamp.Loop == NewTimestamp.Loop)
			{
				while (OldTimestamp.Frame < NewTimestamp.Frame)
				{
					// get root motion
					FTransform RootMotion;
					Animation.GetRootMotion(OldTimestamp.Frame + 1, PlaybackContext.RotationIndex, RootMotion);
					PlaybackContext.AccumulatedRootMotion = PlaybackContext.AccumulatedRootMotion * RootMotion;
					OldTimestamp.Frame++;
				}
			}
		}
	}

	PlaybackContext.FrameTimeStamp = NewTimestamp;
}

FTransform UAnimSpritePlayer::ConsumeRootMotion()
{
	FTransform Retval = PlaybackContext.AccumulatedRootMotion;
	PlaybackContext.AccumulatedRootMotion = FTransform::Identity;
	return Retval;
}

void UAnimSpritePlayer::TryCastAnimationFinishedCallback()
{
	if (!OnAnimationFinished.IsBound())
	{
		return;
	}

	if (PlaybackContext.bFinishedCallbackDone)
	{
		return;
	}

	FAnimSpriteData CurrentAnimation;
	if (!GetCurrentAnimation(CurrentAnimation))
	{
		return;
	}

	const float CurrentPosition = GetCurrentPosition();
	if (CurrentPosition >= CurrentAnimation.GetDuration()-KINDA_SMALL_NUMBER)
	{
		// finished
		OnAnimationFinished.Broadcast(PlaybackContext.AnimationName);		
		PlaybackContext.bFinishedCallbackDone = true;
	}
}





// Sets default values for this component's properties
UAnimSpriteRenderComponent::UAnimSpriteRenderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	static ConstructorHelpers::FObjectFinder<UStaticMesh> QuadMeshFinder(TEXT("/AnimToSprite/SM_Quad"));
	QuadMesh = QuadMeshFinder.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> QuadSDMeshFinder(TEXT("/AnimToSprite/SM_QuadSubdiv"));
	QuadSubdivMesh = QuadSDMeshFinder.Object;

}


// Called when the game starts
void UAnimSpriteRenderComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (IsValid(Player))
	{
		Player->OnAnimationFinished.AddDynamic(this, &ThisClass::OnAnimFinished);
	}
}

void UAnimSpriteRenderComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(Player))
	{
		Player->OnAnimationFinished.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UAnimSpriteRenderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bAutoRotate)
	{
		if (!IsValid(Player))
		{
			return;
		}
		UWorld* World = GetWorld();
		if (IsValid(World))
		{
			APlayerController *PC = World->GetFirstPlayerController();
			const FRotator CameraRotation = PC->PlayerCameraManager->GetCameraRotation();

			int RotationIndex = 0;
			bool bReverse = false;
			RotationToIndex(CameraRotation.UnrotateVector(GetOwner()->GetActorForwardVector()), RotationIndex, bReverse);

			Player->SetReverseX(bReverse);
			Player->SetRotationIndex(RotationIndex);
		}
	}

	if (IsValid(Player))
	{
		Player->UpdateRootMotion();

		Player->TryCastAnimationFinishedCallback();
	}
}

void UAnimSpriteRenderComponent::OnRegister()
{
	Super::OnRegister();

	{
		SpriteMaterial = UMaterialInstanceDynamic::Create(SpriteMaterialBase, this);
	}
	if (!IsValid(Player))
	{
		Player = NewObject<UAnimSpritePlayer>(this);
	}
	Player->Initialize(EAnimSpritePlayMode::SceneQuad, SpriteMaterial);

	AActor* MyOwner = GetOwner();
	if ((MyOwner != nullptr) && !IsRunningCommandlet())
	{
		RemoveUnhandledMeshComponent();

		if (MeshComponent == nullptr)
		{
			MeshComponent = NewObject<UStaticMeshComponent>(MyOwner, NAME_None, RF_Transactional);
			MeshComponent->RegisterComponentWithWorld(GetWorld());
			MeshComponent->CreationMethod = CreationMethod;
			MeshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		}
		MeshComponent->SetStaticMesh(bUseSubdividedMesh ? QuadSubdivMesh : QuadMesh);
		MeshComponent->SetMaterial(0, SpriteMaterial);
	}

//	Player->Play(NAME_None, true);
}

void UAnimSpriteRenderComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	RemoveUnhandledMeshComponent();

	if (MeshComponent)
	{
		MeshComponent->DestroyComponent();
		MeshComponent = nullptr;
	}
}

void UAnimSpriteRenderComponent::SetAnimSpriteSet(class UAnimSpriteSet* NewAnimationSet, FName InitialAnimation)
{
	if (!IsValid(NewAnimationSet))
	{
		return;
	}
	if (!IsValid(Player))
	{
		return;
	}

	Player->SetAnimSpriteSet(NewAnimationSet);

	FName Animation = InitialAnimation;
	if (InitialAnimation.IsNone())
	{
		Animation = NewAnimationSet->DefaultAnimation;
	}

	Player->Play(Animation, true);
}

float UAnimSpriteRenderComponent::Play(FName NewAnimation, bool bLoop)
{
	if (!IsValid(Player))
	{
		return 0.0f;
	}

	const float Retal = Player->Play(NewAnimation, bLoop);

	FAnimSpriteData Animation;
	if (Player->GetCurrentAnimation(Animation))
	{
		const float BoundsScale = Animation.SpriteSize.Length() / 50.0f;		
		MeshComponent->SetBoundsScale(BoundsScale);
	}

	OnAnimationFinished.Clear();

	return Retal;
}

bool UAnimSpriteRenderComponent::PlayIfChanged(FName NewAnimation, bool bLoop, float &RemainedTime)
{
	if (!IsValid(Player))
	{
		return false;
	}

	if (Player->GetCurrentAnimationName() != NewAnimation)
	{
		 RemainedTime = Play(NewAnimation, bLoop);
		 return true;
	}
	else
	{
		return false;
	}
}

bool UAnimSpriteRenderComponent::HasAnimation(FName AnimationName)const
{
	if (!IsValid(Player))
	{
		return false;
	}
	return Player->HasAnimation(AnimationName);
}

void UAnimSpriteRenderComponent::SetReverseX(bool bNewReverse, bool bForceApply)
{
	if (!IsValid(Player))
	{
		return;
	}
	Player->SetReverseX(bNewReverse);
}

FVector ModelDirectionToVector(ECaptureModelDirection Direction)
{
	switch (Direction)
	{
	case ECaptureModelDirection::YPositive:
		return FVector::YAxisVector;
	case ECaptureModelDirection::YNegative:
		return -FVector::YAxisVector;
	case ECaptureModelDirection::XPositive:
		return FVector::XAxisVector;
	case ECaptureModelDirection::XNegative:
		return -FVector::XAxisVector;
	}

	return FVector::YAxisVector;
}

bool UAnimSpriteRenderComponent::RotationToIndex(const FVector &ForwardVectorInCameraSpace, int& Index, bool& bReverseX/*, const FVector& ModelForwardVector*/)const
{
	Index = 0;
	bReverseX = false;

	if (!IsValid(Player))
	{
		return false;
	}

	FAnimSpriteData Animation;
	Player->GetCurrentAnimation(Animation);

	const FVector& ModelForwardVector = ModelDirectionToVector(Animation.ModelDirection);
	const FVector Forward = ForwardVectorInCameraSpace;// RelativeRotationToCamera.RotateVector(ModelForwardVector);

	float MaxDot = -1.0f;

	for (int i = 0; i < Animation.Rotations.Num(); ++i)
	{
		const FVector TmpForward = Animation.Rotations[i].RotateVector(ModelForwardVector);

		const float Dot = Forward | TmpForward;

		if (Dot >= MaxDot)
		{
			Index = i;
			bReverseX = false;
			MaxDot = Dot;
		}
		if (!Animation.bRotationsSymmetry)
		{
			const FVector TmpForwardReverse = TmpForward * FVector(1.0f, -1.0f, 1.0f);
			const float DotReverse = ( Forward | TmpForwardReverse ) - 0.01f;
			if (DotReverse >= MaxDot)
			{
				Index = i;
				bReverseX = true;
				MaxDot = DotReverse;
			}
		}
	}
	return true;
}

void UAnimSpriteRenderComponent::SetDepthOffset(float NewDepth)
{
	if (IsValid(Player))
	{
		if (IsValid(Player->MID))
		{
			Player->MID->SetScalarParameterValue(FName(TEXT("DepthOffset")), NewDepth);
		}
	}
}

void UAnimSpriteRenderComponent::OnAnimFinished(FName AnimationName)
{
	OnAnimationFinished.Broadcast(AnimationName);
	OnAnimationFinished.Clear();
}

bool UAnimSpriteRenderComponent::HasAnySockets()const
{
	if (IsValid(Player))
	{
		TArray<FName> SocketNames;
		Player->GetSocketNames(SocketNames);

		return !SocketNames.IsEmpty();
	}
	return false;
}

bool UAnimSpriteRenderComponent::DoesSocketExist(FName InSocketName)const
{
	if (IsValid(Player))
	{
		TArray<FName> SocketNames;
		Player->GetSocketNames(SocketNames);

		return SocketNames.Contains(InSocketName);
	}
	return false;
}

FTransform UAnimSpriteRenderComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const
{
	if (IsValid(Player))
	{
		FTransform LocalTransform;
		Player->GetSocketTransform(InSocketName, LocalTransform);
		switch (TransformSpace)
		{
		case RTS_World:
			return LocalTransform * GetComponentTransform();

		case RTS_Actor:
			if (const AActor* Actor = GetOwner())
			{
				const FTransform SocketTransform = LocalTransform * GetComponentTransform();
				return SocketTransform.GetRelativeTransform(Actor->GetTransform());
			}
			break;

		case RTS_Component:
		case RTS_ParentBoneSpace:
			return LocalTransform;

		default:
			check(false);
		}
	}

	return Super::GetSocketTransform(InSocketName, TransformSpace);
}

void UAnimSpriteRenderComponent::QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const
{
	if (IsValid(Player))
	{
		TArray<FName> SocketNames;
		Player->GetSocketNames(SocketNames);

		OutSockets.SetNum(SocketNames.Num());
		for (int i=0; i<SocketNames.Num(); ++i)
		{
			OutSockets[i] = FComponentSocketDescription(SocketNames[i], EComponentSocketType::Socket);
		}
	}
}


void UAnimSpriteRenderComponent::RemoveUnhandledMeshComponent()
{
	TArray<USceneComponent*> Children;
	GetChildrenComponents(true, Children);
	for (int i = 0; i < Children.Num(); ++i)
	{
		if (Children[i] == MeshComponent)
		{
			continue;
		}

		Children[i]->DestroyComponent();
	}
}