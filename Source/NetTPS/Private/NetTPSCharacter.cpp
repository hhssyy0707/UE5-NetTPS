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
#include "Net/UnrealNetwork.h"// DOREPLIFETIME 
#include "Components/HorizontalBox.h"//HorizontalBox
#include "NetPlayerController.h"

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
	GetCharacterMovement()->bUseControllerDesiredRotation = true; //2040108 수업추가
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
	GunComp->SetupAttachment(GetMesh(), TEXT("GunPosition")); // Socket이름
	GunComp->SetRelativeLocation(FVector((-7.617589f, 0.958795f, 4.504652f)));
	GunComp->SetRelativeRotation(FRotator((10.0f, 90.0f, 10.0f)));

	HPUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HPUIComp->SetupAttachment(GetMesh());

	//액터 리플리케이션 사용하겠다.
	bReplicates = true;
	SetReplicates(true);
}

void ANetTPSCharacter::BeginPlay()
{
	PRINTLOG(TEXT("ANetTPSCharacter::BeginPlay() Begin, Controller : %s"), Controller? TEXT("Valid"): TEXT("Invalid"));
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

	CurrentBulletCount = MaxBulletCount;

	// Client 창에서 실행되는 코드여야 한다.
	//내가 제어중이고 , Client인지 확인 (MainUI 세팅하기 위해)
	if (IsLocallyControlled() && HasAuthority() == false) {
		InitUIWidget();
	}

	//UI 머리위에 다시 뜨는 이유 Controller 못찾아서 
	// -> PossessedBy에서 UI 초기화를 해줘야한다.
	//BeginPlay는 서버, 클라 둘다 실행됨


	//PRINTLOG(TEXT("ANetTPSCharacter::BeginPlay() End"));
	PRINTLOG(TEXT("End, Controller : %s"), Controller ? TEXT("Valid") : TEXT("Invalid"));
}

void ANetTPSCharacter::TakePistol(const FInputActionValue& Value)
{
	//지정된 영역 안에 있는 총을 잡고 싶다.

	//총을 소유하고 있지 않으면 일정 범위 안에 있는 총을 잡는다.
	// 1. 총을 소유하고 있지 않을 때
	// 2. 총이 어딨는지 알아야한다.
	// 3. 총과의 거리를 구해야한다. 
	// 4. 일정 범위 안에 있을 때
	// 5. 총을 소유하고 싶다.
	// 6. 총을 잡고 싶다.
	// -> 필요 속성 : 총 소유 여부, 잡을 수 있는 범위(기획 파트에서 정의되어야할 문제)


	// 1. 총을 소유하고 있지 않을 때
	// 1-1. 소유하면 아무것도 하지 않는다.
	if (bHasPistol == true || IsDead)
	{
		return;
	}

	//대체
	ServerRPC_TakePistol();

	//// 2. 총이 어딨는지 알아야한다.(월드에서 모두 가져오기)
	//TArray<AActor*> AllGunActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllGunActors);
	//// 2-1. 총인지 검사한다.
	////for (int32 i = 0; i < AllGunActors.Num(); i++) {
	////	if (AllGunActors[i]->GetActorNameOrLabel().Contains("BP_Pistol") == false) {
	////		continue;
	////	}
	////}

	//for (auto TempPistol : AllGunActors) {
	//	if (TempPistol->GetName().Contains("BP_Pistol") == false) {
	//		continue;
	//	}


	//	// 3. 총과의 거리
	//	float DistanceToGun = FVector::Dist(GetActorLocation(), TempPistol->GetActorLocation());

	//	// 4. 일정 범위 밖에 있을 때
	//	if (DistanceToGun > MaxDistanceToGun) {
	//		continue;
	//	}

	//	// 5. 총을 소유하고 싶다.
	//	bHasPistol = true;
	//	OwnedPistol = TempPistol;
	//	OwnedPistol->SetOwner(this); // 사실 attach하면 안해줘도됨...
	//	
	//	// 6. 총을 잡고 싶다.
	//	AttachPistol(OwnedPistol);
	//	//AttachPistol(TempPistol);

	//	// 7. 총을 잡았으니 Loop 종료
	//	break;
	//}
}

