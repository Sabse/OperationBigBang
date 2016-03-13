// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "OperationBigBang.h"
#include "OperationBigBangGameMode.h"
#include "OperationBigBangPlayerController.h"
#include "OperationBigBangCharacter.h"

AOperationBigBangGameMode::AOperationBigBangGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AOperationBigBangPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
}
