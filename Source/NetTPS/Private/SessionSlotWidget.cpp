// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionSlotWidget.h"

#include "NetTPSGameInstance.h"
#include <Components/Button.h>
#include <Components/TextBlock.h>


void USessionSlotWidget::Set(const struct FSessionInfo& sessionInfo)
{
	TXT_SessionNum->SetText(FText::FromString(FString::Printf(TEXT("[%d]"), sessionInfo.index)));
	TXT_RoomName->SetText(FText::FromString(sessionInfo.roomName));
	TXT_HostName->SetText(FText::FromString(sessionInfo.hostName));
	TXT_PlayerCount->SetText(FText::FromString(sessionInfo.playerCount));
	TXT_PingSpeed->SetText(FText::FromString(FString::Printf(TEXT("%d ms"), sessionInfo.pingSpeed)));

	sessionNumber = sessionInfo.index;
}

void USessionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BTN_Join->OnClicked.AddDynamic(this, &USessionSlotWidget::JoinSession);

}

void USessionSlotWidget::JoinSession()
{
	auto gi = Cast<UNetTPSGameInstance>(GetWorld()->GetGameInstance());

	if (gi) {
		gi->JoinSelectedSession(sessionNumber);
	}

}
