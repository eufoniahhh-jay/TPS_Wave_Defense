// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
#include "Enemy.h"
#include <EngineUtils.h>
#include <Kismet/GameplayStatics.h>
#include "WaveManager.h"

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	// Tick()함수가 매 프레임 호출되지 않도록 설정
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	// WaveManager 찾기
	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaveManager::StaticClass(), FoundManagers);
	if (FoundManagers.Num() > 0) {
		WaveManager = Cast<AWaveManager>(FoundManagers[0]);
	}
	
	//// 1. 랜덤 생성 시간 구하기
	//float createTime = FMath::RandRange(minTime, maxTime);
	//// 2. Timer Manager한테 알람 등록
	//GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, createTime);
	StartSpawning();

	// 스폰 위치 동적 할당
	FindSpawnPoints();
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*void AEnemyManager::CreateEnemy()
{
	// + 안전 장치
	if (spawnPoints.Num() == 0 || !enemyFactory)
		return;

	// 랜덤 위치 구하기
	int index = FMath::RandRange(0, spawnPoints.Num() - 1);
	// 적 생성 및 배치하기
	AEnemy* Enemy = GetWorld()->SpawnActor<AEnemy>(enemyFactory, spawnPoints[index]->GetActorLocation(), FRotator(0));

	//// Difficulty 적용
	//if (Enemy && WaveManager)
	//{
	//	FEnemyDifficulty Diff = WaveManager->GetDifficultyForSpawn();
	//	Enemy->ApplyDifficulty(Diff);
	//
	//	UE_LOG(LogTemp, Log,
	//		TEXT("[EnemyManager] Spawn Enemy | Star=%d"),
	//		Diff.StarLevel
	//	);
	//}

	// Data Asset 반영 Difficulty 적용
	// 부터
	if (!Enemy)
		return;

	// 1. Difficulty 획득
	FEnemyDifficulty Diff = WaveManager->GetDifficultyForSpawn();

	// 2. EnemyData 적용 (Base + 외형)
	UEnemyDataAsset* EnemyData = GetEnemyDataByStar(Diff.StarLevel);
	if (EnemyData)
	{
		Enemy->ApplyEnemyData(EnemyData);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[EnemyManager] EnemyData not found for Star=%d"),
			Diff.StarLevel
		);
	}

	// 3. Difficulty 적용 (Multiplier)
	Enemy->ApplyDifficulty(Diff);

	UE_LOG(LogTemp, Log,
		TEXT("[EnemyManager] Spawn Enemy | Star=%d"),
		Diff.StarLevel
	);

	// 까지

	// 다시 랜덤 시간에 CreateEnemy 함수가 호출되도록 타이머 설정
	//float createTime = FMath::RandRange(minTime, maxTime);
	//GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, createTime);

	// 이제는 wave가 증가할 수록 더 빨리 적이 생성되도록 할 것이므로, stage 반영해서 수정
	float createTime = minTime;

	if (WaveManager)
	{
		createTime = WaveManager->GetSpawnInterval();
	}

	GetWorld()->GetTimerManager().SetTimer(
		spawnTimerHandle,
		this,
		&AEnemyManager::CreateEnemy,
		createTime,
		false
	);

	UE_LOG(LogTemp, Log,
		TEXT("[EnemyManager] Next Spawn In %.2f sec (Stage=%d)"),
		createTime,
		WaveManager ? WaveManager->GetCurrentStage() : -1
	);
}*/

