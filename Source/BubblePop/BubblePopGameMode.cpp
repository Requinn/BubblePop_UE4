// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BubblePopGameMode.h"
#include "BubblePopPlayerController.h"
#include "BubblePopPawn.h"

ABubblePopGameMode::ABubblePopGameMode()
{
	// no pawn by default
	DefaultPawnClass = ABubblePopPawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = ABubblePopPlayerController::StaticClass();
}
