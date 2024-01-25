// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTPSGameMode.h"
#include "GameFramework/SpectatorPawn.h"
#include "NetTPSCharacter.h"
#include <EngineUtils.h>
#include <GameFramework/PlayerStart.h>

void ANetPlayerController::BeginPlay()
{	
	Super::BeginPlay();

	//서버일때만 GameMode 인스턴스 할당
	if (HasAuthority()) {
		GMNetTPS = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
	}
}

void ANetPlayerController::ServerRPC_RespawnPlayer_Implementation()
{
	//1. 기존 플레이어를 Unpossess 
	auto DiedPlayer = GetPawn();
	//2. 얘를 UnPosess
	UnPossess();

	//3. Destroy
	DiedPlayer->Destroy();
	
	//4. Respawn
	//240125 주석처리 아래 코드로 대체
	//GMNetTPS->RestartPlayer(this);

	//랜덤위치
	//GMNetTPS->RestartPlayerAtTransform(RandomPosition등 입력);

	//240125 추가
	//월드에 PALYER START2개만들고 
	//홀수는 0번째 들어가고 짝수는 1번째에 들어가고
	// 예를들면, 롤에서 블루팀은 다 0번째 팀에 들어가고 , 레드팀은 다 1번째 팀에 들어가는 것

	// NetTPSGameMode로 이동
	/*
	TArray<APlayerStart*> playerStarts;
	playerStarts.SetNum(2); // 사이즈 정해주기
	for (TActorIterator<APlayerStart> it(GetWorld()); it; ++it) {
		APlayerStart* ps = *it;
		if (ps->ActorHasTag(TEXT("Blue"))) {
			playerStarts[0] = ps;
		}
		else {
			playerStarts[1] = ps;

		}
	}*/

	//죽고 다시리스폰 안되길래 추가
	GMNetTPS->RestartPlayer(this);
	//GMNetTPS->ChoosePlayerStart_Implementation(this);
}

void ANetPlayerController::ServerRPC_ChangeToSpectator_Implementation()
{
	//1. 기존 Pawn 필요
	//2. 관전자 스폰
	//3. Possess -> Transform 정보 필요
	//4. 기존 Pawn 제거

	auto Player1  = GetPawn();
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto SpectatorInfo = GMNetTPS->SpectatorClass;
	auto TransfromInfo = Player1->GetActorTransform();
	auto Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorInfo, TransfromInfo, Params);
	Possess(Spectator);
	Player1->Destroy();

	//5초 후에 Respawn
	//FTimerHandle TimerHandler;
	//GetWorld()->GetTimerManager().SetTimer(TimerHandler, FTimerDelegate::CreateLambda([this]()->void {
	//	GetWorld()->SpawnActor<ANetTPSCharacter>();
	//	}),5,false);
	FTimerHandle TimerHandler;
	GetWorldTimerManager().SetTimer(TimerHandler, this, &ANetPlayerController::ServerRPC_RespawnPlayer_Implementation,3,false); // 굳이 패킷을 소모할 필요없음(서버에서 서버로 RPC 호출할 필요없음)

}
