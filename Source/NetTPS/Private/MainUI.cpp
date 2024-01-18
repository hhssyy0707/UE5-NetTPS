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
	// 피격 시 애니메이션 재생
	PlayAnimation(DamageUIAnim);

}

void UMainUI::NativeConstruct()
{
	Super::NativeDestruct();
	
	//버튼이벤트 등록하게
	BTN_Retry->OnClicked.AddDynamic(this, &UMainUI::OnRetry);

	

}

void UMainUI::OnRetry()
{
	//  UI니까 Client 코드
	// Server한테 리스폰 요청을 하고 싶다.
	
	//1. 마우스 , UI Visibility 변경 필요(떠있는 상태)
	HB_GameOverUI->SetVisibility(ESlateVisibility::Hidden);
	auto PC = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC) {
		PC->SetShowMouseCursor(false);
		//3. Server에 Respawn 요청
		PC-> ServerRPC_RespawnPlayer();
	}



}
