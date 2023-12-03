// Fill out your copyright notice in the Description page of Project Settings.


#include "PixelateDataActions.h"
#include "PixelateDataEditorToolkit.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "PixelatedWorkflowSettings.h"
#include "FileHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"


FPixelateDataActions::FPixelateDataActions(EAssetTypeCategories::Type InAssetCategory)
{
	AssetCategory = InAssetCategory;
}
UClass* FPixelateDataActions::GetSupportedClass() const
{
	return UPixelateData::StaticClass();
}

FText FPixelateDataActions::GetName() const
{
	return INVTEXT("PixelateData");
}

FColor FPixelateDataActions::GetTypeColor() const
{
	return FColor::Orange;
}

uint32 FPixelateDataActions::GetCategories()
{
	return AssetCategory;
}

void FPixelateDataActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);
	auto PixelateDatas = GetTypedWeakObjectPtrs<UPixelateData>(InObjects);
	MenuBuilder.AddMenuEntry(
		FText::FromString("Auto Update PixelatedData"),
		FText::FromString("Automatically set the name and path of PixelatedData"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &FPixelateDataActions::ExecuteAutoSetPixelateData, PixelateDatas)
		)
		);
}

// HasActions() 必须返回 true 以使 GetActions() 有效
bool FPixelateDataActions::HasActions(const TArray<UObject*>& InObjects) const
{
	return true;
}

void FPixelateDataActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	MakeShared<FPixelateDataEditorToolkit>()->InitEditor(InObjects);
}

void FPixelateDataActions::SavePixelatedFile(UPixelateData* PixelateData)
{
	FString Name = PixelateData->GetName();
	if (PixelateData->GetOutermost()->IsDirty())
	{
		FEditorFileUtils::PromptForCheckoutAndSave({ PixelateData->GetOutermost() }, false, false);
		UE_LOG(LogTemp, Log, TEXT("File with name %s has saved in the folder: %s."), *Name, *FPaths::GetPath(Name));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("File with name %s dose not need to save."), *Name);
	}
}

void FPixelateDataActions::ExecuteAutoSetPixelateData(TArray<TWeakObjectPtr<UPixelateData>> Objects)
{
	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UPixelateData* PixelateData = (*ObjIt).Get();
		if (PixelateData)
		{
			SetPixelateData(PixelateData);
		}
	}
}

bool  FPixelateDataActions::CheckNewPixelateDataFileExist(UPixelateData* PixelateData)
{
	const FString OutName = PixelateData->GetOutermost()->GetName();
	FString Name = PixelateData->GetName();
	FString NewName = PixelateData->GetPixelateDataFileName();
	// 使用文件管理器检查文件是否存在
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	//首先检查当前文件所在的文件下是否有重名的文件

	// 构建新文件的相对路径
	FString CurNewFilePath = FPaths::Combine(FPaths::GetPath(OutName), NewName);
	//转换成绝对路径
	FString CurNewFileAbsolutePath = FPaths::ConvertRelativePathToFull(CurNewFilePath);
	FString CurNewFileAbsolutePath1 = FPaths::ConvertRelativePathToFull(OutName);
	/*UE_LOG(LogTemp, Log, TEXT("File Path %s File name %s ..  %s .. %s"), *CurNewFilePath,
		*FPaths::GetPath(OutName), *FPaths::GetCleanFilename(OutName), *OutName);*/
	UE_LOG(LogTemp, Log, TEXT("RPath %s Path: %s --- %s"), *CurNewFilePath, 
		*CurNewFileAbsolutePath, *CurNewFileAbsolutePath1);

	const FString AssetPath = UKismetSystemLibrary::GetPathName(PixelateData);
	const FString LongPackagePath = FPackageName::GetLongPackagePath(AssetPath);

	UE_LOG(LogTemp, Log, TEXT("RPath %s Path: %s"), *AssetPath,
		*LongPackagePath);

	FAssetData AssetData = GetPixelateDataAsset(PixelateData, NewName);
	if (AssetData.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("File : %s already exists in the folder: %s."), *NewName);
		return true;
	}
	else
	{
		const UPixelatedWorkflowSettings* Settings = GetDefault<UPixelatedWorkflowSettings>();
		//检查指定的文件夹下是否存在同名文件
		const FString DataPath = Settings->PixelatedPathSettings.PixelatedDataPath.Path;
		FString TargetNewFilePath = FPaths::Combine(DataPath, NewName);
		if (PlatformFile.FileExists(*TargetNewFilePath))
		{
			UE_LOG(LogTemp, Warning, TEXT("File with name %s already exists in the folder: %s."), *NewName, *DataPath);
			return true;
		}
		else {
			return false;
		}
	}
}

