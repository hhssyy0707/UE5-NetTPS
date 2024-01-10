// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NetTPSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANetTPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	ANetTPSCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


public:
	//총붙일 컴포넌트
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* GunComp;

public:
	
	bool bHasPistol = false;

	// 잡을 수 있는 범위
	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	float MaxDistanceToGun = 200.0f;

	// 소유중인 총
	UPROPERTY()
	AActor* OwnedPistol = nullptr;


	
	//Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* TakePistolAction;
	//총을 컴포넌트로 붙이는 함수
	void AttachPistol(AActor* PistolActor);
	//총잡기 처리할 함수
	void TakePistol(const FInputActionValue& Value);

	//총놓기
	//Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ReleasePistolAction;
	void ReleasePistol(const FInputActionValue& Value);
	void DetachPistol(AActor* PistolActor);
	
	
	//총쏘기
	//Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FireAction;
	void Fire(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	class UParticleSystem* GunEffect;
	

	//메인 UI
public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainUI> MainUIWidget;
	
	UPROPERTY()
	class UMainUI* MainUI;

	void InitUIWidget();

	UPROPERTY(EditAnywhere, Category = "Bullet")
	int32 MaxBulletCount;

	int32 CurrentBulletCount = MaxBulletCount; // 초기화
};

