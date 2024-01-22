// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "NetTPSGameInstance.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BTN_CreateRoom->OnClicked.AddDynamic(this, &ULoginWidget::CreateRoom);


	//GameInstance
	gi = Cast<UNetTPSGameInstance>(GetWorld()->GetGameInstance());

	Slider_PlayerCount->OnValueChanged.AddDynamic(this, &ULoginWidget::OnValueChanged);

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
