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
	//�Ѻ��� ������Ʈ
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* GunComp;

public:
	
	//240115 ��Ʈ��ũ �߰�
	UPROPERTY(Replicated)
	bool bHasPistol = false;

	// ���� �� �ִ� ����
	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	float MaxDistanceToGun = 200.0f;

	// �������� ��
	UPROPERTY()
	AActor* OwnedPistol = nullptr;


	
	//Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* TakePistolAction;
	//���� ������Ʈ�� ���̴� �Լ�
	void AttachPistol(AActor* PistolActor);
	//����� ó���� �Լ�
	void TakePistol(const FInputActionValue& Value);

	//�ѳ���
	//Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ReleasePistolAction;

	void ReleasePistol(const FInputActionValue& Value);
	void DetachPistol(AActor* PistolActor);
	
	
	//�ѽ��
	//Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FireAction;
	void Fire(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = "Pistol")
	class UParticleSystem* GunEffect;
	

	//���� UI
public:
	/*UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainUI> MainUIWidget;*/

	UPROPERTY()
	class UMainUI* MainUI;

	void InitUIWidget();

	//Ŭ�󿡼��� ȣ��
	// �������� �ִ� �����͸� Ŭ�� �ʱ�ȭ(����ȭ ����)
	virtual void PostNetInit() override;
	//SetupPlayerInputComponent �װź��� ���� ����Ǵ� ����������Ŭ �����
	// ȣ�� ������ �� �˾ƾ� ����, �ʱ�ȭ, �������� �� ���� �� �ִ�.
	

	UPROPERTY(EditAnywhere, Category = "Bullet")
	int32 MaxBulletCount = 10;
	
	//UPROPERTY(ReplicatedUsing = OnRep_CurrentBulletCount)
	//int32 CurrentBulletCount = MaxBulletCount; // �ʱ�ȭ
	int32 CurrentBulletCount; // �ʱ�ȭ
	
	//FUNCTION()
	//oid OnRep_CurrentBulletCount();

	//������
	//Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ReloadAction;
	void ReloadPistol(const FInputActionValue& Value);

	//�Ѿ� UI ����
	void InitBulletUI();

	// ���� ���������� 
	bool IsReloading = false;

public:
	// �÷��̾� ü��
	UPROPERTY(EditDefaultsOnly, Category = "PlayerHP")
	float MaxHP = 10;

	//UPROPERTY(EditDefaultsOnly, Category = "PlayerHP")
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP)
	float CurrentHP = MaxHP;

	UFUNCTION()
	void OnRep_CurrentHP();

	//�𸮾󿡼��� ������ �ڵ�
	__declspec(property(get=GetHP, put = SetHP))
	float HP;
	float GetHP();
	void SetHP(float Value);

	UPROPERTY(VisibleAnywhere, Category = "PlayerHP")
	class UWidgetComponent* HPUIComp;
	//class UUserWidget* HPUIComp;

	//�ǰ�ó�� �Լ�
	void DamageProcess();

	//vlrurtl ī�޶� ����ũ
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UCameraShakeBase> DamageCameraShake;

	//���� ó��
public:
	//UPROPERTY(Replicated)
	bool IsDead = false;

	void DieProcess();

public:
	virtual void Tick( float DeltaSeconds ) override;
	//��Ʈ��ũ �α���� �Լ�
	void PrintNetLog();

	//============================MultiPlayer ��ҵ�======================
public:
	//�� ����
	UFUNCTION(Server , Reliable)
	void ServerRPC_TakePistol();
	
	UFUNCTION(NetMulticast , Reliable)
	void MultiRPC_TakePistol(AActor* PistolActor); //� ���� ����� ��Ʈ��ũ ������ ����(NetID)
	
	
	//�� ����
	UFUNCTION(Server , Reliable)
	void ServerRPC_ReleasePistol();
	
	UFUNCTION(NetMulticast , Reliable)
	void MultiRPC_ReleasePistol(AActor* PistolActor); 

	//�� ���
	UFUNCTION(Server , Reliable)
	void ServerRPC_Fire();
	
	UFUNCTION(NetMulticast , Reliable)
	void MultiRPC_Fire(const FHitResult& HitInfo, bool bHit, const int NewBulletCount);

	
	//�� ������
	UFUNCTION(Server , Reliable)
	void ServerRPC_Reload();
	
	UFUNCTION(Client , Reliable)
	void ClientRPC_Reload();

};