void ANetTPSCharacter::AttachPistol(AActor* PistolActor)
{
	auto MeshComp = PistolActor->GetComponentByClass<UStaticMeshComponent>();
	MeshComp->SetSimulatePhysics(false);
	MeshComp-> AttachToComponent(GunComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	// UI 변경
	//멀티때 바꿀 예정
	if (MainUI) {// 멀티
		MainUI->ShowCrosshair(true);
	}
}

void ANetTPSCharacter::Fire(const FInputActionValue& Value)
{
	//총을 소유하고 있지 않다면 처리하지 않는다.
	if (bHasPistol == false || CurrentBulletCount <= 0 || IsReloading || IsDead) {
		return;
	}

	// 총알 없으면 return
	if (CurrentBulletCount <= 0) {
		return;
	}

	ServerRPC_Fire();

	//// 총쏘기
	//FHitResult HitInfo;
	//FVector StartPos = FollowCamera->GetComponentLocation();
	//FVector EndPos = StartPos + FollowCamera->GetForwardVector()*10000.0f; 
	//FCollisionQueryParams Params;
	//Params.AddIgnoredActor(this);

	//bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartPos, EndPos, ECC_Visibility, Params);
	
	//if (bHit) { // 충돌하면
	//	//맞은 자리에 파티클 효과 재생
	//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunEffect, HitInfo.Location, FRotator());

	//	//맞은 대상이 상대방일 경우 데미지 처리
	//	auto OtherPlayer = Cast<ANetTPSCharacter>(HitInfo.GetActor());

	//	if (OtherPlayer) {
	//		OtherPlayer->DamageProcess();
	//	}
	//}

	//	// 총알 소모
	//	//CurrentBulletCount--;
	//	--CurrentBulletCount;
	//	MainUI->PopBullet(CurrentBulletCount);
	
	////총쏘는 애니메이션 재생
	//auto Anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	//Anim->PlayerFireAnimation();
	
//#pragma region AutoReload
//
//	if (CurrentBulletCount <= 0) {
//		//MainUI->ReloadBullet();
//
//		FTimerHandle ReloadBulletTimer;
//
//		GetWorld()->GetTimerManager().SetTimer(ReloadBulletTimer, FTimerDelegate::CreateLambda([this]()->void {
//
//			// 총알 추가
//			for (int i = 0; i < MaxBulletCount; ++i) {
//				MainUI->AddBullet();
//			}
//			CurrentBulletCount = MaxBulletCount;
//
//			}), 2.0f, false);
//	}
//#pragma endregion

}

void ANetTPSCharacter::InitUIWidget()
{	

	//Player 아니면 처리하지 않도록 하자
	auto PC = Cast<ANetPlayerController>(Controller);
	if (PC == nullptr) {
		return;
	}


	if (PC->MainUIWidget) {
		//MainUI가 없을 때만 해주자
		if (PC->MainUI == nullptr) {

			PC->MainUI = Cast<UMainUI>(CreateWidget(GetWorld(), PC->MainUIWidget));
			
			PC->MainUI->AddToViewport();
		}
		MainUI = PC->MainUI;
		MainUI->ShowCrosshair(false);

		// UI 초기화하기전에 총알 모두 제거하기
		MainUI->RemoveAllBullets();

		// 총알 UI 세팅
		CurrentBulletCount = MaxBulletCount;

		// 총알 추가
		for (int i = 0; i < MaxBulletCount; ++i) {
			MainUI->AddBullet();
		}

		if (HPUIComp) {
			HPUIComp->SetVisibility(false);
		}
	}
}

//void ANetTPSCharacter::OnRep_CurrentBulletCount()
//{
//
//	UE_LOG(LogTemp, Warning, TEXT("OnRep_CurrentBulletCount After : %d/%d"), CurrentBulletCount, MaxBulletCount);
//	if (GetLocalRole() == ROLE_AutonomousProxy)
//	if (MainUI) {
//		MainUI->PopBullet(CurrentBulletCount);
//	}
//}

void ANetTPSCharacter::ReloadPistol(const FInputActionValue& Value)
{
	if(bHasPistol == false || IsReloading == true){
		return;
	}

	//애니메이션 클라에서 발생 -> 굳이 서버로 옮길 필요없음
	auto AnimInst = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInst->PlayReloadAnimation();

	IsReloading = true;
}

void ANetTPSCharacter::InitBulletUI()
{

	ServerRPC_Reload();

	//MainUI->RemoveAllBullets();

	//UE_LOG(LogTemp, Warning,TEXT("Before : %d/%d"), CurrentBulletCount, MaxBulletCount);
	//// 총알 개수 초기화
	//CurrentBulletCount = MaxBulletCount;

	//UE_LOG(LogTemp, Warning, TEXT("After : %d/%d"), CurrentBulletCount, MaxBulletCount);
	//for (int i = 0; i < MaxBulletCount; i++) {
	//	MainUI->AddBullet();
	//}

	//IsReloading = false;

}

void ANetTPSCharacter::OnRep_CurrentHP()
{
	// 죽음처리
	if (HP <= 0) {
		IsDead = true;

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();
		//죽었으니까 총은 놓자
		ReleasePistol(FInputActionValue());
	}

	float HPPercent = CurrentHP / MaxHP;

	if (MainUI) { // 메인 플레이어 일때만
	//if(IsLocallyControlled()){
		MainUI->PB_HPValue = HPPercent;

		// 피격 시 애니메이션 및 이펙트
		MainUI->PlayDamageAnimation();
		if (DamageCameraShake) {
			auto PC = Cast<APlayerController>(Controller);
			PC->ClientStartCameraShake(DamageCameraShake);
		}

	}
	else {
		auto HPUI = Cast<UHealthBar>(HPUIComp->GetWidget());
		HPUI->PB_HPValue = HPPercent;
	}
}

float ANetTPSCharacter::GetHP()
{
	return CurrentHP;
}

void ANetTPSCharacter::SetHP(float Value)
{
	CurrentHP = Value;
	
	//Tick으로 이동 -> Onrep_HP로 이동
	/*
	float HPPercent = CurrentHP / MaxHP;
	
	if (MainUI) {
		MainUI->PB_HPValue = HPPercent;
	}
	else {
		auto HPUI = Cast<UHealthBar>(HPUIComp->GetWidget());
		HPUI->PB_HPValue = HPPercent;
	}*/

	//240116
	//OnRep은 서버X 클라에서만 호출됨
	// 서버 HP가 줄어들지 않아 여기서 강제 OnRep 실행해주기
	OnRep_CurrentHP();
}

void ANetTPSCharacter::DamageProcess()
{
	HP--;

	// OnRep_CurrentHP로 이동
	//// 죽음처리
	//if (HP <= 0) {
	//	IsDead = true;
	//}
}

void ANetTPSCharacter::DieProcess()
{
	//애니메이션 끝난 후 호출예정
	//클라이언트에게만 보이는 시각적 효과 -> OnRep_CurrentHP
	//BP_ThirdPerson에서 Saturation 켜줌
	GetFollowCamera()->PostProcessSettings.ColorSaturation = FVector4(0,0,0,1);

	auto PC = Cast<APlayerController>(Controller);
	PC->SetShowMouseCursor(true);

	//DieUI표시
	MainUI->HB_GameOverUI->SetVisibility(ESlateVisibility::Visible);
}

void ANetTPSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PrintNetLog();

	//OnRep_RCurrentHP()로 이동
	/*float HPPercent = CurrentHP / MaxHP;

	if (MainUI) {
		MainUI->PB_HPValue = HPPercent;
	}
	else {
		auto HPUI = Cast<UHealthBar>(HPUIComp->GetWidget());
		HPUI->PB_HPValue = HPPercent;
	}*/


	//HP UI Billboarding 처리
	//빌보딩 : 항상 앞으로 보이게 하는 것
	if (HPUIComp && HPUIComp->GetVisibleFlag()) {

		// 플레이어의 카메라 위치 정보를 가져온다
		FVector CamLoc = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->GetCameraLocation();
		//다른방법 : GetWorld()->GetFirstPlayerController()->GetPawn() Cast 후 

		FVector Direction = CamLoc - HPUIComp->GetComponentLocation(); //월드좌표 기준으로 가져온다. 컴포넌트 로케이션 사용 이유
		Direction.Z = 0;

		HPUIComp->SetWorldRotation(Direction.GetSafeNormal().ToOrientationRotator());
	}

}

