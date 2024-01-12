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

	// Owner�� �������־����
	//����ȭ ó�� �ش޶�� ����
	bReplicates = true; //-> ��Ű����� �༮�� ��

	//NetUpdateFrequency = 1.0f; // 1�ʿ� 1���� ����ȭ , �Ҷ� ����
}

// Called when the game starts or when spawned
void ANetActor::BeginPlay()
{
	Super::BeginPlay();

	MatInstDymc = MeshComp->CreateDynamicMaterialInstance(0);
	
	//�����϶��� �۵���Ű�ڴ�
	if (HasAuthority()) {
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			MatColor = FLinearColor(FMath::RandRange(0,1),FMath::RandRange(0,1), FMath::RandRange(0, 1),1);

			//���������� �߰��� ����
			OnRep_MatColor(); // Ŭ��ܿ��� ����Ǳ⶧���� �������� �����Ű�� ���� ������ �־���

		}),1,true);
	}
	
}

// Called every frame
void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintNetLog();
	FindOwner();

	//Yaw���� �������� P = P0 + vt ȸ���ӵ� 50
	if (HasAuthority()) {
		//�����϶��� �� �۾��� �ؾ���
		AddActorLocalRotation(FRotator(0,50*DeltaTime,0));
		RotYawValue = GetActorRotation().Yaw;
	}
	/* ///����ż� else�� ������ �ڵ����� �ؿ� �Լ� ����
	else {
		FRotator NewRot = GetActorRotation();
		NewRot.Yaw = RotYawValue;
		SetActorRotation(NewRot);
	}*/

	// Tick���� ��� �Ʒ� �Լ��� ȣ���ϴ� ���� ������ -> OnRep_MatColor()�� ����
	//if (MatInstDymc) {
	//	MatInstDymc->SetVectorParameterValue(TEXT("FloorColor"),MatColor);
	//}

}

//������̺� �����̸��� �ش��ϴ� �迭������ 
void ANetActor::OnRep_RotYawValue()
{
	FRotator NewRot = GetActorRotation();
	NewRot.Yaw = RotYawValue;
	SetActorRotation(NewRot);
}

//�������� ���� �ٲٸ� client���� �ٲ۴�. -> ���������� ������ �ȵ�(����ȵ�)
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

	//Owner ���� : �������� ���� ����� ĳ���͸� ã�Ƽ� Owner�� ����
	//if(GetLocalRole() == ROLE_Authority)
	if (HasAuthority()) {
		//������ �� Owner ����
		AActor* NewOwner = nullptr;
		float MinDist = SearchDistance;

		for (TActorIterator<ANetTPSCharacter> it(GetWorld()); it; ++it) {
			AActor* OtherActor = *it;
			float Dist = GetDistanceTo(OtherActor);

			// ������ ����ϰ� �ִ°Ÿ����� �۴ٸ�
			if (Dist < MinDist) {
				//�Ÿ� ������Ʈ
				MinDist = Dist;
				// Owner ����
				NewOwner = OtherActor;
			}
		}

		//���� ����
		//���� �����ϰ� �ٸ��༮�̸� ���� ������Ʈ
		if (GetOwner() != NewOwner) {
			SetOwner(NewOwner);
		}
	}


	//���� �ð�ȭ
	DrawDebugSphere(GetWorld(), GetActorLocation(), SearchDistance, 30, FColor::Yellow, false, 0, 0, 1);
}


void ANetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ����ȭ ���� ������ ���⿡ �ݵ�� �߰����� ��
	DOREPLIFETIME(ANetActor, RotYawValue);
	DOREPLIFETIME(ANetActor, MatColor);
}