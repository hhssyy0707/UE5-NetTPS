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
	
		
};
