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
	// �ٸ� ����ڵ��� GameInstance�� �ִ� ���ǳ����� �������� �ʹ�.
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
	// �ǰ� �� �ִϸ��̼� ���
	PlayAnimation(DamageUIAnim);

}

void UMainUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	//��ư�̺�Ʈ ����ϰ�
	BTN_Retry->OnClicked.AddDynamic(this, &UMainUI::OnRetry);

	BTN_MsgSend->OnClicked.AddDynamic(this, &UMainUI::SendMsg);
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
		//PC-> ServerRPC_RespawnPlayer();
		PC-> ServerRPC_ChangeToSpectator();

	}



}

void UMainUI::SendMsg()
{
	//NetPTSCharacter�� �ִ� ServerRPC
	// �޼����� ������ �����ϰ� �ʹ�.
	//1. ������ ����
	//2. NetPTSCharacter�� �ʿ�
	//3. NetPlayerController�� �ʿ�
	// ���� 3->2->1
	
	FString msg = ETXT_MsgInput->GetText().ToString();
	if (msg.IsEmpty() == false) {
		// PlayerController �ҷ�����
		auto pc = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
		// GetPawn
		auto player = pc->GetPawn<ANetTPSCharacter>();
		// Server ����
		player->ServerRPC_SendMsg(msg);
	}
	ETXT_MsgInput->SetText(FText::GetEmpty());

}

void UMainUI::ReceiveMsg(const FString& msg)
{
	// �޼��� �޾Ƽ� Chatwidget�� ���� �־��ֱ�(UClass �ʿ� CreateWidget�Ϸ���)
	auto msgWidget = CreateWidget<UChatWidget>(GetWorld(), chatWidget);
	msgWidget->TXT_MsgList->SetText(FText::FromString(msg));

	//��ũ�� �ڽ��� �߰����ֱ�
	Scroll_MsgList->AddChild(msgWidget);
	Scroll_MsgList->ScrollToEnd();
	
}