FAssetData FPixelateDataActions::GetPixelateDataAsset(UPixelateData* PixelateData, FString NewFile)
{
	FString AssetPath = UKismetSystemLibrary::GetPathName(PixelateData);
	const FString LongPackagePath = FPackageName::GetLongPackagePath(AssetPath);
	if (NewFile != TEXT(""))
	{
		AssetPath = FPaths::Combine(LongPackagePath, FString::Printf(TEXT("%s%s"),*NewFile, *FPackageName::GetAssetPackageExtension()));
	}
	FString CurNewFileAbsolutePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), AssetPath);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// 检查源文件是否存在
	if (PlatformFile.FileExists(*CurNewFileAbsolutePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Rename Fail !!!"), *CurNewFileAbsolutePath);
	}
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(*AssetPath);
	return AssetData;
}

void FPixelateDataActions::SetPixelateData(UPixelateData* PixelateData)
{
	CheckNewPixelateDataFileExist(PixelateData);
	/*RenamePixelateData(PixelateData);
	AutoMovePixelateData(PixelateData);*/
}

void FPixelateDataActions::RenamePixelateData(UPixelateData* PixelateData)
{
	// 自动命名
	FString Name = PixelateData->GetName();
	FString NewName = PixelateData->GetPixelateDataFileName();
	if (NewName != Name)
	{
		if (!CheckNewPixelateDataFileExist(PixelateData))
		{
			PixelateData->SetFlags(RF_Transactional);
			PixelateData->Modify();
			UObject* NewOuter = PixelateData->GetOuter();
			UClass* NewClass = PixelateData->GetClass();
			// 不存在同名对象，可以进行重命名操作
			if (PixelateData->Rename(*NewName, PixelateData->GetOuter(), RF_NoFlags))
			{
				UE_LOG(LogTemp, Log, TEXT("%s Rename Success !!!"), *Name);
				// 重命名成功
				SavePixelatedFile(PixelateData);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s Rename Fail !!!"), *Name);
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("%s do not need Rename !!!"), *Name);
	}
}

void FPixelateDataActions::AutoMovePixelateData(UPixelateData* PixelateData)
{
	FString Name = PixelateData->GetName();
	FString SourceFilePath = FPaths::Combine(FPaths::GetPath(Name), Name);
	// 使用文件管理类 FPlatformFileManager 进行文件移动操作
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// 检查源文件是否存在
	if (PlatformFile.FileExists(*SourceFilePath))
	{
		const UPixelatedWorkflowSettings* Settings = GetDefault<UPixelatedWorkflowSettings>();
		//检查指定的文件夹下是否存在同名文件
		const FString DataPath = Settings->PixelatedPathSettings.PixelatedDataPath.Path;
		FString TargetFilePath = FPaths::Combine(DataPath, Name);
		// 如果目标文件夹不存在，则创建目标文件夹
		if (!PlatformFile.DirectoryExists(*DataPath))
		{
			PlatformFile.CreateDirectory(*DataPath);
		}
		// 构建目标文件的完整路径
		FString DestinationFile = FPaths::Combine(DataPath, FPaths::GetCleanFilename(SourceFilePath));

		// 尝试移动文件
		if (PlatformFile.MoveFile(*DestinationFile, *SourceFilePath))
		{
			// 移动成功
			UE_LOG(LogTemp, Log, TEXT("%s move to success !!!"), *Name);
		}
		else
		{
			// 移动失败
			UE_LOG(LogTemp, Warning, TEXT("%s move to success !!!"), *Name);
		}
	}
	else
	{
		// 源文件不存在
		UE_LOG(LogTemp, Warning, TEXT("%s dose not exit！"), *SourceFilePath);
	}
}
