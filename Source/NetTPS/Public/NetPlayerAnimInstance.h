// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UNetPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyAnimSettings")
	bool bHasPistol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyAnimSettings")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyAnimSettings")
	float Speed;

	//������ ���庯��
	UPROPERTY()
	class ANetTPSCharacter* Player;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "MyAnimSettings")
	class UAnimMontage* FireMontage;

	//�ѽ�� ����Լ�
	void PlayerFireAnimation();

	// pitch ȸ����(���� �ü��������� ������ �� �ֵ���)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MyAnimSettings")
	float PitchAngle;
	

public:
	//������
	UPROPERTY(EditDefaultsOnly, Category = "MyAnimSettings")
	class UAnimMontage* ReloadMontage;
	void PlayReloadAnimation();

	//������ �������� ȣ��� Modify �̺�Ʈ �Լ�
	UFUNCTION()
	void AnimNotify_OnReloadFinish();
		

	//����ó��
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MyAnimSettings")
	bool IsDead = false;
	
};
