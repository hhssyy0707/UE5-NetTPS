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
	//������ ������ GameMode�� ����Ѵ�.
	UPROPERTY()
	class ANetTPSGameMode* GMNetTPS;

public:
	virtual void BeginPlay() override;

	//Respawn Server RPC
	UFUNCTION(Server, Reliable)
	void ServerRPC_RespawnPlayer();

	//Main UI ����(���ѹ��� �����ϱ� ���� PlayerController�� �̵�)
public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainUI> MainUIWidget;
	
	UPROPERTY()
	class UMainUI* MainUI;
};
