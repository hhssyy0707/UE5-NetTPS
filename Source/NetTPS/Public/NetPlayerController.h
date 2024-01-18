// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API ANetPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	//리스폰 역할은 GameMode가 담당한다.
	UPROPERTY()
	class ANetTPSGameMode* GMNetTPS;

public:
	virtual void BeginPlay() override;

	//Respawn Server RPC
	UFUNCTION(Server, Reliable)
	void ServerRPC_RespawnPlayer();

	//Main UI 생성(단한번만 생성하기 위해 PlayerController로 이동)
public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainUI> MainUIWidget;
	
	UPROPERTY()
	class UMainUI* MainUI;
};
