// Copyright Epic Games, Inc. All Rights Reserved.

#include "GasNetworkGameMode.h"
#include "GasNetworkCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGasNetworkGameMode::AGasNetworkGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
