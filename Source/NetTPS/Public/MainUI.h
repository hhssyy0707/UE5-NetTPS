// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UMainUI : public UUserWidget
{
	GENERATED_BODY()
	
//protected:
	

public:

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UImage* IM_Crosshair;

	void ShowCrosshair(bool IsShow);

public:
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UUniformGridPanel* BulletPanel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet")
	TSubclassOf<class UUserWidget> BulletUIFactory;

	void AddBullet();

	void PopBullet(int32 BulletIndex);

	//��� �Ѿ� UI ����
	void RemoveAllBullets();

public:
	//ü��
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HP")
	float PB_HPValue = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* DamageUIAnim;

	//�ǰ�ó�� �ִϸ��̼� ��� �Լ�
	UFUNCTION()
	void PlayDamageAnimation();

	//����UI
public:
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UHorizontalBox* HB_GameOverUI;
	
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UButton* BTN_Retry;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UButton* BTN_Exit;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	//��Ʈ���� ��ư ������ �� ����Ǵ� �Լ�
	UFUNCTION()
	void OnRetry();


	
public:
//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,  meta = (BindWidget))
UPROPERTY(meta = (BindWidget))
class UTextBlock* TXT_UserName;

// ä��===============================
public:
UPROPERTY(meta = (BindWidget))
class UScrollBox* Scroll_MsgList;

UPROPERTY(meta = (BindWidget))
class UEditableText* ETXT_MsgInput;

UPROPERTY(meta = (BindWidget))
class UButton* BTN_MsgSend;

UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
TSubclassOf<class UChatWidget> chatWidget;


	UFUNCTION()
	void SendMsg();

	UFUNCTION()
	void ReceiveMsg(const FString& msg);


};
