// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionSlotWidget.generated.h"




/**
 * 
 */
UCLASS()
class NETTPS_API USessionSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TXT_RoomName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TXT_HostName;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TXT_PlayerCount;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TXT_PingSpeed;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TXT_SessionNum;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BTN_Join;

	int32 sessionNumber;

	void Set(const struct FSessionInfo& sessionInfo);

	

	//세션 조인
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void JoinSession();


};
