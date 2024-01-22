// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(NetLog, Log, All);

#define LOCALROLE (UEnum::GetValueAsString<ENetRole>(GetLocalRole()))

#define REMOTEROLE (UEnum::GetValueAsString<ENetRole>(GetRemoteRole()))

//Ŭ���̾�Ʈâ���� �������� ��������
#define NETMODE (GetWorld()->GetNetMode() == ENetMode::NM_Client ? TEXT("Client") : GetWorld()->GetNetMode() == ENetMode::NM_Standalone ? TEXT("StandAlone") : TEXT("Server"))

// �Լ� ȣ��Ǵ� ����
#define  CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

//���� ����ϴ� �Լ� ��ũ��
//�������� ...
#define  PRINTLOG(fmt, ... ) \
UE_LOG(NetLog, Warning, TEXT("[%s]%s %s"),NETMODE, *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__))
//UE_LOG(LogTemp, Warning, TEXT("[%s]%s %s"), NETMODE , *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__))