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

public:
//���ǽ���
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
class UScrollBox* Scroll_RoomList;

UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
class UButton* BTN_Find;

UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
class UTextBlock* TXT_SearchingMsg;


//���� ���� ����
UPROPERTY(EditDefaultsOnly)
TSubclassOf<class USessionSlotWidget> sessionSlotWidget;
//class USessionSlotWidget* slotWidget;

UFUNCTION()
void AddSlotWidget(const struct FSessionInfo& sessionInfo);

UFUNCTION()
void OnClickedFindSession();

	//Delegatee
	//��ã�� ���� �̺�Ʈ �ݹ�
	//��ε�ĳ��Ʈ�� ȣ��� �༮
	UFUNCTION()
	void OnChangeButtonEnabled(bool bIsSearching);


public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void CreateRoom();

	//�����̴��� ����� ȣ��Ǵ� �Լ� 
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
