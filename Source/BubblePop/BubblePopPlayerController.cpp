// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BubblePopPlayerController.h"

ABubblePopPlayerController::ABubblePopPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}
