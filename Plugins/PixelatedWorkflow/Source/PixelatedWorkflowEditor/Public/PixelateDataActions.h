// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "AssetTypeCategories.h"
#include "PixelateData.h"

class FPixelateDataActions : public FAssetTypeActions_Base
{
public:
	FPixelateDataActions(EAssetTypeCategories::Type InAssetCategory);

	virtual UClass* GetSupportedClass() const override;
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;

	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor) override;

	void SavePixelatedFile(UPixelateData* PixelateData);
	void ExecuteAutoSetPixelateData(TArray<TWeakObjectPtr<UPixelateData>> Objects);

	bool CheckNewPixelateDataFileExist(UPixelateData* PixelateData);

	FAssetData GetPixelateDataAsset(UPixelateData* PixelateData, FString NewFile = TEXT(""));

	void SetPixelateData(UPixelateData* PixelateData);

	void RenamePixelateData(UPixelateData* PixelateData);
	void AutoMovePixelateData(UPixelateData* PixelateData);

private:

	EAssetTypeCategories::Type AssetCategory;
};
