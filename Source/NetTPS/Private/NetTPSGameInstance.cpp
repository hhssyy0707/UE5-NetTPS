// Fill out your copyright notice in the Description page of Project Settings.


#include "NetTPSGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "NetTPS.h"
#include "Online/OnlineSessionNames.h"

UNetTPSGameInstance::UNetTPSGameInstance()
{
}

void UNetTPSGameInstance::Init()
{
	Super::Init();

	// �¶��� ���� �������̽� �����
	auto Subsys = IOnlineSubsystem::Get();
	if (Subsys) {
		SessionInterface = Subsys->GetSessionInterface();
		//�ݹ�
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetTPSGameInstance::OnCreateSessionComplete);
		//�˻� �ݹ�
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetTPSGameInstance::OnFindSessionsComplete);

		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetTPSGameInstance::OnJoinSessionComplete);
	}

	//FTimerHandle handle;
	//GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&] {CreateMySession(hostName, 10); }), 2, false);
	//GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&] {FindOtherSessions(); }), 2, false);

}

void UNetTPSGameInstance::CreateMySession(FString roomName, int32 playerCount)
{

	// ���� ����� �� ���� ���� �ʱ�ȭ
	FOnlineSessionSettings sessionSettings;

	//1.Dedicated Server ��뿩��
	sessionSettings.bIsDedicated = false;
	// 2. ������Ī(����), ������Ī ������� ����
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	//DefaultEngine.ini���� �������� ���ð� Ȯ�� DefaultPlatformService=Steam
	sessionSettings.bIsLANMatch = subsysName == "NULL";

	//��Ī�� �¶����� ���� ������� ����
	// false�� ��쿡�� �ʴ븦 ���ؼ��� ���� ����
	sessionSettings.bShouldAdvertise = true;
	//SessionInterface->SendSessionInviteToFriend()

	// 4. �¶��� ���� ������ Ȱ���ϰ� �� �� ����
	sessionSettings.bUsesPresence = true;

	//5. ���������߿� ������ �� �ִ��� ����
	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bAllowJoinViaPresence = true;

	//6. �ִ� ����ο��� ���� ( ���簡��������� �ִ�� �����ϰ���� ������ �ִ´�)

	sessionSettings.NumPublicConnections = playerCount;

	//7. Ŀ���� �ɼ� ex. ���̸�
	sessionSettings.Set(FName("ROOM_NAME"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	sessionSettings.Set(FName("HOST_NAME"), hostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);






	// ���� ����
	// ���� ���̵�� 0�̾ �������� Ư�� ����� ��Ҹ��� ����Ͱų� �Ҷ� �������̵� �ʿ��� -> �ݾ��̵� �ʿ�
	// net id
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	// ���������� ���
	SessionInterface->CreateSession(*netID, FName(hostName), sessionSettings);

	//�� ��� ��˻����̴ٴ� �� �˾ƶ�~
	onSearchState.Broadcast(true);



}

void UNetTPSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PRINTLOG(TEXT("Create Session Start : %s"), *SessionName.ToString(), bWasSuccessful);

	//�濡 ������ ó������
	if (bWasSuccessful) {
		//GetWorld()->ServerTravel(TEXT("/Script/Engine.World'/Game/Net/Maps/BattleMap.BattleMap'"));

		GetWorld()->ServerTravel(TEXT("/Game/Net/Maps/BattleMap?listen"));
	}
}

void UNetTPSGameInstance::FindOtherSessions()
{
	// �˻�����
	//sessionSearch = new FOnlineSessionSearch();
	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	//1.SEARCH_PRESENCE�� true�� �Ǿ��ִ� ���Ǹ� �˻�
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);

	//2.�� ����Ī����
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";

	//3. �ִ� �˻� ����(�� ��
	sessionSearch->MaxSearchResults = 10;

	//4. �˻�
	SessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
	//0 �ƹ��ų�?
	// Ư���� ��? id �� �Է�
	

	onSearchState.Broadcast(true);
}

void UNetTPSGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	//ã�� ���� ��
	if (bWasSuccessful == false) {
	onSearchState.Broadcast(false);
		PRINTLOG(TEXT("Session search failed..."));
		return;
	}

	//���ǰ˻� ���
	auto results = sessionSearch->SearchResults;
	PRINTLOG(TEXT("Search Result Count %d"), results.Num());

	for (int i = 0; i < results.Num(); i++) {
		auto sr = results[i];
		if (sr.IsValid() == false) {
			continue;

		}
		//1.���̸�
		//2.ȣ��Ʈ�̸�
		//3.�÷��̾��(�ִ밡�ɼ� - �������尡�ɼ�)
		//4.������

		/*
		FString roomName;
		FString hostName1;
		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), roomName);
		sr.Session.SessionSettings.Get(FName("HOST_NAME"), hostName1);
		//PC ������ �̸�
		FString OwnerName = sr.Session.OwningUserName;
		//�÷��̾��
		int32 maxPlayerCount = sr.Session.NumOpenPublicConnections;
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;
		//������
		int32 pingSpeed = sr.PingInMs; // ������ġ�϶��� 9999 (local network������ ���������� ����)
		
		PRINTLOG(TEXT("%s : %s(%s) - (%d/%d), %d ms"), *roomName, *hostName1, *OwnerName,currentPlayerCount,maxPlayerCount,pingSpeed);
		*/


		FSessionInfo sessionInfo;
		sessionInfo.index = i;

		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), sessionInfo.roomName);
		sr.Session.SessionSettings.Get(FName("HOST_NAME"), sessionInfo.hostName);

		//�÷��̾��
		int32 maxPlayerCount = sr.Session.NumOpenPublicConnections;
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;
		sessionInfo.playerCount = FString::Printf(TEXT("(%d/%d)"),currentPlayerCount, maxPlayerCount);
		
		//������
		sessionInfo.pingSpeed = sr.PingInMs; // ������ġ�϶��� 9999 (local network������ ���������� ����)

		PRINTLOG(TEXT("%s"),*sessionInfo.ToString());

		//240124 UI ���� �߰�
		//���⼭ �̶� ���� �߰� �ʿ� -> LoginWidget(level blueprint���� �����ϰ� ����)
		onSearchCompleted.Broadcast(sessionInfo);



	}

	//������ ������ ���� ���;���
	//SessionInterface->DestroySession();

	onSearchState.Broadcast(false);

}

void UNetTPSGameInstance::JoinSelectedSession(int32 roomIndex)
{
	auto sr = sessionSearch->SearchResults[roomIndex];

	SessionInterface->JoinSession(0,FName(hostName), sr);//���Ǽ��ÿ� ���ִ� ���������̵� �ᵵ�ǰ� Ȥ�� 0


}

void UNetTPSGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	//Client�� ������ �����Ѵ�.
	//->Client Travel 
	if (result==EOnJoinSessionCompleteResult::Success) {
		//������ ���� ������ url �ʿ�
		FString url;
		//url�� �����ε� �־���� ������ ����
		SessionInterface->GetResolvedConnectString(sessionName, url);

		//240124 �߰�
		PRINTLOG(TEXT("Client Travel Url : %s"), *url); // ��Ʈ��ȣ�� 0�̸� StandAlone�̴� -> lobbyGameMode.h���� bUseSeamlessTravel �ּ�ó���� ��

		if(url.IsEmpty() == false)
		{
			//url�� ������ ������ ����.
			auto PC = GetWorld()->GetFirstPlayerController();
			PC->ClientTravel(url, ETravelType::TRAVEL_Absolute); //���� �־��� �α��� �������� ������ ���� �� �� ���� , Relative�� �� ���� ������ ���� �� �� ����. ���̳� ĳ���� �������� ��

		}
	}
	else {
		PRINTLOG(TEXT("Join Session Failed...%d"), result);
	}

}
