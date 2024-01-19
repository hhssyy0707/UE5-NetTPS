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

	//모든 총악 UI 제거
	void RemoveAllBullets();

public:
	//체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HP")
	float PB_HPValue = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* DamageUIAnim;

	//피격처리 애니메이션 재생 함수
	UFUNCTION()
	void PlayDamageAnimation();

	//죽음UI
public:
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UHorizontalBox* HB_GameOverUI;
	
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UButton* BTN_Retry;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UButton* BTN_Exit;

public:
	virtual void NativeConstruct() override;

	//리트라이 버튼 눌렀을 때 실행되는 함수
	UFUNCTION()
	void OnRetry();
};
