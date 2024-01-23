// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()
public:
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
class UButton* BTN_CreateRoom;

UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
class UEditableText* ETXT_RoomName;

UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
class USlider* Slider_PlayerCount;

UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
class UTextBlock* TXT_PlayerCountNum; 




UPROPERTY()
class UNetTPSGameInstance* gi;

public:
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
class UWidgetSwitcher* widgetSwitcher;

UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
class UButton* BTN_MainCreateSession;

UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
class UButton* BTN_MainFindSession;

UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
class UButton* BTN_Back;

UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
class UButton* BTN_Back1;


UPROPERTY(EditDefaultsOnly)
class USessionSlotWidget* slotWidget;




public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void CreateRoom();

	//슬라이더값 변경시 호출되는 함수 
	UFUNCTION()
	void OnValueChanged(float value);




public:

	UFUNCTION()
	void SwitchCreatePanel();
	UFUNCTION()
	void SwitchFindPanel();
	UFUNCTION()
	void SwitchMainPanel();



};