void ANetTPSCharacter::PrintNetLog()
{
	const FString ConnStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	const FString OwnerStr = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");

	const FString LogStr = FString::Printf(TEXT("Connection : %s \nOwner : %s\nLocal Role : %s, Remote Role : %s\n CurrentBulletCount : %d/%d,  "), *ConnStr,*OwnerStr,*LOCALROLE, *REMOTEROLE, CurrentBulletCount, MaxBulletCount);

	DrawDebugString(GetWorld(), GetActorLocation(), LogStr, nullptr, FColor::Purple,0,true, 1);
}

void ANetTPSCharacter::ServerRPC_TakePistol_Implementation()
{
	// 2. 총이 어딨는지 알아야한다.(월드에서 모두 가져오기)
	TArray<AActor*> AllGunActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllGunActors);
	// 2-1. 총인지 검사한다.
	//for (int32 i = 0; i < AllGunActors.Num(); i++) {
	//	if (AllGunActors[i]->GetActorNameOrLabel().Contains("BP_Pistol") == false) {
	//		continue;
	//	}
	//}

	for (auto TempPistol : AllGunActors) {
		if (TempPistol->GetName().Contains("BP_Pistol") == false) {
			continue;
		}


		// 3. 총과의 거리
		float DistanceToGun = FVector::Dist(GetActorLocation(), TempPistol->GetActorLocation());

		// 4. 일정 범위 밖에 있을 때
		if (DistanceToGun > MaxDistanceToGun) {
			continue;
		}

		// 5. 총을 소유하고 싶다.
		bHasPistol = true;
		OwnedPistol = TempPistol;
		OwnedPistol->SetOwner(this); // 사실 attach하면 안해줘도됨...
		

		//// 6. 총을 잡고 싶다.
		//AttachPistol(OwnedPistol);
		////AttachPistol(TempPistol);
		//-> MultiRPC_TakePistol_Implementation로 이동
		// 모든 클라이언트한테 전송
		MultiRPC_TakePistol(OwnedPistol);
		//AttachPistol(TempPistol);

		// 7. 총을 잡았으니 Loop 종료
		break;
	}
}

