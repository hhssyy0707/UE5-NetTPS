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

	virtual void PossessedBy(AController* NewController) override;
	
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
	
	//240115 네트워크 추가
	UPROPERTY(Replicated)
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
	/*UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainUI> MainUIWidget;*/

	UPROPERTY()
	class UMainUI* MainUI;

	void InitUIWidget();

	//클라에서만 호출
	// 서버에만 있는 데이터를 클라에 초기화(동기화 역할)
	virtual void PostNetInit() override;
	//SetupPlayerInputComponent 그거보다 빨리 실행되는 라이프사이클 펑션임
	// 호출 순서를 잘 알아야 세팅, 초기화, 설정값을 잘 해줄 수 있다.
	

	UPROPERTY(EditAnywhere, Category = "Bullet")
	int32 MaxBulletCount = 10;
	
	//UPROPERTY(ReplicatedUsing = OnRep_CurrentBulletCount)
	//int32 CurrentBulletCount = MaxBulletCount; // 초기화
	int32 CurrentBulletCount; // 초기화
	
	//FUNCTION()
	//oid OnRep_CurrentBulletCount();

	//재장전
	//Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ReloadAction;
	void ReloadPistol(const FInputActionValue& Value);

	//총알 UI 리셋
	void InitBulletUI();

	// 지금 장전중인지 
	bool IsReloading = false;

public:
	// 플레이어 체력
	UPROPERTY(EditDefaultsOnly, Category = "PlayerHP")
	float MaxHP = 10;

	//UPROPERTY(EditDefaultsOnly, Category = "PlayerHP")
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP)
	float CurrentHP = MaxHP;

	UFUNCTION()
	void OnRep_CurrentHP();

	//언리얼에서만 가능한 코드
	__declspec(property(get=GetHP, put = SetHP))
	float HP;
	float GetHP();
	void SetHP(float Value);

	UPROPERTY(VisibleAnywhere, Category = "PlayerHP")
	class UWidgetComponent* HPUIComp;
	//class UUserWidget* HPUIComp;

	//피격처리 함수
	void DamageProcess();

	//vlrurtl 카메라 쉐이크
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UCameraShakeBase> DamageCameraShake;

	//죽음 처리
public:
	//UPROPERTY(Replicated)
	bool IsDead = false;

	void DieProcess();

public:
	virtual void Tick( float DeltaSeconds ) override;
	//네트워크 로그찍는 함수
	void PrintNetLog();

	//============================MultiPlayer 요소들======================
public:
	//총 집기
	UFUNCTION(Server , Reliable)
	void ServerRPC_TakePistol();
	
	UFUNCTION(NetMulticast , Reliable)
	void MultiRPC_TakePistol(AActor* PistolActor); //어떤 총을 잡는지 네트워크 상으로 날림(NetID)
	
	
	//총 놓기
	UFUNCTION(Server , Reliable)
	void ServerRPC_ReleasePistol();
	
	UFUNCTION(NetMulticast , Reliable)
	void MultiRPC_ReleasePistol(AActor* PistolActor); 

	//총 쏘기
	UFUNCTION(Server , Reliable)
	void ServerRPC_Fire();
	
	UFUNCTION(NetMulticast , Reliable)
	void MultiRPC_Fire(const FHitResult& HitInfo, bool bHit, const int NewBulletCount);

	
	//총 재장전
	UFUNCTION(Server , Reliable)
	void ServerRPC_Reload();
	
	UFUNCTION(Client , Reliable)
	void ClientRPC_Reload();

};

