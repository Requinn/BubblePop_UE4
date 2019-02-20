// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BubblePopBlock.generated.h"


enum class BlockType {
	Red = 0,
	Green = 1,
	Blue = 2,
	Orange = 3,
	Purple = 4,
	Yellow = 5,
	Pink = 6
};

/** A block that can be clicked */
UCLASS(minimalapi)
class ABubblePopBlock : public AActor
{
public:
	GENERATED_BODY()
	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** StaticMesh component for the clickable block */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

public:
	ABubblePopBlock();

	int row = 0, column = 0;
	BlockType AssignedType;

	//currently assigned material
	class UMaterialInstance* currentMaterial;

	/** Are we currently active? */
	bool bIsActive;

	void AssignData(int row, int column, BlockType type, UMaterialInstance& material);
	void UpdatePositionValue(const int row, const int column);

	BlockType& GetType();

	/** Grid that owns us */
	UPROPERTY()
	class ABubblePopBlockGrid* OwningGrid;

	/** Handle the block being clicked */
	UFUNCTION()
	void BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked);

	/** Handle the block being touched  */
	UFUNCTION()
	void OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	void HandleClicked();

	void Highlight(bool bOn);

public:
	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns BlockMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }
};