void ANetTPSCharacter::MultiRPC_TakePistol_Implementation(AActor* PistolActor)
{
	AttachPistol(PistolActor);
}

void ANetTPSCharacter::ServerRPC_ReleasePistol_Implementation()
{
	//OwnedPistol ->PistolActor
	//방어코드
	if (OwnedPistol) {

		MultiRPC_ReleasePistol(OwnedPistol);

		bHasPistol = false;
		OwnedPistol->SetOwner(nullptr);
		OwnedPistol = nullptr;
	}
}

void ANetTPSCharacter::MultiRPC_ReleasePistol_Implementation(AActor* PistolActor)
{
	DetachPistol(PistolActor);
}

void ANetTPSCharacter::ServerRPC_Fire_Implementation()
{
	FHitResult HitInfo;
	FVector StartPos = FollowCamera->GetComponentLocation();
	FVector EndPos = StartPos + FollowCamera->GetForwardVector() * 10000.0f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartPos, EndPos, ECC_Visibility, Params);

	if (bHit) {
		
		//맞은 대상이 상대방일 경우 데미지 처리
		auto OtherPlayer = Cast<ANetTPSCharacter>(HitInfo.GetActor());

		if (OtherPlayer) {
			OtherPlayer->DamageProcess();
		}
	}
	// 총알 소모
	//CurrentBulletCount--;
	--CurrentBulletCount;

	MultiRPC_Fire(HitInfo, bHit, CurrentBulletCount);

}

