// Copyright Epic Games, Inc. All Rights Reserved.

#include "MineSweepGameMode.h"
#include "MineSweepCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMineSweepGameMode::AMineSweepGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
