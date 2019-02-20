// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BubblePopBlockGrid.h"
#include "Components/TextRenderComponent.h"
#include "RenderCore.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "Materials/MaterialInstance.h"
#include <algorithm>
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"
ABubblePopBlockGrid::ABubblePopBlockGrid()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RedMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> GreenMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PurpleMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> YellowMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PinkMaterial;
		ConstructorHelpers::FObjectFinderOptional<USoundCue> popSound;
		ConstructorHelpers::FObjectFinderOptional<USoundCue> bgm;
		FConstructorStatics()
			: BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			, RedMaterial(TEXT("/Game/Puzzle/Meshes/RedMaterial.RedMaterial"))
			, GreenMaterial(TEXT("/Game/Puzzle/Meshes/GreenMaterial.GreenMaterial"))
			, PurpleMaterial(TEXT("/Game/Puzzle/Meshes/PurpleMaterial.PurpleMaterial"))
			, YellowMaterial(TEXT("/Game/Puzzle/Meshes/YellowMaterial.YellowMaterial"))
			, PinkMaterial(TEXT("/Game/Puzzle/Meshes/PinkMaterial.PinkMaterial"))
			, popSound(TEXT("/Game/Puzzle/PopSound.PopSound"))
			, bgm(TEXT("/Game/Puzzle/BGM.BGM"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Save a pointer to the materials
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	RedMaterial = ConstructorStatics.RedMaterial.Get();
	GreenMaterial = ConstructorStatics.GreenMaterial.Get();
	PurpleMaterial = ConstructorStatics.PurpleMaterial.Get();
	YellowMaterial = ConstructorStatics.YellowMaterial.Get();
	PinkMaterial = ConstructorStatics.PinkMaterial.Get();
	popSound = ConstructorStatics.popSound.Get();
	backgroundMusic = ConstructorStatics.bgm.Get();

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText0"));
	ScoreText->SetRelativeLocation(FVector(200.f,0.f,0.f));
	ScoreText->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(0)));
	ScoreText->SetupAttachment(DummyRoot);

	//get audio handler
	bgmSource = CreateDefaultSubobject<UAudioComponent>(FName("BGM"));
	bgmSource->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	popSource = CreateDefaultSubobject<UAudioComponent>(FName("Pop"));
	popSource->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	popSource->SetSound(popSound);
	bgmSource->SetSound(backgroundMusic);
	// Set defaults
	width = 10;
	height = 12;
	BlockSpacing = 300.f;
	blockArray.Init(nullptr, width * height);
}


void ABubblePopBlockGrid::BeginPlay()
{
	Super::BeginPlay();
	// Number of blocks
	const int NumBlocks = width * height;
	TArray<FIntPoint> startingGrid;
	for (int i = 0; i < width; i++) {
		startingGrid.Add({0, i});
	}
	GenerateNewBlocks(startingGrid);
	bgmSource->Play();
}

//move all blocks above an empty space down
void ABubblePopBlockGrid::FillDownGrid() {
	//this will store the values to fill up the grid from
	TArray<FIntPoint> fillFromPoints;
	//traverse each column
	int lowRowIndex = height + 1;
	//for every column
	for (int col = 0; col < width; col++) {
		lowRowIndex = height + 1;
		//for every row
		for (int row = 0; row < height; row++) {
			//check if that row,col index in the array 
			if (blockArray[(col * height) + row] != nullptr && !IsValid(blockArray[(col * height) + row])) {
				//UE_LOG(LogTemp, Warning, TEXT("Empty At %i %i"), row, col);
				if (lowRowIndex == height + 1) {
					lowRowIndex = row;
				}
			}
			else if(IsValid(blockArray[(col * height) + row]) && lowRowIndex < row){
				//UE_LOG(LogTemp, Warning, TEXT("Moving from %i %i to %i %i"), row, col, lowRowIndex, col);

				//move the lowest valid block above this down
				//swap the pointers
				//once swapped, update the value of the block we just moved to the new position it is in

				blockArray[(col * height) + row]->SetActorLocation({ lowRowIndex * BlockSpacing + GetActorLocation().X, col * BlockSpacing + GetActorLocation().Y, GetActorLocation().Z });
				std::swap(blockArray[(col * height) + lowRowIndex], blockArray[(col * height) + row]);
				blockArray[(col * height) + lowRowIndex]->UpdatePositionValue(lowRowIndex, col);

				//increment low to move up the column
				lowRowIndex++;
			}
		}
		if (lowRowIndex <= height) {
			fillFromPoints.Add({ lowRowIndex, col });
			//UE_LOG(LogTemp, Warning, TEXT("Highest empty point after moving is %i"), lowRowIndex+1);
		}
	}
	GenerateNewBlocks(fillFromPoints);
}

