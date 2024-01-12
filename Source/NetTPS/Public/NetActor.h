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

	//��Ʈ��ũ �α���� �Լ�
	void PrintNetLog();

	UPROPERTY(EditAnywhere)
	float SearchDistance = 200.f;

	void FindOwner();

public:
	//Yaw �� ȸ���� ����ȭ ����
	UPROPERTY(ReplicatedUsing=OnRep_RotYawValue) // RotYawValue���� ����Ǹ� OnRep_RotYawValue(�̺�Ʈ�Լ�)�� ȣ��ȴ�. 
	float RotYawValue = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION()
	void OnRep_RotYawValue(); // �������̶� ��ġ�ؾ��� OnRep_�ڿ�

public:
	UPROPERTY()
	class UMaterialInstanceDynamic* MatInstDymc;

	//������ ����ȭ�� ����
	//UPROPERTY(Replicated) 
	UPROPERTY(ReplicatedUsing=OnRep_MatColor)
	FLinearColor MatColor;

	//Tick���� ó������ �ʱ� ����
	UFUNCTION()
	void OnRep_MatColor(); 

};
