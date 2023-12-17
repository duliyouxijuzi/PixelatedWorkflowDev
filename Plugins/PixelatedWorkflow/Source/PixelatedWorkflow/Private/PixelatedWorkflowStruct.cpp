#include "PixelatedWorkflowStruct.h"

bool FPixelatedModelSetting::IsValid() const
{
	switch (ModelType)
	{
	case EPixelatedModelType::ActorClass:
		return ActorClass != nullptr;
	case EPixelatedModelType::SkeletalMesh:
		return SkeletalMesh != nullptr;
	case EPixelatedModelType::StaticMesh:
		return StaticMesh != nullptr;
	default:
		return false;
	}
}

void FPixelatedModelSetting::Clear()
{
	ActorClass = nullptr;
	SkeletalMesh = nullptr;
}

bool FPixelatedModelSetting::operator==(const FPixelatedModelSetting& Other) const
{
	if (ModelType != Other.ModelType)
	{
		return false;
	}

	switch (ModelType)
	{
	case EPixelatedModelType::ActorClass:
		return ActorClass == Other.ActorClass;
	case EPixelatedModelType::SkeletalMesh:
		return SkeletalMesh == Other.SkeletalMesh;
	case EPixelatedModelType::StaticMesh:
		return StaticMesh == Other.StaticMesh;
	default:
		return false;
	}
}
