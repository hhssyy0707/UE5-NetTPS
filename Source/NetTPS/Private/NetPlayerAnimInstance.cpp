// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"
#include "NetTPSCharacter.h"

void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Player = Cast<ANetTPSCharacter>(TryGetPawnOwner());
}

void UNetPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (Player) {
	
		// bHasPistol = true;
		
		// �÷��̾��� �ӵ��� �����ͼ�
		FVector Velocity = Player->GetVelocity();
		//Speed�� �Ҵ��ϰ� �ʹ�.
		Speed = FVector::DotProduct(Velocity, Player->GetActorForwardVector());
		//Direction�� �Ҵ��ϰ� �ʹ�.
		Direction = FVector::DotProduct(Velocity, Player->GetActorRightVector());

		//���� �ü��������� ȸ���� ����
		PitchAngle = Player->GetBaseAimRotation().GetNormalized().Pitch; //����ȭ (���ϸ� �� Ʀ, -180~180���̿��� ����� �� �ֵ��� ����ȭ)
		PitchAngle = FMath::Clamp(PitchAngle, -60,60) * -1;

		bHasPistol = Player->bHasPistol;
	}


}

void UNetPlayerAnimInstance::PlayerFireAnimation()
{	
	if (bHasPistol && FireMontage) {
		Montage_Play(FireMontage, 2);
	}
	
}
