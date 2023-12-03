#pragma once

#include "CoreMinimal.h"
#include "PixelateData.h"
#include "Toolkits/AssetEditorToolkit.h"

class FPixelateDataEditorToolkit : public FAssetEditorToolkit
{
public:
	// 外部调用的入口，它可以是任意名字，可以具有任意参数。
	void InitEditor(const TArray<UObject*>& InObjects);

	// 必须实现的虚函数
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual FName GetToolkitFName() const override { return "PixelateDataEditor"; }
	virtual FText GetBaseToolkitName() const override { return INVTEXT("PixelateData Editor"); }
	virtual FString GetWorldCentricTabPrefix() const override { return "PixelateData"; }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return {}; }

	float GetMean() const;
	float GetStandardDeviation() const;
	void SetMean(float Mean);
	void SetStandardDeviation(float StandardDeviation);
	
private:
	
	UPixelateData* PixelateData = nullptr;
};