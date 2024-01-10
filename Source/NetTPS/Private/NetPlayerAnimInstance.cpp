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
		
		// 플레이어의 속도를 가져와서
		FVector Velocity = Player->GetVelocity();
		//Speed에 할당하고 싶다.
		Speed = FVector::DotProduct(Velocity, Player->GetActorForwardVector());
		//Direction에 할당하고 싶다.
		Direction = FVector::DotProduct(Velocity, Player->GetActorRightVector());

		//총을 시선방향으로 회전값 적용
		PitchAngle = Player->GetBaseAimRotation().GetNormalized().Pitch; //정규화 (안하면 팍 튐, -180~180사이에서 변경될 수 있도록 정규화)
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
