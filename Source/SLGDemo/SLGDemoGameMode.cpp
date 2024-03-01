// Copyright Epic Games, Inc. All Rights Reserved.

#include "SLGDemoGameMode.h"
#include "SLGDemoPlayerController.h"
#include "SLGDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASLGDemoGameMode::ASLGDemoGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ASLGDemoPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}