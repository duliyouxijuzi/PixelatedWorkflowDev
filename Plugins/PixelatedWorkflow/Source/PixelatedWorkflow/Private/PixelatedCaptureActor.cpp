// Fill out your copyright notice in the Description page of Project Settings.

// plugin header file
#include "PixelatedCaptureActor.h"
#include "PixelatedWorkflowStruct.h"

// Components header file
#include "Components/SceneComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkeletalMeshComponent.h"

// other
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMesh.h"

// Sets default values
APixelatedCaptureActor::APixelatedCaptureActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create components
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	CaptureRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CaptureRoot"));
	NormalCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("NormalCapture"));
	BaseColorCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("BaseColorCapture"));
	ResolutionText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ResolutionText"));
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	CellsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CellsRoot"));
	DirectionalLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("DirectionalLight"));

	// Attach components
	SetRootComponent(DefaultSceneRoot);
	Root->SetupAttachment(DefaultSceneRoot);
	CaptureRoot->SetupAttachment(Root);
	NormalCapture->SetupAttachment(CaptureRoot);
	BaseColorCapture->SetupAttachment(CaptureRoot);
	ResolutionText->SetupAttachment(CaptureRoot);
	Box->SetupAttachment(Root);
	CellsRoot->SetupAttachment(Root);
	DirectionalLight->SetupAttachment(DefaultSceneRoot);

	// Set initial
	CaptureRoot->SetRelativeLocation(FVector(-200.0f, 800.0f, -300.0f));
	Box->SetRelativeLocation(FVector(0.0f, 800.0f, -300.0f));
	DirectionalLight->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	DirectionalLight->SetRelativeRotation(FRotator(-20.0f, 0.0f, -60.0f));
}

void APixelatedCaptureActor::PixelatedCapture(UPixelatedAsset* PixelatedAsset)
{
	if (PixelatedAsset == nullptr)
	{
		return;
	}
	ResetCapture();
	CalculateCellRowAndColumn(PixelatedAsset);
	CreateAllCellBox(PixelatedAsset);
	CreateAllPoseObject(PixelatedAsset);
}

void APixelatedCaptureActor::ResetCapture()
{
	CellBoxList.Empty();
	PostActorList.Empty();
	PoseSKCMList.Empty();
}

void APixelatedCaptureActor::CalculateCellRowAndColumn(UPixelatedAsset* PixelatedAsset)
{
	PixelatedAsset->CalculateCellLayout(RowNum, ColumnNum, TexWidth, TexHeight);
}

void APixelatedCaptureActor::CreateAllCellBox(UPixelatedAsset* PixelatedAsset)
{
	int32 TotalNums = PixelatedAsset->GetCellTotalNums();
	const FVector2D CellSize = PixelatedAsset->GetPixelatedCellSize();
	for (int32 i = 0; i < TotalNums; i++)
	{
		int32 RowIndex = i % ColumnNum;
		int32 ColumnIndex = i / ColumnNum;
		CreateCellBox(RowIndex, ColumnIndex, CellSize);
	}
}

