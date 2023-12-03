#pragma once

#include "PixelateDataEditorToolkit.h"
#include "Widgets/Docking/SDockTab.h"
#include "SPixelateDataWidget.h"
#include "Modules/ModuleManager.h"

void FPixelateDataEditorToolkit::InitEditor(const TArray<UObject*>& InObjects)
{
	PixelateData = Cast<UPixelateData>(InObjects[0]);

	const TSharedRef<FTabManager::FLayout> Layout = 
		FTabManager::NewLayout("PixelateDataEditorLayout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()
				->SetSizeCoefficient(0.6f)
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->AddTab("PixelateDataPDFTab", ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->AddTab("PixelateDataDetailsTab", ETabState::OpenedTab)
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.4f)
				->AddTab("OutputLog", ETabState::OpenedTab)
			)
		);

	FAssetEditorToolkit::InitAssetEditor(EToolkitMode::Standalone, {}, "PixelateDataEditor", Layout, true, true, InObjects);
}

void FPixelateDataEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	WorkspaceMenuCategory = 
		InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("PixelateDataTabs"));

	// 注册 SPixelateDataWidget TabSpawner
	InTabManager->RegisterTabSpawner("PixelateDataPDFTab", 
		FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
		{
			return SNew(SDockTab)
			[
				SNew(SPixelateDataWidget)
				.Mean(this, &FPixelateDataEditorToolkit::GetMean)
				.StandardDeviation(this, &FPixelateDataEditorToolkit::GetStandardDeviation)
				.OnMeanChanged(this, &FPixelateDataEditorToolkit::SetMean)
				.OnStandardDeviationChanged(this, &FPixelateDataEditorToolkit::SetStandardDeviation)
			];
		}))
	.SetDisplayName(INVTEXT("PDF"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	// 创建 PixelateData DetailsView
	FPropertyEditorModule& PropertyEditorModule = 
		FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObjects(TArray<UObject*>{ PixelateData });

	// 注册 PixelateData DetailsView TabSpawner
	InTabManager->RegisterTabSpawner("PixelateDataDetailsTab", 
		FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
		{
			return SNew(SDockTab)
			[
				DetailsView
			];
		}))
	.SetDisplayName(INVTEXT("Details"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FPixelateDataEditorToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner("PixelateDataPDFTab");
	InTabManager->UnregisterTabSpawner("PixelateDataDetailsTab");
}

float FPixelateDataEditorToolkit::GetMean() const
{
	return PixelateData->Mean;
}

float FPixelateDataEditorToolkit::GetStandardDeviation() const
{
	return PixelateData->StandardDeviation;
}

void FPixelateDataEditorToolkit::SetMean(float Mean)
{
	PixelateData->Modify();
	PixelateData->Mean = Mean;
}

void FPixelateDataEditorToolkit::SetStandardDeviation(float StandardDeviation)
{
	PixelateData->Modify();
	PixelateData->StandardDeviation = StandardDeviation;
}