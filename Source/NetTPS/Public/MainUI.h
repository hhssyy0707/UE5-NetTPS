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

	//��Ʈ���� ��ư ������ �� ����Ǵ� �Լ�
	UFUNCTION()
	void OnRetry();
};
