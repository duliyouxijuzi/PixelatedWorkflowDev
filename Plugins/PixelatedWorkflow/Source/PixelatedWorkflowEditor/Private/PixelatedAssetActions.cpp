// Fill out your copyright notice in the Description page of Project Settings.


#include "PixelatedAssetActions.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "PixelatedWorkflowSettings.h"
#include "FileHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetViewUtils.h"
#include "PixelatedAsset.h"
#include "Interfaces/IPluginManager.h"
#include "PixelatedCaptureActor.h"


FPixelatedAssetActions::FPixelatedAssetActions(EAssetTypeCategories::Type InAssetCategory)
{
	AssetCategory = InAssetCategory;
}
UClass* FPixelatedAssetActions::GetSupportedClass() const
{
	return UPixelatedAsset::StaticClass();
}

FText FPixelatedAssetActions::GetName() const
{
	return INVTEXT("PixelatedAsset");
}

FColor FPixelatedAssetActions::GetTypeColor() const
{
	return FColor::Orange;
}

uint32 FPixelatedAssetActions::GetCategories()
{
	return AssetCategory;
}

void FPixelatedAssetActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);
	auto PixelatedAssets = GetTypedWeakObjectPtrs<UPixelatedAsset>(InObjects);
	MenuBuilder.AddMenuEntry(
		FText::FromString("Auto Update PixelatedAsset"),
		FText::FromString("Automatically set the name and path of PixelatedAsset"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &FPixelatedAssetActions::ExecuteAutoSetPixelatedAsset, PixelatedAssets)
		)
	);
	MenuBuilder.AddMenuEntry(
		FText::FromString("Set PixelatedAsset Name"),
		FText::FromString("Automatically set the name PixelatedAsset"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &FPixelatedAssetActions::ExecuteSetPixelatedAssetName, PixelatedAssets)
		)
	);
	MenuBuilder.AddMenuEntry(
		FText::FromString("Set PixelatedAsset path"),
		FText::FromString("Automatically set the path PixelatedAsset"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &FPixelatedAssetActions::ExecuteSetPixelatedAssetPath, PixelatedAssets)
		)
	);
	MenuBuilder.AddMenuEntry(
		FText::FromString("Pixelated Capture Texture"),
		FText::FromString("Automatically set the path PixelatedAsset"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &FPixelatedAssetActions::ExecutePixelatedCaptureTexture, PixelatedAssets)
		)
	);
}

// HasActions() 必须返回 true 以使 GetActions() 有效
bool FPixelatedAssetActions::HasActions(const TArray<UObject*>& InObjects) const
{
	return true;
}

// void FPixelatedAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
// 	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
// {
// 	MakeShared<FPixelatedAssetEditorToolkit>()->InitEditor(InObjects);
// }


void FPixelatedAssetActions::ExecuteAutoSetPixelatedAsset(TArray<TWeakObjectPtr<UPixelatedAsset>> Objects)
{
	ExecuteSetPixelatedAssetName(Objects);
	ExecuteSetPixelatedAssetPath(Objects);
	ExecutePixelatedCaptureTexture(Objects);
}

void FPixelatedAssetActions::ExecuteSetPixelatedAssetName(TArray<TWeakObjectPtr<UPixelatedAsset>> Objects)
{
	TArray<FAssetRenameData> AssetRenameData;
	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UPixelatedAsset* PixelatedAsset = (*ObjIt).Get();
		if (RenamePixelatedAsset(PixelatedAsset))
		{
			const FString PackagePath = FPackageName::GetLongPackagePath(PixelatedAsset->GetOutermost()->GetName());
			AssetRenameData.Add(FAssetRenameData(PixelatedAsset, PackagePath, PixelatedAsset->GetPixelatedAssetFileName()));
		}
	}
	const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	if (AssetToolsModule.Get().RenameAssets(AssetRenameData))
	{
		UE_LOG(LogTemp, Log, TEXT("Finish Rename num: %d."), AssetRenameData.Num());
	}
}

void FPixelatedAssetActions::ExecuteSetPixelatedAssetPath(TArray<TWeakObjectPtr<UPixelatedAsset>> Objects)
{
	TArray<UObject*> MoveUPixelatedAssets;
	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UPixelatedAsset* PixelatedAsset = (*ObjIt).Get();
		if (PixelatedWorkflowSettings->bSetPixelatedAssetPath && CheckMovePixelatedAsset(PixelatedAsset))
		{
			MoveUPixelatedAssets.Add(PixelatedAsset);
		}
	}
	if (MoveUPixelatedAssets.Num() > 0)
	{
		AssetViewUtils::MoveAssets(MoveUPixelatedAssets, GetPixelatedContentAssetPath());

	}
}

void FPixelatedAssetActions::ExecutePixelatedCaptureTexture(TArray<TWeakObjectPtr<UPixelatedAsset>> Objects)
{
	APixelatedCaptureActor* PixelatedCaptureActor = APixelatedCaptureActor::GetPixelatedCaptureSpawnActor();
	if (PixelatedCaptureActor)
	{
		for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
		{
			UPixelatedAsset* PixelatedAsset = (*ObjIt).Get();
			if (PixelatedAsset)
			{
				PixelatedCaptureActor->PixelatedCapture(PixelatedAsset);
			}
		}
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Don't Spawn Capture Actor!!!"));
	}
}

