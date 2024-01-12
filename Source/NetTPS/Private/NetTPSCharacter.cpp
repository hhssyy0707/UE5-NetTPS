// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "Kismet/GameplayStatics.h"
#include "NetPlayerAnimInstance.h"
#include "MainUI.h"
#include "Components/WidgetComponent.h"
#include "HealthBar.h"
#include "NetTPS.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ANetTPSCharacter

ANetTPSCharacter::ANetTPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->bUseControllerDesiredRotation = true; //2040108 �����߰�
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->TargetArmLength = 150.0f;;
	CameraBoom->SetRelativeLocation(FVector(0,40,60));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	GunComp = CreateDefaultSubobject<USceneComponent>(TEXT("GunComp"));
	GunComp->SetupAttachment(GetMesh(), TEXT("GunPosition")); // Socket�̸�
	GunComp->SetRelativeLocation(FVector((-7.617589f, 0.958795f, 4.504652f)));
	GunComp->SetRelativeRotation(FRotator((10.0f, 90.0f, 10.0f)));

	HPUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HPUIComp->SetupAttachment(GetMesh());

}

void ANetTPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}


	InitUIWidget();
}

void ANetTPSCharacter::TakePistol(const FInputActionValue& Value)
{
	//������ ���� �ȿ� �ִ� ���� ��� �ʹ�.

	//���� �����ϰ� ���� ������ ���� ���� �ȿ� �ִ� ���� ��´�.
	// 1. ���� �����ϰ� ���� ���� ��
	// 2. ���� ������� �˾ƾ��Ѵ�.
	// 3. �Ѱ��� �Ÿ��� ���ؾ��Ѵ�. 
	// 4. ���� ���� �ȿ� ���� ��
	// 5. ���� �����ϰ� �ʹ�.
	// 6. ���� ��� �ʹ�.
	// -> �ʿ� �Ӽ� : �� ���� ����, ���� �� �ִ� ����(��ȹ ��Ʈ���� ���ǵǾ���� ����)


	// 1. ���� �����ϰ� ���� ���� ��
	// 1-1. �����ϸ� �ƹ��͵� ���� �ʴ´�.
	if (bHasPistol == true)
	{
		return;
	}

	// 2. ���� ������� �˾ƾ��Ѵ�.(���忡�� ��� ��������)
	TArray<AActor*> AllGunActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllGunActors);
	// 2-1. ������ �˻��Ѵ�.
	//for (int32 i = 0; i < AllGunActors.Num(); i++) {
	//	if (AllGunActors[i]->GetActorNameOrLabel().Contains("BP_Pistol") == false) {
	//		continue;
	//	}
	//}

	for (auto TempPistol : AllGunActors) {
		if (TempPistol->GetName().Contains("BP_Pistol") == false) {
			continue;
		}


		// 3. �Ѱ��� �Ÿ�
		float DistanceToGun = FVector::Dist(GetActorLocation(), TempPistol->GetActorLocation());

		// 4. ���� ���� �ۿ� ���� ��
		if (DistanceToGun > MaxDistanceToGun) {
			continue;
		}

		// 5. ���� �����ϰ� �ʹ�.
		bHasPistol = true;
		OwnedPistol = TempPistol;
		OwnedPistol->SetOwner(this); // ��� attach�ϸ� �����൵��...
		
		// 6. ���� ��� �ʹ�.
		AttachPistol(OwnedPistol);
		//AttachPistol(TempPistol);

		// 7. ���� ������� Loop ����
		break;
	}
}

void ANetTPSCharacter::AttachPistol(AActor* PistolActor)
{
	auto MeshComp = PistolActor->GetComponentByClass<UStaticMeshComponent>();
	MeshComp->SetSimulatePhysics(false);
	MeshComp-> AttachToComponent(GunComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	// UI ����
	//��Ƽ�� �ٲ� ����
	if (MainUI) {// ��Ƽ
		MainUI->ShowCrosshair(true);
	}
}

void ANetTPSCharacter::Fire(const FInputActionValue& Value)
{
	//���� �����ϰ� ���� �ʴٸ� ó������ �ʴ´�.
	if (bHasPistol == false || IsReloading) {
		return;
	}

	// �Ѿ� ������ return
	if (CurrentBulletCount <= 0) {
		return;
	}

	// �ѽ��
	FHitResult HitInfo;
	FVector StartPos = FollowCamera->GetComponentLocation();
	FVector EndPos = StartPos + FollowCamera->GetForwardVector()*10000.0f; 
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartPos, EndPos, ECC_Visibility, Params);

	if (bHit) { // �浹�ϸ�
		//���� �ڸ��� ��ƼŬ ȿ�� ���
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunEffect, HitInfo.Location, FRotator());

		//���� ����� ������ ��� ������ ó��
		auto OtherPlayer = Cast<ANetTPSCharacter>(HitInfo.GetActor());

		if (OtherPlayer) {
			OtherPlayer->DamageProcess();
		}
	}

	//�ѽ�� �ִϸ��̼� ���
	auto Anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	Anim->PlayerFireAnimation();

