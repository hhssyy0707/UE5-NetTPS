// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "NetTPSGameInstance.generated.h"

//������(����) ����� ����ü
USTRUCT(BlueprintType)
struct FSessionInfo {
	
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString roomName;
	UPROPERTY(BlueprintReadOnly)
	FString hostName;
	UPROPERTY(BlueprintReadOnly)
	FString playerCount;
	UPROPERTY(BlueprintReadOnly)
	int32 pingSpeed;
	UPROPERTY(BlueprintReadOnly)
	int32 index;

	//�ζ����Լ� ����
	inline FString ToString(){
		return FString::Printf(TEXT("[%d] %s : %s - %s, %d ms"), index, *roomName, *hostName, *playerCount, pingSpeed);
	}


};

/**
 * 
 */
UCLASS()
class NETTPS_API UNetTPSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UNetTPSGameInstance();

	virtual void Init() override;


public:
	// ���� ����
	IOnlineSessionPtr SessionInterface;

	// ���� ȣ��Ʈ �̸�
	FString hostName = "Syuna";

	// ���� �Լ� ����
	void CreateMySession(const FString roomName, const int32 playerCount);

	//���� �ϼ� �̺�Ʈ �ݹ�
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

public:
//��˻�

// �˻� ���� ����
// ����Ʈ ������ ���
TSharedPtr<FOnlineSessionSearch> sessionSearch; //UProperty�� �� �� ���� -> ����Ʈ �����͸� ����Ѵ�.(����Ʈ �����Ϳ� ���� �޸� ������ �ް���
// Ŭ������ ��������� �޸� ���� �ö󰡸�
// �� ���� ��Ī�� �ȵǸ� �޸� ������ �Ͼ
// ���ų, ���۸� �����͵��� ������ �߻��� �� ����
void FindOtherSessions();

void OnFindSessionsComplete(bool bWasSuccessful);
};
