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
	// 위젯 생성
	auto slot = CreateWidget<USessionSlotWidget>(this, sessionSlotWidget);
	// 데이터할당
	slot->Set(sessionInfo);
	// 스크롤박스에 추가
	Scroll_RoomList->AddChild(slot);

}

void ULoginWidget::OnClickedFindSession()
{
	//방 목록 리셋
	Scroll_RoomList->ClearChildren();

	//방 목록 검색
	if (gi) {
		gi->FindOtherSessions();

	}
	//데이터 업데이트 시켜줌
	




}

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BTN_CreateRoom->OnClicked.AddDynamic(this, &ULoginWidget::CreateRoom);


	//GameInstance
	gi = Cast<UNetTPSGameInstance>(GetWorld()->GetGameInstance());

	// 슬롯추가 이벤트 콜백 등록
	gi->onSearchCompleted.AddDynamic(this, &ULoginWidget::AddSlotWidget);
	// 방 목록 검색중인지 여부, 이벤트 콜백 등록
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


	//GameInstance에 있는 CreateMyRoom() 함수 호출
	//FString sessionName, int32 playerCount

	if (gi && ETXT_RoomName->GetText().IsEmpty() == false) {
		FString roomName = ETXT_RoomName->GetText().ToString();
		int32 playerCount = Slider_PlayerCount->GetValue(); //int float 자동 타입캐스팅됨
		gi->CreateMySession(roomName, playerCount);
	}	
}

void ULoginWidget::OnValueChanged(float value)
{
	TXT_PlayerCountNum->SetText(FText::AsNumber(value));
	
}

void ULoginWidget::OnChangeButtonEnabled(bool bIsSearching)
{
	BTN_Find->SetIsEnabled(!bIsSearching);//true면 false, false면 true를 넣어줘야함

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
	
	//방 목록 검색
	OnClickedFindSession();

}

void ULoginWidget::SwitchMainPanel()
{
	widgetSwitcher->SetActiveWidgetIndex(0);
}
