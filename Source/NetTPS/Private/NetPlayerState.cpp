// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerState.h"
#include "NetTPSGameInstance.h"

void ANetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	auto con = GetPlayerController();

	//IsLocalController �� �ڽ�����(��������)
	if (con && con->IsLocalController()) {
		auto gi = Cast<UNetTPSGameInstance>(GetWorld()->GetGameInstance());
		ServerRPC_SetUserName(gi->hostName);
	}


}

void ANetPlayerState::ServerRPC_SetUserName_Implementation(const FString& name)
{
	SetPlayerName(name);

}
