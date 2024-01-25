// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "NetTPSGameInstance.h"
#include "Components/WidgetSwitcher.h"
#include "SessionSlotWidget.h"
#include <Components/ScrollBox.h>

void ULoginWidget::AddSlotWidget(const struct FSessionInfo& sessionInfo)
{
	// ���� ����
	auto slot = CreateWidget<USessionSlotWidget>(this, sessionSlotWidget);
	// �������Ҵ�
	slot->Set(sessionInfo);
	// ��ũ�ѹڽ��� �߰�
	Scroll_RoomList->AddChild(slot);

}

void ULoginWidget::OnClickedFindSession()
{
	//�� ��� ����
	Scroll_RoomList->ClearChildren();

	//�� ��� �˻�
	if (gi) {
		gi->FindOtherSessions();

	}
	//������ ������Ʈ ������
	




}

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BTN_CreateRoom->OnClicked.AddDynamic(this, &ULoginWidget::CreateRoom);


	//GameInstance
	gi = Cast<UNetTPSGameInstance>(GetWorld()->GetGameInstance());

	// �����߰� �̺�Ʈ �ݹ� ���
	gi->onSearchCompleted.AddDynamic(this, &ULoginWidget::AddSlotWidget);
	// �� ��� �˻������� ����, �̺�Ʈ �ݹ� ���
	gi->onSearchState.AddDynamic(this, &ULoginWidget::OnChangeButtonEnabled);
	BTN_Find->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedFindSession);
	
	Slider_PlayerCount->OnValueChanged.AddDynamic(this, &ULoginWidget::OnValueChanged);

	//canvas switch
	BTN_MainCreateSession->OnClicked.AddDynamic(this, &ULoginWidget::SwitchCreatePanel);
	BTN_MainFindSession->OnClicked.AddDynamic(this, &ULoginWidget::SwitchFindPanel);
	BTN_Back->OnClicked.AddDynamic(this, &ULoginWidget::SwitchMainPanel);
	BTN_Back1->OnClicked.AddDynamic(this, &ULoginWidget::SwitchMainPanel);

	

}

void ULoginWidget::CreateRoom()
{


	//GameInstance�� �ִ� CreateMyRoom() �Լ� ȣ��
	//FString sessionName, int32 playerCount

	if (gi && ETXT_RoomName->GetText().IsEmpty() == false) {
		FString roomName = ETXT_RoomName->GetText().ToString();
		int32 playerCount = Slider_PlayerCount->GetValue(); //int float �ڵ� Ÿ��ĳ���õ�
		gi->CreateMySession(roomName, playerCount);
	}	
}

void ULoginWidget::OnValueChanged(float value)
{
	TXT_PlayerCountNum->SetText(FText::AsNumber(value));
	
}

void ULoginWidget::OnChangeButtonEnabled(bool bIsSearching)
{
	BTN_Find->SetIsEnabled(!bIsSearching);//true�� false, false�� true�� �־������

	if (bIsSearching == true) {
		TXT_SearchingMsg->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		TXT_SearchingMsg->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ULoginWidget::SwitchCreatePanel()
{

	FString UserName = ETXT_UserName->GetText().ToString();

	if (UserName.IsEmpty() == false) {
		gi->hostName = UserName;
	}

	widgetSwitcher->SetActiveWidgetIndex(1);

}

void ULoginWidget::SwitchFindPanel()
{

	FString UserName = ETXT_UserName->GetText().ToString();
	if (UserName.IsEmpty() == false) {
		gi->hostName = UserName;
	}

	widgetSwitcher->SetActiveWidgetIndex(2);
	
	//�� ��� �˻�
	OnClickedFindSession();

}

void ULoginWidget::SwitchMainPanel()
{
	widgetSwitcher->SetActiveWidgetIndex(0);
}
