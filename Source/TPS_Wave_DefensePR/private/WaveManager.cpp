// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveManager.h"
#include <Kismet/GameplayStatics.h>
#include "EnemyManager.h"
#include "Enemy.h"

// Sets default values
AWaveManager::AWaveManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = false;

    UE_LOG(LogTemp, Warning,
        TEXT("[WaveManager CONSTRUCT] %s | bUsePerSpawnDifficulty=%d"),
        *GetName(), bUsePerSpawnDifficulty
    );
}

// Called when the game starts or when spawned
void AWaveManager::BeginPlay()
{
	Super::BeginPlay();
	
    // 아직 웨이브 시작 로직은 없으니, "대기 상태"에서 남은 시간을 0으로 유지
    WaveState = EWaveState::Waiting;
    RemainingTime = 0.f;

    if (bDebugLog)
    {
        UE_LOG(LogTemp, Log, TEXT("[WaveManager] BeginPlay | Stage=%d | Duration=%.1f | State=%s"),
            CurrentStage,
            WaveDuration,
            *UEnum::GetValueAsString(WaveState));
    }
    
    //웨이브매니저 중복생성인지 확인용 로그
    UE_LOG(LogTemp, Warning,
        TEXT("[WaveManager BeginPlay] %s | bUsePerSpawnDifficulty=%d"),
        *GetName(), bUsePerSpawnDifficulty
    );

    // enemyManager 관련
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyManager::StaticClass(), Found);

    if (Found.Num() > 0)
    {
        EnemyManager = Cast<AEnemyManager>(Found[0]);
    }

    // 테스트용 (나중에 제거)
    StartWave();
}

// Called every frame
void AWaveManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWaveManager::StartWave()
{
    if (WaveState == EWaveState::InWave)
        return;

    WaveState = EWaveState::InWave;
    RemainingTime = WaveDuration;

    // kill ocunt 테스트 로그
    //RegisterKill(nullptr);

    // enemy Difficulty 테스트 로그 - 스테이지 당 가능한 enemy
    //auto Available = GetAvailableDifficulties();
    //FString Debug;
    //for (const auto& D : Available) {
    //    //Debug += FString::Printf(TEXT("★%d "), D.StarLevel);
    //    Debug += FString::Printf(TEXT("[Star:%d] "), D.StarLevel);
    //}
    //UE_LOG(LogTemp, Log, TEXT("[Stage %d] Allowed Difficulties: %s"),
    //    CurrentStage, *Debug);
    //

    // enemy Difficulty 테스트 로그 2 - 스테이지 당 enemy 선택 빈도 확인
    auto Available = GetAvailableDifficulties();
    FEnemyDifficulty Selected = SelectDifficultyByWeight(Available);
    UE_LOG(LogTemp, Log,
        TEXT("[Stage %d] Selected Difficulty: Star=%d (Weight=%.1f)"),
        CurrentStage,
        Selected.StarLevel,
        Selected.SpawnWeight
    );
    //

    if (EnemyManager) {
        EnemyManager->StartSpawning();
    }

    if (bDebugLog) {
        UE_LOG(LogTemp, Log,
            TEXT("[WaveManager] Wave START | Stage=%d | Duration=%.1f"),
            CurrentStage,
            WaveDuration
        );
    }

    // Broadcast. 시작 이벤트
    OnWaveStart.Broadcast(CurrentStage, WaveDuration);

    // 1초 타이머 시작
    GetWorldTimerManager().SetTimer(
        WaveTimerHandle,
        this,
        &AWaveManager::UpdateWave,
        1.0f,
        true
    );
}

void AWaveManager::UpdateWave()
{
    RemainingTime -= 1.0f;

    UE_LOG(LogTemp, Log, TEXT("[WaveManager] Wave Tick | Stage=%d | Remaining=%.1f"),
        CurrentStage, RemainingTime);

    // Tick 이벤트
    OnWaveTick.Broadcast(CurrentStage, RemainingTime);

    if (RemainingTime <= 0.0f)
    {
        EndWave();
    }
}


void AWaveManager::EndWave()
{
    GetWorldTimerManager().ClearTimer(WaveTimerHandle);

    WaveState = EWaveState::WaveEnd;

    if (bDebugLog)
    {
        UE_LOG(LogTemp, Log,
            TEXT("[WaveManager] Wave END | Stage=%d"),
            CurrentStage
        );
    }

    //
    // Enemy 정리
    /*TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), Enemies);

    for (AActor* Enemy : Enemies)
    {
        Enemy->Destroy();
    }*/
    if (EnemyManager)
    {
        EnemyManager->StopSpawning();
        EnemyManager->ClearAllEnemies();
    }
    //

    // Broadcast
    OnWaveEnd.Broadcast(CurrentStage);

    // 다음 스테이지 준비
    // CurrentStage++;
    WaveState = EWaveState::Waiting;
}

void AWaveManager::StartNextWave()
{
    CurrentStage++;
    /*
    //RemainingTime = WaveDuration;
    //WaveState = EWaveState::InWave;

    //UE_LOG(LogTemp, Log, TEXT("[WaveManager] Start Next Wave | Stage=%d"), CurrentStage);

    ////
    //OnWaveStart.Broadcast(CurrentStage, WaveDuration);

    //GetWorld()->GetTimerManager().SetTimer(
    //    WaveTimerHandle,
    //    this,
    //    &AWaveManager::UpdateWave,
    //    1.0f,
    //    true
    //);
    //
    */

    StartWave(); // 기존 웨이브 시작 함수
}

