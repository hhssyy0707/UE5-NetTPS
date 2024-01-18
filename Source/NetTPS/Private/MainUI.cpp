// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"

#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "NetPlayerController.h"

void UMainUI::ShowCrosshair(bool IsShow)
{
	if (IsShow) {
		IM_Crosshair->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		IM_Crosshair->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainUI::AddBullet()
{
	auto BulletWidget = CreateWidget(GetWorld(), BulletUIFactory);
	BulletPanel->AddChildToUniformGrid(BulletWidget, 1, BulletPanel->GetChildrenCount());
}

void UMainUI::PopBullet(int32 BulletIndex)
{
	BulletPanel->RemoveChildAt(BulletIndex);
}

void UMainUI::RemoveAllBullets()
{
	/*for (auto BulltetWidget : BulletPanel->GetAllChildren()) {
		BulletPanel->RemoveChild(BulltetWidget);
	}*/

	BulletPanel->ClearChildren();
}

void UMainUI::PlayDamageAnimation()
{
	// �ǰ� �� �ִϸ��̼� ���
	PlayAnimation(DamageUIAnim);

}

void UMainUI::NativeConstruct()
{
	Super::NativeDestruct();
	
	//��ư�̺�Ʈ ����ϰ�
	BTN_Retry->OnClicked.AddDynamic(this, &UMainUI::OnRetry);

	

}

void UMainUI::OnRetry()
{
	//  UI�ϱ� Client �ڵ�
	// Server���� ������ ��û�� �ϰ� �ʹ�.
	
	//1. ���콺 , UI Visibility ���� �ʿ�(���ִ� ����)
	HB_GameOverUI->SetVisibility(ESlateVisibility::Hidden);
	auto PC = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC) {
		PC->SetShowMouseCursor(false);
		//3. Server�� Respawn ��û
		PC-> ServerRPC_RespawnPlayer();
	}



}