//generate new blocks given the lowest empty positions in a column
void ABubblePopBlockGrid::GenerateNewBlocks(TArray<FIntPoint>& fillFromPoints) {
	FDateTime time;
	FRandomStream rng(time.GetMillisecond()); //random seed

	//for every value within fill from points
	for (int i = 0; i < fillFromPoints.Num(); i++) {
		for (int row = fillFromPoints[i].X; row < height; row++) {
			const FVector BlockLocation = FVector(row * BlockSpacing, fillFromPoints[i].Y * BlockSpacing, 0.f) + GetActorLocation();

			//creat new block
			ABubblePopBlock* newBlock = GetWorld()->SpawnActor<ABubblePopBlock>(BlockLocation, FRotator(0, 0, 0));

			//if successful
			if (newBlock != nullptr) {
				//update the block's owner, position, type, and array index
				newBlock->OwningGrid = this;
				int rand = rng.RandRange(0, 6);
				switch (rand) {
				case 0:
					newBlock->AssignData(row, fillFromPoints[i].Y, BlockType::Blue, *BlueMaterial);
					break;
				case 1:
					newBlock->AssignData(row, fillFromPoints[i].Y, BlockType::Red, *RedMaterial);
					break;
				case 2:
					newBlock->AssignData(row, fillFromPoints[i].Y, BlockType::Green, *GreenMaterial);
					break;
				case 3:
					newBlock->AssignData(row, fillFromPoints[i].Y, BlockType::Orange, *OrangeMaterial);
					break;
				case 4:
					newBlock->AssignData(row, fillFromPoints[i].Y, BlockType::Purple, *PurpleMaterial);
					break;
				case 5:
					newBlock->AssignData(row, fillFromPoints[i].Y, BlockType::Yellow, *YellowMaterial);
					break;		
				case 6:
					newBlock->AssignData(row, fillFromPoints[i].Y, BlockType::Pink, *PinkMaterial);
					break;
				}
				
				blockArray[(fillFromPoints[i].Y * height) + row] = newBlock;
			}
		}
	}
}

//remove all connected matching blocks
void ABubblePopBlockGrid::ClearGridAndAddPoints(TArray<ABubblePopBlock*>& blocksToClear) {
	for (int i = 0; i < blocksToClear.Num(); i++) {
		//UE_LOG(LogTemp, Warning, TEXT("Popped %s At %i %i"), *blocksToClear[i]->GetName(), blocksToClear[i]->row, blocksToClear[i]->column);
		blocksToClear[i]->Destroy();
		Score++;
		// Update text
	}
	//replace with blueprint call?
	ScoreText->SetText(FText::Format(LOCTEXT("ScoreFmt", "Score: {0}"), FText::AsNumber(Score)));
	FillDownGrid();
}

//using a modified flood fill algorithm
void ABubblePopBlockGrid::CheckForSimilar(const ABubblePopBlock& instigator) {
	//Flood Fill Algorithm

	//Add instigator to the queue
	//while queue isn't empty
	//dequeue and check that blocks Left, Right, Up, and Down
	//if that checked block isn't the same type as the instigator, do NOT queue
	//else queue
	//then go back through the queue and add points per successful delete and pop performed
	//then call filldown
	//then call fillempty

	TQueue<ABubblePopBlock*> blocksToCheck;
	TArray<ABubblePopBlock*> blocksToClear;
	ABubblePopBlock* currentCheck = nullptr, *nextToCheck = nullptr;
	blocksToCheck.Enqueue(GetBlockAtLocation(instigator.row, instigator.column));

	while (!blocksToCheck.IsEmpty()) {
		blocksToCheck.Dequeue(currentCheck);
		if (blocksToClear.Contains(currentCheck)) {
			continue;
		}
		blocksToClear.Add(currentCheck);
		//check the left
		nextToCheck = GetBlockAtLocation(currentCheck->row - 1, currentCheck->column);
		if (nextToCheck!= nullptr && nextToCheck->AssignedType == currentCheck->AssignedType) {
			blocksToCheck.Enqueue(nextToCheck);
		}
		//right
		nextToCheck = GetBlockAtLocation(currentCheck->row + 1, currentCheck->column);
		if (nextToCheck != nullptr && nextToCheck->AssignedType == currentCheck->AssignedType) {
			blocksToCheck.Enqueue(nextToCheck);
		}
		//top
		nextToCheck = GetBlockAtLocation(currentCheck->row, currentCheck->column + 1);
		if (nextToCheck != nullptr && nextToCheck->AssignedType == currentCheck->AssignedType) {
			blocksToCheck.Enqueue(nextToCheck);
		}
		//bottom
		nextToCheck = GetBlockAtLocation(currentCheck->row, currentCheck->column - 1);
		if (nextToCheck != nullptr && nextToCheck->AssignedType == currentCheck->AssignedType) {
			blocksToCheck.Enqueue(nextToCheck);
		}
	}

	if (blocksToClear.Num() >= 2) {
		//play sound here
		popSource->Play();
		//once the array is solved, clear it up
		ClearGridAndAddPoints(blocksToClear);
	}
}

ABubblePopBlock* ABubblePopBlockGrid::GetBlockAtLocation(const int& row, const int& col)
{
	//i'm dumb and flipped height and width here
	if (row >= 0 && row < height && col >= 0 && col < width) {
		//switched to column major
		return blockArray[(col * height) + row];
	}
	return nullptr;
}


void ABubblePopBlockGrid::CheckBlockInBoard(ABubblePopBlock* block)
{
	// Increment score
	//UE_LOG(LogTemp, Warning, TEXT("Pressed Block at %i %i"), block->row, block->column);
	CheckForSimilar(*block);

}

#undef LOCTEXT_NAMESPACE