void APixelatedCaptureActor::CreateCellBox(const int32 RowIndex, const int32 ColumnIndex, const FVector2D CellSize)
{
	FVector2D CellUV = FVector2D(RowIndex + 0.5f, ColumnIndex + 0.5f) * CellSize;
	FVector Location = FVector(0, CellUV.X, CellUV.Y) * FVector(1.0f, 1.0f, -1.0f);
	UBoxComponent* CellBox = NewObject<UBoxComponent>(this, FName("BoxComponent" + FString::FromInt(RowIndex) + FString::FromInt(ColumnIndex)));
	if (CellBox)
	{
		CellBox->RegisterComponent();
		CellBox->AttachToComponent(CellsRoot, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		CellBox->SetRelativeTransform((FTransform)Location);
		CellBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CellBox->SetBoxExtent(FVector(0.f, CellSize.X * 0.5f, CellSize.Y * 0.5f));
		CellBoxList.Add(MakeShared<UBoxComponent*>(CellBox));
	}
	else
	{
		// 处理创建失败的情况
		UE_LOG(LogTemp, Warning, TEXT("Failed to create BoxComponent (%d, %d)"), RowIndex, ColumnIndex);
	}
}

void APixelatedCaptureActor::CreateAllPoseObject(UPixelatedAsset* PixelatedAsset)
{
	const FPixelatedModelSetting Model = PixelatedAsset->ModelSetting;
	switch (Model.ModelType)
	{
	case EPixelatedModelType::ActorClass:
		CreateAllPoseObjectByActorClass(PixelatedAsset);
		break;
	case EPixelatedModelType::SkeletalMesh:
		CreateAllPoseObjectBySkeletalMesh(PixelatedAsset);
		break;
	case EPixelatedModelType::StaticMesh:
		CreateAllPoseObjectByStaticMesh(PixelatedAsset);
		break;
	default:
		break;
	}
}

void APixelatedCaptureActor::PoseUpdateComponents(UPixelatedAsset* PixelatedAsset, const int32 Index, TArray<TWeakObjectPtr<USkeletalMeshComponent>> SKCMs)
{
	const float FrameDuration = PixelatedAsset->GetPerFrameDuration();
	const float PreFrameDuration = PixelatedAsset->GetPreFrameDuration(Index);
	const float CurFrameDuration = PixelatedAsset->GetCurFrameDuration(Index);
	if (!SKCMs.IsEmpty())
	{
		for (auto ObjIt = SKCMs.CreateConstIterator(); ObjIt; ++ObjIt)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = (*ObjIt).Get();
			if (SkeletalMeshComponent)
			{
				if (!PoseSKCMList.Contains(MakeShared<USkeletalMeshComponent*>(SkeletalMeshComponent)))
				{

				}
			}
		}
	}
}

