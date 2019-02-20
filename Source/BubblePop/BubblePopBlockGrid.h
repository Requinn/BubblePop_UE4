// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BubblePopBlock.h"
#include "SoundDefinitions.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "GameFramework/Actor.h"
#include "BubblePopBlockGrid.generated.h"
class UAudioComponent;

/** Class used to spawn blocks and manage score */
UCLASS(minimalapi)
class ABubblePopBlockGrid : public AActor
{
	GENERATED_BODY()

	/** Dummy root component */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	/** Text component for the score */
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* ScoreText;

public:
	ABubblePopBlockGrid();

	/* Array to hold all the blocks in the grid*/
	//column major array
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	TArray<ABubblePopBlock*> blockArray;

	void FillDownGrid();
	void CheckForSimilar(const ABubblePopBlock& instigator);
	void ClearGridAndAddPoints(TArray<ABubblePopBlock*>& blocksToClear);
	void GenerateNewBlocks(TArray<FIntPoint>& fillFromPoints);

	ABubblePopBlock* GetBlockAtLocation(const int& row, const int& col);
	/** How many blocks have been clicked */
	int32 Score;

	/** Number of blocks along each side of grid */
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
		int32 width;
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	int32 height;

	/** Spacing of blocks */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSpacing;

protected:
	// Begin AActor interface
	virtual void BeginPlay() override;
	// End AActor interface

public:

	/** Handle the block being clicked */
	void CheckBlockInBoard(ABubblePopBlock* block);

	/** Returns DummyRoot subobject **/
	FORCEINLINE class USceneComponent* GetDummyRoot() const { return DummyRoot; }
	/** Returns ScoreText subobject **/
	FORCEINLINE class UTextRenderComponent* GetScoreText() const { return ScoreText; }

	UPROPERTY()
	USoundCue* popSound;
	UPROPERTY()
	USoundCue* backgroundMusic;
	UPROPERTY(BlueprintReadOnly)
	UAudioComponent* bgmSource;
	UPROPERTY(BlueprintReadOnly)
	UAudioComponent* popSource;
private:
	/** Pointer to white material used on the focused block */
	UPROPERTY()
		class UMaterial* BaseMaterial;

	/** Pointer to blue material used on inactive blocks */
	UPROPERTY()
		class UMaterialInstance* BlueMaterial;

	/** Pointer to orange material used on active blocks */
	UPROPERTY()
		class UMaterialInstance* OrangeMaterial;

	/** Pointer to orange material used on active blocks */
	UPROPERTY()
		class UMaterialInstance* RedMaterial;
	/** Pointer to orange material used on active blocks */
	UPROPERTY()
		class UMaterialInstance* GreenMaterial;
	/** Pointer to orange material used on active blocks */
	UPROPERTY()
		class UMaterialInstance* PurpleMaterial;
	UPROPERTY()
		class UMaterialInstance* YellowMaterial;
	UPROPERTY()
		class UMaterialInstance* PinkMaterial;
};