void AEnemyManager::CreateEnemy()
{
	// + 안전 장치
	if (spawnPoints.Num() == 0 || !enemyFactory || !WaveManager)
		return;

	// 랜덤 위치 구하기
	int index = FMath::RandRange(0, spawnPoints.Num() - 1);
	FTransform SpawnTM(FRotator::ZeroRotator, spawnPoints[index]->GetActorLocation());

	// 1. Deferred Spawn
	AEnemy* Enemy = GetWorld()->SpawnActorDeferred<AEnemy>(enemyFactory, SpawnTM);
	if (!Enemy)
		return;

	// Data Asset 반영 Difficulty 적용
	// 2. Difficulty 획득
	FEnemyDifficulty Diff = WaveManager->GetDifficultyForSpawn();

	// 3. EnemyData 적용 (Mesh / AnimBP / BaseStat)
	UEnemyDataAsset* EnemyData = GetEnemyDataByStar(Diff.StarLevel);
	if (EnemyData)
	{
		Enemy->ApplyEnemyData(EnemyData);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[EnemyManager] EnemyData not found for Star=%d"),
			Diff.StarLevel
		);
	}

	// 4. Difficulty 적용 (Multiplier)
	Enemy->ApplyDifficulty(Diff);

	// 5. 여기서 BeginPlay 실행됨
	UGameplayStatics::FinishSpawningActor(Enemy, SpawnTM);

	UE_LOG(LogTemp, Log,
		TEXT("[EnemyManager] Spawn Enemy | Star=%d"),
		Diff.StarLevel
	);

	// star5 사운드 적용
	if (EnemyData && Diff.StarLevel == 5)
	{
		UGameplayStatics::PlaySound2D(this, Star5SpawnSFX);
	}


	// Spawn Interval 처리
	// 이제는 wave가 증가할 수록 더 빨리 적이 생성되도록 할 것이므로, stage 반영해서 수정
	float createTime = minTime;
	createTime = WaveManager->GetSpawnInterval();

	GetWorld()->GetTimerManager().SetTimer(
		spawnTimerHandle,
		this,
		&AEnemyManager::CreateEnemy,
		createTime,
		false
	);

	UE_LOG(LogTemp, Log,
		TEXT("[EnemyManager] Next Spawn In %.2f sec (Stage=%d)"),
		createTime,
		WaveManager->GetCurrentStage()
	);
}


// 스폰 위치 동적할당
void AEnemyManager::FindSpawnPoints()
{
	//// 방법 1.
	//for (TActorIterator<AActor> It(GetWorld()); It; ++It) {
	//	AActor* spawn = *It;
	//	// 찾은 액터의 이름에 해당 문자열을 포함하고 있다면
	//	if (spawn->GetName().Contains(TEXT("BP_EnemySpawnPoint"))) {
	//		// 스폰 목록에 추가
	//		spawnPoints.Add(spawn);
	//	}
	//}

	// 방법 2.
	// 검색으로 찾은 결과를 저장할 배열
	TArray<AActor*> allActors;
	// 원하는 타입의 액터 모두 찾아오기
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), allActors);
	// 찾은 결과가 있을 경우 반복적으로
	for (auto spawn : allActors) {
		// 찾은 액터의 이름에 해당 문자열을 포함하고 있다면
		if (spawn->GetName().Contains(TEXT("BP_EnemySpawnPoint"))) {
			// 스폰 목록에 추가
			spawnPoints.Add(spawn);
		}
	}
}

void AEnemyManager::ClearAllEnemies()
{
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Enemies);

	for (AActor* Enemy : Enemies)
	{
		Enemy->Destroy();
	}

	UE_LOG(LogTemp, Log, TEXT("[EnemyManager] All enemies cleared"));
}

void AEnemyManager::StartSpawning()
{
	/*float createTime = FMath::RandRange(minTime, maxTime);
	GetWorld()->GetTimerManager().SetTimer(
		spawnTimerHandle,
		this,
		&AEnemyManager::CreateEnemy,
		createTime,
		false
	);
	UE_LOG(LogTemp, Log, TEXT("[EnemyManager] Start Wave Spawning"));*/

	float createTime = minTime;

	if (WaveManager)
	{
		createTime = WaveManager->GetSpawnInterval();
	}

	GetWorld()->GetTimerManager().SetTimer(
		spawnTimerHandle,
		this,
		&AEnemyManager::CreateEnemy,
		createTime,
		false
	);

	UE_LOG(LogTemp, Log,
		TEXT("[EnemyManager] Start Wave Spawning | FirstSpawn=%.2f"),
		createTime
	);
}

void AEnemyManager::StopSpawning()
{
	GetWorld()->GetTimerManager().ClearTimer(spawnTimerHandle);

	UE_LOG(LogTemp, Log, TEXT("[EnemyManager] Stop Wave Spawning"));
}

UEnemyDataAsset* AEnemyManager::GetEnemyDataByStar(int32 StarLevel) const
{
	int32 Index = StarLevel - 1;

	if (EnemyDataAssets.IsValidIndex(Index))
	{
		return EnemyDataAssets[Index];
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[EnemyManager] Invalid StarLevel %d (EnemyDataAssets Num=%d)"),
		StarLevel,
		EnemyDataAssets.Num()
	);

	return nullptr;
}