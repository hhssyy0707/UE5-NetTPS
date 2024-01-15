// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetActor.generated.h"

UCLASS()
class NETTPS_API ANetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MeshComp;

	//네트워크 로그찍는 함수
	void PrintNetLog();

	UPROPERTY(EditAnywhere)
	float SearchDistance = 200.f;

	void FindOwner();

public:
	//Yaw 축 회전값 동기화 변수
	UPROPERTY(ReplicatedUsing=OnRep_RotYawValue) // RotYawValue값이 변경되면 OnRep_RotYawValue(이벤트함수)가 호출된다. 
	float RotYawValue = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION()
	void OnRep_RotYawValue(); // 변수명이랑 일치해야함 OnRep_뒤에

public:
	UPROPERTY()
	class UMaterialInstanceDynamic* MatInstDymc;

	//재질에 동기화될 색상
	//UPROPERTY(Replicated) 
	UPROPERTY(ReplicatedUsing=OnRep_MatColor)
	FLinearColor MatColor;

	//Tick에서 처리하지 않기 위해
	UFUNCTION()
	void OnRep_MatColor(); 

	//색상을 변경시킬 Server RPC 함수
	UFUNCTION(Server, Reliable, WithValidation) //WithValidation 잘못된 색상을 만들면 꺼버리기
	void ServerRPC_ChangeColor(const FLinearColor NewColor);

	//색상 변경된 이벤트를 받아서 처리할 Client RPC 함수
	//커넥션 필요 -> PlayerController 사용
	UFUNCTION(Client, Unreliable )
	void ClientRPC_ChangeColor(const FLinearColor NewColor);

	//모든 유저들이 처리할 NetMulticast RPC함수
	UFUNCTION(NetMulticast, Unreliable) 
	void MultiRPC_ChangeColor(const FLinearColor NewColor);

};
