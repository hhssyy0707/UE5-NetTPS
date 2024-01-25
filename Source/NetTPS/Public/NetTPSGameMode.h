// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NetTPSGameMode.generated.h"

UCLASS(minimalapi)
class ANetTPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANetTPSGameMode();

	//240125 추가
	// Player를 원하는 위치에 리스폰 시키고 싶을때 사용하는 함수
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MemberNum = 8;
};



