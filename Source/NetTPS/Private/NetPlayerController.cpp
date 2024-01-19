// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTPSGameMode.h"
#include "GameFramework/SpectatorPawn.h"
#include "NetTPSCharacter.h"

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
	UnPossess();

	//2. Destroy
	DiedPlayer->Destroy();
	
	//3. Respawn
	GMNetTPS->RestartPlayer(this);

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
	GetWorldTimerManager().SetTimer(TimerHandler, this, &ANetPlayerController::ServerRPC_RespawnPlayer_Implementation,5,false); // 굳이 패킷을 소모할 필요없음(서버에서 서버로 RPC 호출할 필요없음)

}