void ANetTPSCharacter::MultiRPC_Fire_Implementation(const FHitResult& HitInfo, bool bHit, const int NewBulletCount)
{
	// OnRep에서는 문제없었을것
	// 클라에서 따로 바뀐 총알수를 동기화해줘야한다.
	CurrentBulletCount = NewBulletCount;

	// 총쏘기
	if (bHit) { // 충돌하면
		//맞은 자리에 파티클 효과 재생
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunEffect, HitInfo.Location, FRotator());

	}

	//방법 2가지 
	//1. Onrep_CurrentBulletCount사용
	//2. MultiRPC_Fire에서 UI만 업데이트
	if (MainUI) {
		MainUI->PopBullet(NewBulletCount);
	}

	//총쏘는 애니메이션 재생
	auto Anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	Anim->PlayerFireAnimation();

}

void ANetTPSCharacter::ServerRPC_Reload_Implementation()
{
	//총알 개수 동기화
	CurrentBulletCount = MaxBulletCount;
	//Client RPC Call
	ClientRPC_Reload();
}

void ANetTPSCharacter::ClientRPC_Reload_Implementation()
{
	// 총알 개수 초기화
	CurrentBulletCount = MaxBulletCount;
	if (MainUI) {

		MainUI->RemoveAllBullets();
		for (int i = 0; i < MaxBulletCount; i++) {
			MainUI->AddBullet();
		}
	}

	IsReloading = false;
}

void ANetTPSCharacter::DetachPistol(AActor* PistolActor)
{
	auto MeshComp = PistolActor->GetComponentByClass<UStaticMeshComponent>();
	MeshComp -> SetSimulatePhysics(true);
	MeshComp -> DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);


	if (MainUI) {// 멀티용 방어코드

		MainUI->ShowCrosshair(false);
	}
}

void ANetTPSCharacter::ReleasePistol(const FInputActionValue& Value)
{
	if (bHasPistol == false || IsReloading || IsLocallyControlled() == false) {
		return;
	}

	//ServerRPC_ReleasePistol_Implementation()로 이동
	ServerRPC_ReleasePistol();

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

		// 총잡기
		EnhancedInputComponent->BindAction(TakePistolAction, ETriggerEvent::Started, this, &ANetTPSCharacter::TakePistol);

		// 총놓기
		EnhancedInputComponent->BindAction(ReleasePistolAction, ETriggerEvent::Started, this, &ANetTPSCharacter::ReleasePistol);
		
		// 총쏘기
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ANetTPSCharacter::Fire);
		
		// 재장전
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ANetTPSCharacter::ReloadPistol);


	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetTPSCharacter::PossessedBy(AController* NewController)
{
	//PRINTLOG(TEXT("Begin"));
	PRINTLOG(TEXT("Begin, Controller : %s"), Controller ? TEXT("Valid") : TEXT("Invalid"));
	Super::PossessedBy(NewController);
	//PRINTLOG(TEXT("End"));


	//내가 제어 중인지 (메인 플레이어인지) 체크
	if (IsLocallyControlled()) {
		
		// UI 초기화
		InitUIWidget();
	}

	PRINTLOG(TEXT("End, Controller : %s"), Controller ? TEXT("Valid") : TEXT("Invalid"));
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

void ANetTPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetTPSCharacter, bHasPistol);
	//DOREPLIFETIME(ANetTPSCharacter, CurrentBulletCount);
	DOREPLIFETIME(ANetTPSCharacter, CurrentHP);
	//DOREPLIFETIME(ANetTPSCharacter, IsDead);
	

}