bool AWaveManager::IsInWave()
{
    return WaveState == EWaveState::InWave;
}

TArray<FEnemyDifficulty> AWaveManager::GetAvailableDifficulties()
{
    TArray<FEnemyDifficulty> Result;

    for (const FEnemyDifficulty& Diff : DifficultyTable)
    {
        if (IsDifficultyAllowedForStage(Diff.StarLevel))
        {
            Result.Add(Diff);
        }
    }

    return Result;
}

bool AWaveManager::IsDifficultyAllowedForStage(int32 StarLevel)
{
    if (CurrentStage <= 3)
    {
        return StarLevel == 1;
    }
    else if (CurrentStage <= 6)
    {
        return StarLevel <= 2;
    }
    else if (CurrentStage <= 10)
    {
        return StarLevel >= 2 && StarLevel <= 3;
    }
    else if (CurrentStage <= 15)
    {
        return StarLevel >= 3 && StarLevel <= 4;
    }
    else
    {
        return StarLevel >= 3; // ★3~★5
    }
}

FEnemyDifficulty AWaveManager::SelectDifficultyByWeight(const TArray<FEnemyDifficulty>& Candidates) {
    // 안전 장치
    if (Candidates.Num() == 0)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[WaveManager] SelectDifficultyByWeight called with EMPTY Candidates")
        );

        return FEnemyDifficulty(); // 기본값 (★1)
    }

    // 1. 전체 가중치 합
    float TotalWeight = 0.f;
    for (const FEnemyDifficulty& Diff : Candidates)
    {
        TotalWeight += Diff.SpawnWeight;
    }

    // 2. 랜덤 값 생성
    const float RandValue = FMath::FRandRange(0.f, TotalWeight);

    // 3. 누적 가중치 비교
    float AccWeight = 0.f;
    for (const FEnemyDifficulty& Diff : Candidates)
    {
        AccWeight += Diff.SpawnWeight;

        if (RandValue <= AccWeight)
        {
            return Diff;
        }
    }

    // 4. 이론상 도달하면 안 되지만, 안전하게 마지막 반환
    return Candidates.Last();
}

FEnemyDifficulty AWaveManager::GetDifficultyForSpawn()
{
    /*if (bUsePerSpawnDifficulty)
    {
        auto Allowed = GetAvailableDifficulties();
        return SelectDifficultyByWeight(Allowed);
    }

    // 지금은 항상 CurrentWaveDifficulty 반환 (bUsePerSpawnDifficulty를 false로 해뒀으니)
    return CurrentWaveDifficulty;*/

    UE_LOG(LogTemp, Warning,
        TEXT("[GetDifficultyForSpawn] bUsePerSpawnDifficulty=%d"),
        bUsePerSpawnDifficulty
    );

    if (bUsePerSpawnDifficulty)
    {
        auto Allowed = GetAvailableDifficulties();

        UE_LOG(LogTemp, Warning,
            TEXT("[GetDifficultyForSpawn] AllowedCount=%d"),
            Allowed.Num()
        );

        for (auto& D : Allowed)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("  - Candidate Star=%d Weight=%.1f"),
                D.StarLevel, D.SpawnWeight
            );
        }

        auto Result = SelectDifficultyByWeight(Allowed);

        UE_LOG(LogTemp, Warning,
            TEXT("[GetDifficultyForSpawn] Selected Star=%d"),
            Result.StarLevel
        );

        return Result;
    }

    return CurrentWaveDifficulty;
}

float AWaveManager::GetSpawnInterval()
{
    float Min, Max;

    if (CurrentStage <= 3)
    {
        Min = 2.5f; Max = 3.5f;
    }
    else if (CurrentStage <= 6)
    {
        Min = 2.0f; Max = 3.0f;
    }
    else if (CurrentStage <= 10)
    {
        Min = 1.5f; Max = 2.5f;
    }
    else
    {
        Min = 1.0f; Max = 2.0f;
    }

    return FMath::FRandRange(Min, Max);
}

void AWaveManager::RegisterKill(AEnemy* DeadEnemy)
{
    KillCount++;

    Score += CalculateScore(DeadEnemy);

    // UI에 알림(브로드캐스트)
    OnScoreUpdated.Broadcast(KillCount, Score);

    UE_LOG(LogTemp, Log,
        TEXT("[WaveManager] Kill Registered | Kill: %d | Score: %d"),
        KillCount, Score
    );
}

int32 AWaveManager::CalculateScore(AEnemy* DeadEnemy)
{
    if (!DeadEnemy)
        return 0;

    // 예시: star 1~5 기반
    int32 StarLevel = 1;

    // 네 Enemy 구조에 맞게 수정
    if (DeadEnemy->StarLevel > 0)
    {
        StarLevel = DeadEnemy->StarLevel;
    }

    return 100 * StarLevel;
}

void AWaveManager::HandleGameOver()
{
    FinalStage = CurrentStage;
    FinalKillCount = KillCount;
    FinalScore = Score;

    //bIsWaveActive = false;

    UE_LOG(LogTemp, Warning,
        TEXT("[WaveManager] GameOver | Stage: %d | Kill: %d | Score: %d"),
        FinalStage, FinalKillCount, FinalScore
    );

    // TODO (다음 단계):
    // - EnemyManager 스폰 중단
    // - GameOver UI 표시 (BP 이벤트)

    OnGameOver.Broadcast(FinalStage, FinalScore, FinalKillCount);
}
