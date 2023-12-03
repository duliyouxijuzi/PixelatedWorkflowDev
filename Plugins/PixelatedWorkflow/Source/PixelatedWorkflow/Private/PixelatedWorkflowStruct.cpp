#include "PixelatedWorkflowStruct.h"
#include "Interfaces/IPluginManager.h"
#include "HAL/PlatformFilemanager.h"
#include "GenericPlatform/GenericPlatformFile.h"

FPixelatedPathSettings::FPixelatedPathSettings(){
	PixelatedDataPath.Path = FPaths::Combine(IPluginManager::Get().FindPlugin("PixelatedWorkflow")->GetContentDir(), TEXT("PixelatedData"));
    PixelatedGenerateTexturePath.Path = FPaths::Combine(IPluginManager::Get().FindPlugin("PixelatedWorkflow")->GetContentDir(), TEXT("PixelatedGenerateTexture"));
	PixelatedOutputPngPath.Path = FPaths::Combine(FPaths::ProjectDir(), TEXT("PixelatedOutputPng"));

	if (isGenerateDirectory)
	{
		GenerateDirectory(PixelatedDataPath.Path);
		GenerateDirectory(PixelatedGenerateTexturePath.Path);
		GenerateDirectory(PixelatedOutputPngPath.Path);
	}
}

void FPixelatedPathSettings::GenerateDirectory(FString DirPath){
	// 获取文件系统管理器
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    // 判断路径是否存在
    if (!PlatformFile.DirectoryExists(*DirPath))
    {
        // 生成文件夹
        if (PlatformFile.CreateDirectoryTree(*DirPath))
        {
            UE_LOG(LogTemp, Warning, TEXT("Folder created: %s"), *DirPath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create folder: %s"), *DirPath);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Folder already exists: %s"), *DirPath);
    }
}