#pragma region AutoReload

	// �Ѿ� �Ҹ�
	//CurrentBulletCount--;
	--CurrentBulletCount;
	MainUI->PopBullet(CurrentBulletCount);

	if (CurrentBulletCount <= 0) {
		//MainUI->ReloadBullet();

		FTimerHandle ReloadBulletTimer;

		GetWorld()->GetTimerManager().SetTimer(ReloadBulletTimer, FTimerDelegate::CreateLambda([this]()->void {

			// �Ѿ� �߰�
			for (int i = 0; i < MaxBulletCount; ++i) {
				MainUI->AddBullet();
			}
			CurrentBulletCount = MaxBulletCount;

			}), 2.0f, false);
	}
#pragma endregion

}

void ANetTPSCharacter::InitUIWidget()
{	

	//Player �ƴϸ� ó������ �ʵ��� ����
	auto PC = Cast<APlayerController>(Controller);
	if (PC == nullptr) {
		return;
	}


	if (MainUIWidget) {
		MainUI = Cast<UMainUI>(CreateWidget(GetWorld(), MainUIWidget));
		MainUI->AddToViewport();
		MainUI->ShowCrosshair(false);

		// �Ѿ� UI ����
		CurrentBulletCount = MaxBulletCount;

		// �Ѿ� �߰�
		for (int i = 0; i < MaxBulletCount; ++i) {
			MainUI->AddBullet();
		}
	}
}

void ANetTPSCharacter::ReloadPistol(const FInputActionValue& Value)
{
	if(bHasPistol == false || IsReloading == true){
		return;
	}

	auto AnimInst = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInst->PlayReloadAnimation();

	IsReloading = true;
}

void ANetTPSCharacter::InitBulletUI()
{
	
	MainUI->RemoveAllBullets();

	// �Ѿ� ���� �ʱ�ȭ
	CurrentBulletCount = MaxBulletCount;

	for (int i = 0; i < MaxBulletCount; i++) {
		MainUI->AddBullet();
	}

	IsReloading = false;

}

float ANetTPSCharacter::GetHP()
{
	return CurrentHP;
}

void ANetTPSCharacter::SetHP(float Value)
{
	CurrentHP = Value;
	float HPPercent = CurrentHP / MaxHP;
	
	if (MainUI) {
		MainUI->PB_HPValue = HPPercent;
	}
	else {
		auto HPUI = Cast<UHealthBar>(HPUIComp->GetWidget());
		HPUI->PB_HPValue = HPPercent;
	}
}

void ANetTPSCharacter::DamageProcess()
{
	HP--;

	// ����ó��
	if (HP <= 0) {
		IsDead = true;
	}
}

void ANetTPSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	PrintNetLog();
}

void ANetTPSCharacter::PrintNetLog()
{
	const FString ConnStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	const FString OwnerStr = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");

	const FString LogStr = FString::Printf(TEXT("Connection : %s \nOwner : %s\nLocal Role : %s, Remote Role : %s"), *ConnStr,*OwnerStr,*LOCALROLE, *REMOTEROLE);

	DrawDebugString(GetWorld(), GetActorLocation(), LogStr, nullptr, FColor::Purple,0,true, 1);
}

void ANetTPSCharacter::DetachPistol(AActor* PistolActor)
{
	auto MeshComp = PistolActor->GetComponentByClass<UStaticMeshComponent>();
	MeshComp -> SetSimulatePhysics(true);
	MeshComp -> DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);


	if (MainUI) {// ��Ƽ

		MainUI->ShowCrosshair(false);
	}
}

void ANetTPSCharacter::ReleasePistol(const FInputActionValue& Value)
{
	if (bHasPistol == false || IsReloading) {
		return;
	}

	//����ڵ�
	if (OwnedPistol) {

		DetachPistol(OwnedPistol);

		bHasPistol = false;
		OwnedPistol->SetOwner(nullptr);
		OwnedPistol = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANetTPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Look);

		// �����
		EnhancedInputComponent->BindAction(TakePistolAction, ETriggerEvent::Started, this, &ANetTPSCharacter::TakePistol);

		// �ѳ���
		EnhancedInputComponent->BindAction(ReleasePistolAction, ETriggerEvent::Started, this, &ANetTPSCharacter::ReleasePistol);
		
		// �ѽ��
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ANetTPSCharacter::Fire);
		
		// ������
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ANetTPSCharacter::ReloadPistol);


	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetTPSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetTPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
