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

	// 온라인 세션 인터페이스 만들기
	auto Subsys = IOnlineSubsystem::Get();
	if (Subsys) {
		SessionInterface = Subsys->GetSessionInterface();
		//콜백
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetTPSGameInstance::OnCreateSessionComplete);
		//검색 콜백
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetTPSGameInstance::OnFindSessionsComplete);

		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetTPSGameInstance::OnJoinSessionComplete);
	}

	//FTimerHandle handle;
	//GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&] {CreateMySession(hostName, 10); }), 2, false);
	//GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&] {FindOtherSessions(); }), 2, false);

}

void UNetTPSGameInstance::CreateMySession(FString roomName, int32 playerCount)
{

	// 세션 만들기 전 세션 설정 초기화
	FOnlineSessionSettings sessionSettings;

	//1.Dedicated Server 사용여부
	sessionSettings.bIsDedicated = false;
	// 2. 랜선매칭(로컬), 스팀매칭 사용할지 여부
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	//DefaultEngine.ini에서 스팀으로 세팅값 확인 DefaultPlatformService=Steam
	sessionSettings.bIsLANMatch = subsysName == "NULL";

	//매칭이 온라인을 통해 노출될지 여부
	// false일 경우에는 초대를 통해서만 입장 가능
	sessionSettings.bShouldAdvertise = true;
	//SessionInterface->SendSessionInviteToFriend()

	// 4. 온라인 상태 정보를 활용하게 할 지 여부
	sessionSettings.bUsesPresence = true;

	//5. 게임진행중에 조인할 수 있는지 여부
	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bAllowJoinViaPresence = true;

	//6. 최대 허용인원수 설정 ( 현재가능입장수에 최대로 설정하고싶은 변수를 넣는다)

	sessionSettings.NumPublicConnections = playerCount;

	//7. 커스텀 옵션 ex. 방이름
	sessionSettings.Set(FName("ROOM_NAME"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	sessionSettings.Set(FName("HOST_NAME"), hostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);






	// 세션 생성
	// 세션 아이디는 0이어도 괜찮지만 특정 사람의 목소리를 끄고싶거나 할때 유저아이디가 필요함 -> 넷아이디 필요
	// net id
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	// 참조연산자 사용
	SessionInterface->CreateSession(*netID, FName(hostName), sessionSettings);

	//야 모두 방검색중이다는 걸 알아라~
	onSearchState.Broadcast(true);



}

void UNetTPSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PRINTLOG(TEXT("Create Session Start : %s"), *SessionName.ToString(), bWasSuccessful);

	//방에 들어가도록 처리하자
	if (bWasSuccessful) {
		//GetWorld()->ServerTravel(TEXT("/Script/Engine.World'/Game/Net/Maps/BattleMap.BattleMap'"));

		GetWorld()->ServerTravel(TEXT("/Game/Net/Maps/BattleMap?listen"));
	}
}

void UNetTPSGameInstance::FindOtherSessions()
{
	// 검색조건
	//sessionSearch = new FOnlineSessionSearch();
	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	//1.SEARCH_PRESENCE이 true로 되어있는 세션만 검색
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);

	//2.랜 ㅂ매칭여부
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";

	//3. 최대 검색 세션(방 수
	sessionSearch->MaxSearchResults = 10;

	//4. 검색
	SessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
	//0 아무거나?
	// 특이한 방? id 값 입력
	

	onSearchState.Broadcast(true);
}

void UNetTPSGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	//찾기 실패 시
	if (bWasSuccessful == false) {
	onSearchState.Broadcast(false);
		PRINTLOG(TEXT("Session search failed..."));
		return;
	}

	//세션검색 결과
	auto results = sessionSearch->SearchResults;
	PRINTLOG(TEXT("Search Result Count %d"), results.Num());

	for (int i = 0; i < results.Num(); i++) {
		auto sr = results[i];
		if (sr.IsValid() == false) {
			continue;

		}
		//1.방이름
		//2.호스트이름
		//3.플레이어수(최대가능수 - 현재입장가능수)
		//4.핑정보

		/*
		FString roomName;
		FString hostName1;
		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), roomName);
		sr.Session.SessionSettings.Get(FName("HOST_NAME"), hostName1);
		//PC 소유자 이름
		FString OwnerName = sr.Session.OwningUserName;
		//플레이어수
		int32 maxPlayerCount = sr.Session.NumOpenPublicConnections;
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;
		//핑정보
		int32 pingSpeed = sr.PingInMs; // 스팀매치일때는 9999 (local network에서는 정상적으로 들어옴)
		
		PRINTLOG(TEXT("%s : %s(%s) - (%d/%d), %d ms"), *roomName, *hostName1, *OwnerName,currentPlayerCount,maxPlayerCount,pingSpeed);
		*/


		FSessionInfo sessionInfo;
		sessionInfo.index = i;

		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), sessionInfo.roomName);
		sr.Session.SessionSettings.Get(FName("HOST_NAME"), sessionInfo.hostName);

		//플레이어수
		int32 maxPlayerCount = sr.Session.NumOpenPublicConnections;
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;
		sessionInfo.playerCount = FString::Printf(TEXT("(%d/%d)"),currentPlayerCount, maxPlayerCount);
		
		//핑정보
		sessionInfo.pingSpeed = sr.PingInMs; // 스팀매치일때는 9999 (local network에서는 정상적으로 들어옴)

		PRINTLOG(TEXT("%s"),*sessionInfo.ToString());

		//240124 UI 슬롯 추가
		//여기서 이때 슬롯 추가 필요 -> LoginWidget(level blueprint에서 생성하고 있음)
		onSearchCompleted.Broadcast(sessionInfo);



	}

	//방목록을 보려면 방을 나와야함
	//SessionInterface->DestroySession();

	onSearchState.Broadcast(false);

}

void UNetTPSGameInstance::JoinSelectedSession(int32 roomIndex)
{
	auto sr = sessionSearch->SearchResults[roomIndex];

	SessionInterface->JoinSession(0,FName(hostName), sr);//세션세팅에 들어가있는 넷유저아이디 써도되고 혹은 0


}

void UNetTPSGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	//Client가 방으로 들어가야한다.
	//->Client Travel 
	if (result==EOnJoinSessionCompleteResult::Success) {
		//서버가 만든 세션의 url 필요
		FString url;
		//url이 참조로돼 있어서여기 변수에 담긴다
		SessionInterface->GetResolvedConnectString(sessionName, url);

		//240124 추가
		PRINTLOG(TEXT("Client Travel Url : %s"), *url); // 포트번호가 0이면 StandAlone이다 -> lobbyGameMode.h에서 bUseSeamlessTravel 주석처리할 것

		if(url.IsEmpty() == false)
		{
			//url이 있으면 방으로 들어간다.
			auto PC = GetWorld()->GetFirstPlayerController();
			PC->ClientTravel(url, ETravelType::TRAVEL_Absolute); //전에 있었던 로그인 정보등을 날리고 새로 들어갈 수 있음 , Relative는 몇 가지 정보를 갖고 들어갈 수 있음. 팀이나 캐릭터 정보같은 건

		}
	}
	else {
		PRINTLOG(TEXT("Join Session Failed...%d"), result);
	}

}
