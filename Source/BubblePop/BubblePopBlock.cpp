// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

//grid first, then the block, this order matters
//could fix the circular dependency, but pragma once seems to be helping out here
#include "BubblePopBlock.h"
#include "BubblePopBlockGrid.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"

ABubblePopBlock::ABubblePopBlock()
{
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlainMesh;
		FConstructorStatics()
			: PlainMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;
	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlainMesh.Get());

	BlockMesh->SetRelativeScale3D(FVector(1.f,1.f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,25.f));
	BlockMesh->SetupAttachment(DummyRoot);
	BlockMesh->OnClicked.AddDynamic(this, &ABubblePopBlock::BlockClicked);
	BlockMesh->OnInputTouchBegin.AddDynamic(this, &ABubblePopBlock::OnFingerPressedBlock);

}

void ABubblePopBlock::AssignData(int row, int col, BlockType type, UMaterialInstance& material) {
	this->row = row;
	this->column = col;
	AssignedType = type;

	BlockMesh->SetMaterial(0, &material);
}

///update the assigned position index
void ABubblePopBlock::UpdatePositionValue(const int row, const int col) {
	this->row = row;
	this->column = col;
	//UE_LOG(LogTemp, Warning, TEXT("Update Position of %s to %i %i"),*this->GetName(), row, col);
}
BlockType& ABubblePopBlock::GetType() {
	return AssignedType;
}

void ABubblePopBlock::BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked)
{
	HandleClicked();
}

void ABubblePopBlock::OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	HandleClicked();
}

void ABubblePopBlock::HandleClicked()
{
	// Check we are not already active
	if (!bIsActive)
	{
		bIsActive = true;

		// Tell the Grid
		if (OwningGrid != nullptr)
		{
			OwningGrid->CheckBlockInBoard(this);
		}
	}
}

void ABubblePopBlock::Highlight(bool bOn)
{
	//do nothing during a highlight
}
