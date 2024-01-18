// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTPSGameMode.h"

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
