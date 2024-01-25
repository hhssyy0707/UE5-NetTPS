// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"

#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "NetPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include <Components/TextBlock.h>
#include "NetTPS.h"
#include <Components/EditableText.h>
#include "NetTPSCharacter.h"
#include "ChatWidget.h"
#include <../../../../../../../Source/Runtime/UMG/Public/Components/ScrollBox.h>


void UMainUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//PRINTLOG(TEXT("============================"));
	// 다른 사용자들의 GameInstance에 있는 세션네임을 가져오고 싶다.
	TArray<APlayerState*> playerArr = GetWorld()->GetGameState()->PlayerArray;
	FString name1;

	//PRINTLOG(TEXT("%s"), *name1);

	for (auto pState : playerArr) {
		//name.Append(FString::Printf(TEXT("%s\n"),*pState->SessionName.ToString()));
		name1.Append(FString::Printf(TEXT("%s : %d\n"), *pState->GetPlayerName(), (int32)pState->GetScore()));

	}

	TXT_UserName->SetText(FText::FromString(name1));

}


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
	Super::NativeConstruct();
	
	//버튼이벤트 등록하게
	BTN_Retry->OnClicked.AddDynamic(this, &UMainUI::OnRetry);

	BTN_MsgSend->OnClicked.AddDynamic(this, &UMainUI::SendMsg);
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
		//PC-> ServerRPC_RespawnPlayer();
		PC-> ServerRPC_ChangeToSpectator();

	}



}

void UMainUI::SendMsg()
{
	//NetPTSCharacter에 있는 ServerRPC
	// 메세지를 서버로 전송하고 싶다.
	//1. 서버로 전송
	//2. NetPTSCharacter가 필요
	//3. NetPlayerController가 필요
	// 순서 3->2->1
	
	FString msg = ETXT_MsgInput->GetText().ToString();
	if (msg.IsEmpty() == false) {
		// PlayerController 불러오기
		auto pc = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
		// GetPawn
		auto player = pc->GetPawn<ANetTPSCharacter>();
		// Server 전송
		player->ServerRPC_SendMsg(msg);
	}
	ETXT_MsgInput->SetText(FText::GetEmpty());

}

void UMainUI::ReceiveMsg(const FString& msg)
{
	// 메세지 받아서 Chatwidget에 내용 넣어주기(UClass 필요 CreateWidget하려면)
	auto msgWidget = CreateWidget<UChatWidget>(GetWorld(), chatWidget);
	msgWidget->TXT_MsgList->SetText(FText::FromString(msg));

	//스크롤 박스에 추가해주기
	Scroll_MsgList->AddChild(msgWidget);
	Scroll_MsgList->ScrollToEnd();
	
}
