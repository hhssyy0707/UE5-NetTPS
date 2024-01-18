// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTPSGameMode.h"

void ANetPlayerController::BeginPlay()
{	
	Super::BeginPlay();

	//�����϶��� GameMode �ν��Ͻ� �Ҵ�
	if (HasAuthority()) {
		GMNetTPS = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
	}
}

void ANetPlayerController::ServerRPC_RespawnPlayer_Implementation()
{
	//1. ���� �÷��̾ Unpossess 
	auto DiedPlayer = GetPawn();
	UnPossess();

	//2. Destroy
	DiedPlayer->Destroy();
	
	//3. Respawn
	GMNetTPS->RestartPlayer(this);

}