void FPixelatedAssetActions::SavePixelatedFile(UPixelatedAsset* PixelatedAsset)
{
	FString Name = PixelatedAsset->GetName();
	if (PixelatedAsset->GetOutermost()->IsDirty())
	{
		FEditorFileUtils::PromptForCheckoutAndSave({ PixelatedAsset->GetOutermost() }, false, false);
		UE_LOG(LogTemp, Log, TEXT("File with name %s has saved in the folder: %s."), *Name, *FPaths::GetPath(PixelatedAsset->GetOutermost()->GetName()));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("File with name %s dose not need to save."), *Name);
	}
}

bool  FPixelatedAssetActions::CheckNewPixelatedAssetFileExist(UPixelatedAsset* PixelatedAsset)
{
	FString Name = PixelatedAsset->GetName();
	FString NewName = PixelatedAsset->GetPixelatedAssetFileName();
	// 使用文件管理器检查文件是否存在
	FAssetData CurAssetData = GetPixelatedAssetAsset(PixelatedAsset, NewName);
	if (CurAssetData.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("File : %s already exists in the folder: %s."), *NewName, *FPaths::GetPath(PixelatedAsset->GetOutermost()->GetName()));
		return true;
	}
	else
	{
		//检查指定的文件夹下是否存在同名文件
		FAssetData DatatAssetData = GetPixelatedAssetAsset(PixelatedAsset, NewName, GetPixelatedContentAssetPath());
		if (DatatAssetData.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("File with name %s already exists in the folder: %s."), *NewName, *GetPixelatedContentAssetPath());
			return true;
		}
		else {
			return false;
		}
	}
}

FAssetData FPixelatedAssetActions::GetPixelatedAssetAsset(UPixelatedAsset* PixelatedAsset, FString NewFile, FString NewPath)
{
	FString AssetPath = UKismetSystemLibrary::GetPathName(PixelatedAsset);
	FString LongPackagePath = FPackageName::GetLongPackagePath(AssetPath);
	if (!NewPath.IsEmpty())
	{
		LongPackagePath = NewPath;
	}
	if (!NewFile.IsEmpty())
	{
		AssetPath = FPaths::Combine(LongPackagePath, FString::Printf(TEXT("%s.%s"),*NewFile, *NewFile));
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
#if ENGINE_MINOR_VERSION >= 1
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(AssetPath));
#else
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(*AssetPath);
#endif
	return AssetData;
}

bool FPixelatedAssetActions::RenamePixelatedAsset(UPixelatedAsset* PixelatedAsset)
{
	if (PixelatedAsset == nullptr)
	{
		return false;
	}
	// 自动命名
	FString Name = PixelatedAsset->GetName();
	FString NewName = PixelatedAsset->GetPixelatedAssetFileName();
	if (NewName != Name)
	{
		return !CheckNewPixelatedAssetFileExist(PixelatedAsset);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("%s do not need Rename !!!"), *Name);
	}
	return false;
}

bool FPixelatedAssetActions::CheckMovePixelatedAsset(UPixelatedAsset* PixelatedAsset)
{
	if (PixelatedAsset == nullptr)
	{
		return false;
	}
	FString Name = PixelatedAsset->GetName();
	// 使用文件管理器检查文件是否存在
	FAssetData CurAssetData = GetPixelatedAssetAsset(PixelatedAsset);
	if (CurAssetData.IsValid())
	{
		//检查指定的文件夹下是否存在同名文件
		FString NewName = PixelatedAsset->GetPixelatedAssetFileName();
		FAssetData DatatAssetData = GetPixelatedAssetAsset(PixelatedAsset, NewName, GetPixelatedContentAssetPath(true));
		if (!DatatAssetData.IsValid())
		{
			return true;
		}
	}
	return false;
}

void FPixelatedAssetActions::CheckContentDirectoryPath(FDirectoryPath& DirectoryPath, FString DefaultPath)
{
	// 获取文件系统管理器
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	// 获得Content路径
	DefaultPath = DirectoryPath.Path.IsEmpty() ? DefaultPath : DirectoryPath.Path;
	// 转换成Content相对路径
	FString ContentPath = FPaths::Combine(FPaths::ProjectContentDir(), DefaultPath);
	// 判断路径是否存在
	if (PlatformFile.DirectoryExists(*ContentPath))
	{
		DirectoryPath.Path = DefaultPath;
		UE_LOG(LogTemp, Warning, TEXT("Folder already exists: %s"), *ContentPath);
	}
	else
	{
		// 生成文件夹
		if (PlatformFile.CreateDirectoryTree(*ContentPath))
		{
			DirectoryPath.Path = DefaultPath;
			UE_LOG(LogTemp, Warning, TEXT("Folder created: %s"), *ContentPath);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create folder: %s"), *ContentPath);
		}
	}
}

FString FPixelatedAssetActions::GetPixelatedContentAssetPath(bool forceGet)
{
	if (forceGet || PixelatedWorkflowSettings->bSetPixelatedAssetPath)
	{
		CheckContentDirectoryPath(PixelatedWorkflowSettings->PixelatedAssetPath, TEXT("PixelatedAsset"));
		// Content目录下Asset应该加上Game前缀
		return FPaths::Combine(TEXT("/Game"), PixelatedWorkflowSettings->PixelatedAssetPath.Path);
	}
	else
	{
		return FString();
	}
}
