// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveManager.h"

// Sets default values
AWaveManager::AWaveManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = false;

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

    if (bDebugLog)
    {
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

    // Broadcast
    OnWaveEnd.Broadcast(CurrentStage);

    // 다음 스테이지 준비
    CurrentStage++;
    WaveState = EWaveState::Waiting;
}

