// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "EnhancedInputSubSystems.h"
#include "EnhancedInputComponent.h"
#include "PlayerMove.h"
#include "PlayerFire.h"
#include "TPS_Wave_DefensePR.h"
#include <Kismet/GameplayStatics.h>
#include "WaveManager.h"
#include "RankingSaveGame.h"
#include "RankEntry.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스캘레탈메시 데이터를 불러오고 싶다
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		// 2. Mesh 컴포넌트의 위치와 회전 값을 설정하고 싶다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	// 3. TPS 카메라를 붙이고 싶다
	// 3-1. SpringArm 컵포넌트 붙이기
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;
	// 3-2. Camera 컴포넌트 붙이기
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	tpsCamComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;

	// 4. 총 스켈레탈메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	// 4-1. 부모 컴포넌트를 Mesh 컴포넌트로 설정
	//gunMeshComp->SetupAttachment(GetMesh());
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	// 4-2. 스켈레탈메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	// 4-3. 데이터 로드가 성공했다면
	if (TempGunMesh.Succeeded()) {
		// 4-4. 스켈레탈메시 데이터 할당
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		// 4-5. 위치 조정하기
		//gunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
		gunMeshComp->SetRelativeLocation(FVector(-17, 10, -3));
		gunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));
	}

	// 5. 스나이퍼건 컴포넌트 등록
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	// 5-1. 부모 컴포넌트를 Mesh 컴포넌트로 등록
	//sniperGunComp->SetupAttachment(GetMesh());
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	//5-2. 스태틱 매시 로드
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	// 5-3. 데이터 로드가 성공했다면
	if (TempSniperMesh.Succeeded()) {
		// 5-4. 스태틱메시 데이터 할당
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		// 5-5. 위치 조정하기
		//sniperGunComp->SetRelativeLocation(FVector(-22, 55, 120));
		sniperGunComp->SetRelativeLocation(FVector(-42, 7, 1));
		sniperGunComp->SetRelativeRotation(FRotator(0, 90, 0));
		// 5-6. 크기 조정하기
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}

	playerMove = CreateDefaultSubobject<UPlayerMove>(TEXT("PlayerMove"));
	//playerFire = CreateDefaultSubobject<UPlayerFire>(TEXT("PlayerFire"));
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	auto pc = Cast<APlayerController>(Controller);
	if (pc) {
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem) {
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}

	hp = initialHp;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AWaveManager::StaticClass(),
		FoundActors
	);

	if (FoundActors.Num() > 0)
	{
		WaveManager = Cast<AWaveManager>(FoundActors[0]);
		UE_LOG(LogTemp, Warning, TEXT("[TPSPlayer] WaveManager FOUND"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[TPSPlayer] WaveManager NOT FOUND"));
	}
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (PlayerInput) {
		// 컴포넌트에서 입력 바인딩 처리하도록 호출
		//playerMove->SetupInputBinding(PlayerInput);
		//playerFire->SetupInputBinding(PlayerInput);

		onInputBindingDelegate.Broadcast(PlayerInput);
	}
}

void ATPSPlayer::OnHitEvent()
{
	PRINT_LOG(TEXT("Damaged!!!!!!"));
	hp--;
	if (hp <= 0) {
		PRINT_LOG(TEXT("Player is dead!"));
		OnGameOver();
	}
}

void ATPSPlayer::OnGameOver_Implementation()
{
	if (!WaveManager) {
		UE_LOG(LogTemp, Error, TEXT("[OnGameOver] WaveManager is null"));
		return;
	}

	// 여기서 waveManager의 GAmeOver를 트리거?
	WaveManager->HandleGameOver();

	FinalStage = WaveManager->GetFinalStage();
	FinalScore = WaveManager->GetFinalScore();
	FinalKillCount = WaveManager->GetFinalKill();

	UE_LOG(LogTemp, Warning,
		TEXT("[GameOver Result] Score=%d Stage=%d Kill=%d"),
		FinalScore, FinalStage, FinalKillCount
	);

	// 랭킹 시스템 -> 위젯 블루프린트에서 이걸 호출하도록 수정
	//SaveRankingResult();

	// 게임 오버 시 일시 정지
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void ATPSPlayer::SetPlayerName(const FString& InName)
{
	PlayerName = InName;
}

void ATPSPlayer::SaveRankingResult()
{
	URankingSaveGame* SaveData = nullptr;

	if (UGameplayStatics::DoesSaveGameExist("RankingSlot", 0))
	{
		SaveData = Cast<URankingSaveGame>(
			UGameplayStatics::LoadGameFromSlot("RankingSlot", 0)
		);
	}
	else
	{
		SaveData = Cast<URankingSaveGame>(
			UGameplayStatics::CreateSaveGameObject(
				URankingSaveGame::StaticClass()
			)
		);
	}

	if (!SaveData)
	{
		UE_LOG(LogTemp, Error, TEXT("[Ranking] Failed to load/create SaveGame"));
		return;
	}

	// RankEntry 생성
	FRankEntry NewEntry;
	//NewEntry.PlayerName = TEXT("PLAYER");   
	NewEntry.PlayerName = PlayerName.IsEmpty() ? TEXT("PLAYER") : PlayerName;
	NewEntry.Score = FinalScore;
	NewEntry.Stage = FinalStage;
	NewEntry.KillCount = FinalKillCount;

	// 추가 + 정렬 + Top10 유지
	SaveData->AddEntryAndSort(NewEntry, 10);

	// 저장
	UGameplayStatics::SaveGameToSlot(SaveData, "RankingSlot", 0);

	// 로그로 검증
	SaveData->DebugPrintRanks();

	// top 10 로그로 보기
	int32 Rank = SaveData->FindEntryRank(NewEntry);
	UE_LOG(LogTemp, Warning,
		TEXT("[Ranking] Current Rank: %d / %d"),
		Rank,
		SaveData->TopRanks.Num()
	);
}



