// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSGameMode.h"
#include "NetTPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include <GameFramework/PlayerStart.h>
#include <EngineUtils.h>

ANetTPSGameMode::ANetTPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

//���Ӹ�尡 �÷��̾ �����Ϸ��� �Ҷ� ��� playerStart ��ġ���� ���� �����ϴ� �Լ�
AActor* ANetTPSGameMode::ChoosePlayerStart_Implementation(AController* Player) {

	static int currentPlayer = 0;
	TArray<APlayerStart*> playerStarts;
	
	playerStarts.SetNum(MemberNum); // ������ �����ֱ�
	int32 currentNum=0;
	
	for (TActorIterator<APlayerStart> it(GetWorld()); it; ++it) {
		APlayerStart* ps = *it;
		if (ps->ActorHasTag(TEXT("Blue"))) {
			playerStarts[currentNum] = ps;
		}
		else {
			playerStarts[currentNum] = ps;

		}
		currentNum++;
	}
	currentPlayer = (currentPlayer+1)% playerStarts.Num();
	return playerStarts[currentPlayer];
}
