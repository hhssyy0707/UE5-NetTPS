// Fill out your copyright notice in the Description page of Project Settings.


#include "NetActor.h"

#include "NetTPS.h"
#include "EngineUtils.h"
#include "NetTPSCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ANetActor::ANetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// Owner가 설정돼있어야함
	//동기화 처리 해달라고 설정
	bReplicates = true; //-> 통신가능한 녀석이 됨

	//NetUpdateFrequency = 1.0f; // 1초에 1번만 동기화 , 뚝뚝 끊김
}

// Called when the game starts or when spawned
void ANetActor::BeginPlay()
{
	Super::BeginPlay();

	MatInstDymc = MeshComp->CreateDynamicMaterialInstance(0);
	
	//서버일때만 작동시키겠다
	if (HasAuthority()) {
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			MatColor = FLinearColor(FMath::RandRange(0,1),FMath::RandRange(0,1), FMath::RandRange(0, 1),1);

			//리슨서버라서 추가된 내용
			OnRep_MatColor(); // 클라단에만 적용되기때문에 서버에도 적용시키기 위해 억지로 넣어줌

		}),1,true);
	}
	
}

// Called every frame
void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintNetLog();
	FindOwner();

	//Yaw축을 기준으로 P = P0 + vt 회전속도 50
	if (HasAuthority()) {
		//서버일때만 이 작업을 해야함
		AddActorLocalRotation(FRotator(0,50*DeltaTime,0));
		RotYawValue = GetActorRotation().Yaw;
	}
	/* ///변경돼서 else로 들어오면 자동으로 밑에 함수 실행
	else {
		FRotator NewRot = GetActorRotation();
		NewRot.Yaw = RotYawValue;
		SetActorRotation(NewRot);
	}*/

	// Tick에서 계속 아래 함수를 호출하는 것은 문제다 -> OnRep_MatColor()로 빼기
	//if (MatInstDymc) {
	//	MatInstDymc->SetVectorParameterValue(TEXT("FloorColor"),MatColor);
	//}

}

//룩업테이블 변수이름에 해당하는 배열이있음 
void ANetActor::OnRep_RotYawValue()
{
	FRotator NewRot = GetActorRotation();
	NewRot.Yaw = RotYawValue;
	SetActorRotation(NewRot);
}

//서버에서 값을 바꾸면 client에서 바꾼다. -> 서버에서는 실행이 안됨(적용안됨)
void ANetActor::OnRep_MatColor()
{	
	if (MatInstDymc) {
	
		MatInstDymc->SetVectorParameterValue(TEXT("FloorColor"),MatColor);
	
	}
}


void ANetActor::PrintNetLog()
{
	const FString ConnStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	const FString OwnerStr = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");

	const FString LogStr = FString::Printf(TEXT("Connection : %s \nOwner : %s\nLocal Role : %s, Remote Role : %s"), *ConnStr, *OwnerStr, *LOCALROLE, *REMOTEROLE);

	DrawDebugString(GetWorld(), GetActorLocation(), LogStr, nullptr, FColor::Purple, 0, true, 1);
}

void ANetActor::FindOwner()
{

	//Owner 설정 : 영역안의 가장 가까운 캐릭터를 찾아서 Owner로 설정
	//if(GetLocalRole() == ROLE_Authority)
	if (HasAuthority()) {
		//저장할 새 Owner 변수
		AActor* NewOwner = nullptr;
		float MinDist = SearchDistance;

		for (TActorIterator<ANetTPSCharacter> it(GetWorld()); it; ++it) {
			AActor* OtherActor = *it;
			float Dist = GetDistanceTo(OtherActor);

			// 이전에 기록하고 있는거리보다 작다면
			if (Dist < MinDist) {
				//거리 업데이트
				MinDist = Dist;
				// Owner 설정
				NewOwner = OtherActor;
			}
		}

		//오너 설정
		//기존 오너하고 다른녀석이면 오너 업데이트
		if (GetOwner() != NewOwner) {
			SetOwner(NewOwner);
		}
	}


	//영역 시각화
	DrawDebugSphere(GetWorld(), GetActorLocation(), SearchDistance, 30, FColor::Yellow, false, 0, 0, 1);
}


void ANetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 동기화 해줄 변수는 여기에 반드시 추가해줄 것
	DOREPLIFETIME(ANetActor, RotYawValue);
	DOREPLIFETIME(ANetActor, MatColor);
}