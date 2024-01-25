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

	//�����϶��� GameMode �ν��Ͻ� �Ҵ�
	if (HasAuthority()) {
		GMNetTPS = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
	}
}

void ANetPlayerController::ServerRPC_RespawnPlayer_Implementation()
{
	//1. ���� �÷��̾ Unpossess 
	auto DiedPlayer = GetPawn();
	//2. �긦 UnPosess
	UnPossess();

	//3. Destroy
	DiedPlayer->Destroy();
	
	//4. Respawn
	//240125 �ּ�ó�� �Ʒ� �ڵ�� ��ü
	//GMNetTPS->RestartPlayer(this);

	//������ġ
	//GMNetTPS->RestartPlayerAtTransform(RandomPosition�� �Է�);

	//240125 �߰�
	//���忡 PALYER START2������� 
	//Ȧ���� 0��° ���� ¦���� 1��°�� ����
	// �������, �ѿ��� ������� �� 0��° ���� ���� , �������� �� 1��° ���� ���� ��

	// NetTPSGameMode�� �̵�
	/*
	TArray<APlayerStart*> playerStarts;
	playerStarts.SetNum(2); // ������ �����ֱ�
	for (TActorIterator<APlayerStart> it(GetWorld()); it; ++it) {
		APlayerStart* ps = *it;
		if (ps->ActorHasTag(TEXT("Blue"))) {
			playerStarts[0] = ps;
		}
		else {
			playerStarts[1] = ps;

		}
	}*/

	//�װ� �ٽø����� �ȵǱ淡 �߰�
	GMNetTPS->RestartPlayer(this);
	//GMNetTPS->ChoosePlayerStart_Implementation(this);
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
	GetWorldTimerManager().SetTimer(TimerHandler, this, &ANetPlayerController::ServerRPC_RespawnPlayer_Implementation,3,false); // ���� ��Ŷ�� �Ҹ��� �ʿ����(�������� ������ RPC ȣ���� �ʿ����)

}
