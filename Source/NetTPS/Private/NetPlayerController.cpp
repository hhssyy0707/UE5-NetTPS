// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTPSGameMode.h"
#include "GameFramework/SpectatorPawn.h"
#include "NetTPSCharacter.h"

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

void ANetPlayerController::ServerRPC_ChangeToSpectator_Implementation()
{
	//1. ���� Pawn �ʿ�
	//2. ������ ����
	//3. Possess -> Transform ���� �ʿ�
	//4. ���� Pawn ����

	auto Player1  = GetPawn();
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto SpectatorInfo = GMNetTPS->SpectatorClass;
	auto TransfromInfo = Player1->GetActorTransform();
	auto Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorInfo, TransfromInfo, Params);
	Possess(Spectator);
	Player1->Destroy();

	//5�� �Ŀ� Respawn
	//FTimerHandle TimerHandler;
	//GetWorld()->GetTimerManager().SetTimer(TimerHandler, FTimerDelegate::CreateLambda([this]()->void {
	//	GetWorld()->SpawnActor<ANetTPSCharacter>();
	//	}),5,false);
	FTimerHandle TimerHandler;
	GetWorldTimerManager().SetTimer(TimerHandler, this, &ANetPlayerController::ServerRPC_RespawnPlayer_Implementation,5,false); // ���� ��Ŷ�� �Ҹ��� �ʿ����(�������� ������ RPC ȣ���� �ʿ����)

}
