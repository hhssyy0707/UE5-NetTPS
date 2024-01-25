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

//게임모드가 플레이어를 스폰하려고 할때 어느 playerStart 위치에서 할지 결정하는 함수
AActor* ANetTPSGameMode::ChoosePlayerStart_Implementation(AController* Player) {

	static int currentPlayer = 0;
	TArray<APlayerStart*> playerStarts;
	
	playerStarts.SetNum(MemberNum); // 사이즈 정해주기
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
