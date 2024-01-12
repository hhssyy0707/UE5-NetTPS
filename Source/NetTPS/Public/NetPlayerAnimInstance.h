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

	//소유폰 저장변수
	UPROPERTY()
	class ANetTPSCharacter* Player;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "MyAnimSettings")
	class UAnimMontage* FireMontage;

	//총쏘기 재생함수
	void PlayerFireAnimation();

	// pitch 회전값(총이 시선방향으로 굽어질 수 있도록)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MyAnimSettings")
	float PitchAngle;
	

public:
	//재장전
	UPROPERTY(EditDefaultsOnly, Category = "MyAnimSettings")
	class UAnimMontage* ReloadMontage;
	void PlayReloadAnimation();

	//재장전 끝났을때 호출될 Modify 이벤트 함수
	UFUNCTION()
	void AnimNotify_OnReloadFinish();
		

	//죽음처리
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MyAnimSettings")
	bool IsDead = false;
	
};
