// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "NetTPSGameInstance.generated.h"

//방정보(세션) 기록할 구조체
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

	//인라인함수 선언
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
	// 세션 설정
	IOnlineSessionPtr SessionInterface;

	// 세션 호스트 이름
	FString hostName = "Syuna";

	// 세션 함수 생성
	void CreateMySession(const FString roomName, const int32 playerCount);

	//세션 완성 이벤트 콜백
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

public:
//방검색

// 검색 조건 설정
// 스마트 포인터 사용
TSharedPtr<FOnlineSessionSearch> sessionSearch; //UProperty를 쓸 수 없음 -> 스마트 포인터를 사용한다.(스마트 포인터에 의해 메모리 관리를 받게함
// 클래스를 만들었을때 메모리 위에 올라가면
// 뉴 딜이 매칭이 안되면 메모리 누수가 일어남
// 펜딩킬, 덴글링 포인터등의 문제가 발생할 수 있음
void FindOtherSessions();

void OnFindSessionsComplete(bool bWasSuccessful);
};
