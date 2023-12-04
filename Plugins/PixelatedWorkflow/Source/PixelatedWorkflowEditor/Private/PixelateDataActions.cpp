// Fill out your copyright notice in the Description page of Project Settings.


#include "PixelateDataActions.h"
#include "PixelateDataEditorToolkit.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "PixelatedWorkflowSettings.h"
#include "FileHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ObjectTools.h"


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


void FPixelateDataActions::ExecuteAutoSetPixelateData(TArray<TWeakObjectPtr<UPixelateData>> Objects)
{
	const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	TArray<FAssetRenameData> AssetRenameData;
	TArray<UPixelateData*> MoveUPixelateDatas;
	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UPixelateData* PixelateData = (*ObjIt).Get();
		if (PixelateData)
		{
			if (RenamePixelateData(PixelateData))
			{
				const FString PackagePath = FPackageName::GetLongPackagePath(PixelateData->GetOutermost()->GetName());
				AssetRenameData.Add(FAssetRenameData(PixelateData, PackagePath, PixelateData->GetPixelateDataFileName()));
			}
			if (CheckMovePixelateData(PixelateData))
			{
				MoveUPixelateDatas.Add(PixelateData);
			}
		}
	}
	
	if (AssetToolsModule.Get().RenameAssets(AssetRenameData))
	{
		UE_LOG(LogTemp, Log, TEXT("Finish Rename num: %d."), AssetRenameData.Num());
		if (MoveUPixelateDatas.Num()>0)
		{
			const UPixelatedWorkflowSettings* Settings = GetDefault<UPixelatedWorkflowSettings>();
			const FString DataPath = Settings->PixelatedPathSettings.PixelatedDataPath.Path;
			for (UPixelateData* PixelateData : MoveUPixelateDatas)
			{
				if (PixelateData)
				{
					UObject* NewObject = AssetToolsModule.Get().DuplicateAsset(PixelateData->GetPixelateDataFileName(), DataPath, PixelateData);
					UPixelateData* NewPixelateData = Cast<UPixelateData>(NewObject);
					if (NewPixelateData)
					{
						SavePixelatedFile(NewPixelateData);
					}
					FAssetData CurAssetData = GetPixelateDataAsset(PixelateData);
					ObjectTools::DeleteAssets({ CurAssetData }, false);
				}
			}
		}
	}
}

void FPixelateDataActions::SavePixelatedFile(UPixelateData* PixelateData)
{
	FString Name = PixelateData->GetName();
	if (PixelateData->GetOutermost()->IsDirty())
	{
		FEditorFileUtils::PromptForCheckoutAndSave({ PixelateData->GetOutermost() }, false, false);
		UE_LOG(LogTemp, Log, TEXT("File with name %s has saved in the folder: %s."), *Name, *FPaths::GetPath(PixelateData->GetOutermost()->GetName()));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("File with name %s dose not need to save."), *Name);
	}
}

bool  FPixelateDataActions::CheckNewPixelateDataFileExist(UPixelateData* PixelateData)
{
	FString Name = PixelateData->GetName();
	FString NewName = PixelateData->GetPixelateDataFileName();
	// 使用文件管理器检查文件是否存在
	FAssetData CurAssetData = GetPixelateDataAsset(PixelateData, NewName);
	if (CurAssetData.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("File : %s already exists in the folder: %s."), *NewName, *FPaths::GetPath(PixelateData->GetOutermost()->GetName()));
		return true;
	}
	else
	{
		const FString OutName = PixelateData->GetOutermost()->GetName();
		const UPixelatedWorkflowSettings* Settings = GetDefault<UPixelatedWorkflowSettings>();
		//检查指定的文件夹下是否存在同名文件
		const FString DataPath = Settings->PixelatedPathSettings.PixelatedDataPath.Path;
		FAssetData DatatAssetData = GetPixelateDataAsset(PixelateData, NewName, DataPath);
		if (DatatAssetData.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("File with name %s already exists in the folder: %s."), *NewName, *DataPath);
			return true;
		}
		else {
			return false;
		}
	}
}

FAssetData FPixelateDataActions::GetPixelateDataAsset(UPixelateData* PixelateData, FString NewFile, FString NewPath)
{
	FString AssetPath = UKismetSystemLibrary::GetPathName(PixelateData);
	FString LongPackagePath = FPackageName::GetLongPackagePath(AssetPath);
	if (NewPath != TEXT(""))
	{
		LongPackagePath = NewPath;
	}
	if (NewFile != TEXT(""))
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

bool FPixelateDataActions::RenamePixelateData(UPixelateData* PixelateData)
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
			return true;
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("%s do not need Rename !!!"), *Name);
	}
	return false;
}

bool FPixelateDataActions::CheckMovePixelateData(UPixelateData* PixelateData)
{
	FString Name = PixelateData->GetName();
	
	// 使用文件管理器检查文件是否存在
	FAssetData CurAssetData = GetPixelateDataAsset(PixelateData);
	if (CurAssetData.IsValid())
	{
		const FString OutName = PixelateData->GetOutermost()->GetName();
		const UPixelatedWorkflowSettings* Settings = GetDefault<UPixelatedWorkflowSettings>();
		//检查指定的文件夹下是否存在同名文件
		FString NewName = PixelateData->GetPixelateDataFileName();
		const FString DataPath = Settings->PixelatedPathSettings.PixelatedDataPath.Path;
		FAssetData DatatAssetData = GetPixelateDataAsset(PixelateData, NewName, DataPath);
		if (!DatatAssetData.IsValid())
		{
			return true;
		}
	}
	return false;

}