void APixelatedCaptureActor::CreateAllPoseObjectByActorClass(UPixelatedAsset* PixelatedAsset)
{
	int32 TotalNums = PixelatedAsset->GetCellTotalNums();
	TSubclassOf<AActor> ActorClass = PixelatedAsset->ModelSetting.ActorClass;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FRotator Rotation = GetPoseObjectRelativeRotation(PixelatedAsset);
	FVector Scale = GetPoseObjectRelative3DScale(PixelatedAsset);
	if (ActorClass != nullptr)
	{
		for (int32 i = 0; i < TotalNums; i++)
		{
			int32 RowIndex = i % ColumnNum;
			int32 ColumnIndex = i / ColumnNum;
			FVector Location = GetPoseObjectRelativeLocation(PixelatedAsset, RowIndex, ColumnIndex);
			CreatePoseObjectByActorClass(ActorClass, Location, Rotation, Scale, SpawnParams);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Don't exit the actor of class"));
	}
}

void APixelatedCaptureActor::CreatePoseObjectByActorClass(TSubclassOf<AActor> ActorClass, const FVector Location,
	const FRotator Rotation, const FVector Scale, const FActorSpawnParameters SpawnParams)
{
	UWorld* World = GetWorld();
	if (World)
	{
		// Spawn the actor
		AActor* SpawnedActor = World->SpawnActor<AActor>(ActorClass, Location, Rotation, SpawnParams);

		if (SpawnedActor)
		{
			SpawnedActor->AttachToComponent(CellsRoot, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
			SpawnedActor->SetActorScale3D(Scale);
			PostActorList.Add(MakeShared<AActor*>(SpawnedActor));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn actor of class"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Don't Get World in func: GetPixelatedCaptureSpawnActor"));
	}
}

void APixelatedCaptureActor::CreateAllPoseObjectBySkeletalMesh(UPixelatedAsset* PixelatedAsset)
{
	int32 TotalNums = PixelatedAsset->GetCellTotalNums();
	TObjectPtr<USkeletalMesh> SkeletalMesh = PixelatedAsset->ModelSetting.SkeletalMesh;
	
	FRotator Rotation = GetPoseObjectRelativeRotation(PixelatedAsset);
	FVector Scale = GetPoseObjectRelative3DScale(PixelatedAsset);
	if (SkeletalMesh != nullptr)
	{
		for (int32 i = 0; i < TotalNums; i++)
		{
			int32 RowIndex = i % ColumnNum;
			int32 ColumnIndex = i / ColumnNum;
			FVector Location = GetPoseObjectRelativeLocation(PixelatedAsset, RowIndex, ColumnIndex);
			USkeletalMeshComponent* CellMesh = CreatePoseObjectBySkeletalMesh(FString::Printf(TEXT("SkeletalMesh%d"), i), SkeletalMesh, Location, Rotation, Scale);
			if (CellMesh)
			{
				TArray<TWeakObjectPtr<USkeletalMeshComponent>> SKCMs;
				SKCMs.Add(TWeakObjectPtr<USkeletalMeshComponent>(CellMesh));
				PoseUpdateComponents(PixelatedAsset, i, SKCMs);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Don't exit the SkeletalMesh"));
	}
}

USkeletalMeshComponent* APixelatedCaptureActor::CreatePoseObjectBySkeletalMesh(const FString Name, TObjectPtr<class USkeletalMesh> SkeletalMesh,
	const FVector Location, const FRotator Rotation, const FVector Scale)
{
	USkeletalMeshComponent* CellMesh = NewObject<USkeletalMeshComponent>(this, FName(*Name));
	if (CellMesh)
	{
		CellMesh->RegisterComponent();
		CellMesh->AttachToComponent(CellsRoot, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		CellMesh->SetRelativeLocation(Location);
		CellMesh->SetRelativeRotation(Rotation);
		CellMesh->SetRelativeScale3D(Scale);
		CellMesh->SetSkinnedAssetAndUpdate(SkeletalMesh);
		PoseSKCMList.Add(MakeShared<USkeletalMeshComponent*>(CellMesh));
		return CellMesh;
	}
	else
	{
		// 处理创建失败的情况
		UE_LOG(LogTemp, Warning, TEXT("Failed to create USkeletalMeshComponent"));
	}
	return nullptr;
}

void APixelatedCaptureActor::CreateAllPoseObjectByStaticMesh(UPixelatedAsset* PixelatedAsset)
{
}

FVector APixelatedCaptureActor::GetPoseObjectRelativeLocation(UPixelatedAsset* PixelatedAsset, const int32 RowIndex, const int32 ColumnIndex) const
{
	const FVector2D CellSize = PixelatedAsset->GetPixelatedCellSize();
	const FVector2D PivotUV = PixelatedAsset->PixelatedPivotUV;
	FVector2D Location = (FVector2D(RowIndex, ColumnIndex) + PivotUV) * CellSize * FVector2D(1.0f, -1.0f);
	return FVector(0.f, Location.X, Location.Y);
}

FRotator APixelatedCaptureActor::GetPoseObjectRelativeRotation(UPixelatedAsset* PixelatedAsset) const
{
	const FRotator PRotation = PixelatedAsset->PixelatedRotation;
	return PRotation;
}

FVector APixelatedCaptureActor::GetPoseObjectRelative3DScale(UPixelatedAsset* PixelatedAsset) const
{
	return FVector::OneVector;
}

APixelatedCaptureActor* APixelatedCaptureActor::GetPixelatedCaptureSpawnActor()
{
    UWorld* World = GWorld->GetWorld();
    if (World)
    {
        // ��ȡ���������е� APixelatedCaptureActor ���͵� Actor
        TArray<AActor*> AllAPixelatedCaptureActors;
        UGameplayStatics::GetAllActorsOfClass(World, APixelatedCaptureActor::StaticClass(), AllAPixelatedCaptureActors);

        // �������� APixelatedCaptureActor ���͵� Actor������Ƿ��Ѵ���
        for (AActor* ExistingActor : AllAPixelatedCaptureActors)
        {
            APixelatedCaptureActor* PixelatedCaptureActor = Cast<APixelatedCaptureActor>(ExistingActor);
            if (PixelatedCaptureActor)
            {
                return PixelatedCaptureActor;
            }
        }
        // ���APixelatedCaptureActorû�����ɣ��������������µ�
        APixelatedCaptureActor* PixelatedCaptureSpawnActor = World->SpawnActor<APixelatedCaptureActor>(
            APixelatedCaptureActor::StaticClass(), FVector(0,0,0), FRotator(0.f));
        return PixelatedCaptureSpawnActor;
    }
    else 
    {
        UE_LOG(LogTemp, Warning, TEXT("Don't Get World in func: GetPixelatedCaptureSpawnActor"));
    }
    
	return nullptr;
}

// Called when the game starts or when spawned
void APixelatedCaptureActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APixelatedCaptureActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

