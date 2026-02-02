// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actividad3GameMode.h"
#include "Actividad3Character.h"
#include "UObject/ConstructorHelpers.h"

AActividad3GameMode::AActividad3GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
