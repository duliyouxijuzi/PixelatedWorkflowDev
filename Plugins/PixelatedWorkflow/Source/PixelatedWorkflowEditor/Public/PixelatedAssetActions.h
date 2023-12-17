// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "AssetTypeCategories.h"

class UPixelatedAsset;
class UPixelatedWorkflowSettings;

class FPixelatedAssetActions : public FAssetTypeActions_Base
{
public:
	FPixelatedAssetActions(EAssetTypeCategories::Type InAssetCategory);

	virtual UClass* GetSupportedClass() const override;
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;
	// virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor) override;

	// Action µ÷ÓÃº¯Êý
	void ExecuteAutoSetPixelatedAsset(TArray<TWeakObjectPtr<UPixelatedAsset>> Objects);
	void ExecuteSetPixelatedAssetName(TArray<TWeakObjectPtr<UPixelatedAsset>> Objects);
	void ExecuteSetPixelatedAssetPath(TArray<TWeakObjectPtr<UPixelatedAsset>> Objects);
	void ExecutePixelatedCaptureTexture(TArray<TWeakObjectPtr<UPixelatedAsset>> Objects);

	bool CheckNewPixelatedAssetFileExist(UPixelatedAsset* PixelatedAsset);
	void SavePixelatedFile(UPixelatedAsset* PixelatedAsset);
	FAssetData GetPixelatedAssetAsset(UPixelatedAsset* PixelatedAsset, FString NewFile = FString(), FString NewPath = FString());

	bool RenamePixelatedAsset(UPixelatedAsset* PixelatedAsset);
	bool CheckMovePixelatedAsset(UPixelatedAsset* PixelatedAsset);

	void CheckContentDirectoryPath(FDirectoryPath& DirectoryPath, FString DefaultPath);
	FString GetPixelatedContentAssetPath(bool forceGet = false);

private:
	UPixelatedWorkflowSettings* PixelatedWorkflowSettings = GetMutableDefault<UPixelatedWorkflowSettings>();
	EAssetTypeCategories::Type AssetCategory;
};
