// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.h"
#include "WaveManager.generated.h"

/*WaveManager는 enum 기반 상태 머신과
Delegate 이벤트 구조를 통해
UI·AI·사운드 시스템과 느슨하게 결합된
중앙 게임 루프 컨트롤러 역할을 합니다.*/

class AEnemyManager;

// ---- Delegate Declarations ----
// 웨이브 시작 (Stage 번호 + 제한 시간)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnWaveStart,
	int32, Stage,
	float, Duration
);

// 웨이브 진행 중 Tick (Stage 번호 + 남은 시간)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnWaveTick,
	int32, Stage,
	float, RemainingTime
);

// 웨이브 종료 (Stage 번호)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnWaveEnd,
	int32, Stage
);

// 킬 스코어 업데이트 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnScoreUpdated,
	int32, KillCount,
	int32, Score
);

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	Waiting     UMETA(DisplayName = "Waiting"),      // 웨이브 시작 전(휴식/대기)
	InWave      UMETA(DisplayName = "In Wave"),      // 웨이브 진행 중
	WaveEnd     UMETA(DisplayName = "Wave End"),     // 웨이브 종료 처리 중(정산/연출)
	GameOver    UMETA(DisplayName = "Game Over")     // 게임 오버
};

UCLASS()
class TPS_WAVE_DEFENSEPR_API AWaveManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaveManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 핵심 데이터(1단계: 구조만)
	// 1부터 시작 (Stage 1, 2, 3...)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave|Stage", meta = (ClampMin = "1"))
	int32 CurrentStage = 1;

	// 웨이브 제한 시간(초). 기본 60초
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave|Timer", meta = (ClampMin = "1.0"))
	float WaveDuration = 10.f;

	// 현재 웨이브 남은 시간(초) - 웨이브 시작 시 WaveDuration으로 세팅될 예정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave|Timer")
	float RemainingTime = 0.f;

	// 현재 웨이브 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave|State")
	EWaveState WaveState = EWaveState::Waiting;

	// 디버그 로그 출력 on/off
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave|Debug")
	bool bDebugLog = true;

public:
	// ---- (선택) UI/다른 시스템에서 읽기 쉽게 Getter 제공 ----
	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetCurrentStage() const { return CurrentStage; }

	UFUNCTION(BlueprintPure, Category = "Wave")
	float GetWaveDuration() const { return WaveDuration; }

	UFUNCTION(BlueprintPure, Category = "Wave")
	float GetRemainingTime() const { return RemainingTime; }

	UFUNCTION(BlueprintPure, Category = "Wave")
	EWaveState GetWaveState() const { return WaveState; }

public:
	// ---- Wave Events (for UI / BP) ----

	UPROPERTY(BlueprintAssignable, Category = "Wave|Event")
	FOnWaveStart OnWaveStart;

	UPROPERTY(BlueprintAssignable, Category = "Wave|Event")
	FOnWaveTick OnWaveTick;

	UPROPERTY(BlueprintAssignable, Category = "Wave|Event")
	FOnWaveEnd OnWaveEnd;

public:
	// ---- Wave Control ----
	// 웨이브 흐름 제어
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartWave();

	UFUNCTION()
	void UpdateWave();   // 1초마다 호출

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void EndWave();

	UFUNCTION(BlueprintCallable)
	void StartNextWave();


public:
	// 타이머

	// 웨이브 타이머 핸들
	FTimerHandle WaveTimerHandle;

public:
	// 현재 웨이브 상태 체크
	UFUNCTION(BlueprintCallable)
	bool IsInWave();

public:
	// 웨이브 종료시 enemy 제거 위함
	UPROPERTY()
	AEnemyManager* EnemyManager;

public:
	// enemy Difficulty 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty")
	TArray<FEnemyDifficulty> DifficultyTable;
	// Difficulty 변환 함수
	TArray<FEnemyDifficulty> GetAvailableDifficulties();

	bool IsDifficultyAllowedForStage(int32 StarLevel);

	FEnemyDifficulty SelectDifficultyByWeight( const TArray<FEnemyDifficulty>& Candidates);

	// 현재 웨이브의 기본 난이도
	UPROPERTY()
	FEnemyDifficulty CurrentWaveDifficulty;

	// (미래 확장용) 스폰 단위 난이도 사용 여부
	// 지금은 false 고정하고, 나중에 true로 바꿔주면 확장 가능
	UPROPERTY(EditAnywhere, Category = "Difficulty")
	bool bUsePerSpawnDifficulty = true;

	// Enemy에게 줄 difficulty 요청 함수
	FEnemyDifficulty GetDifficultyForSpawn();

public:
	// 언제 스폰할지 판단하는 함수
	float GetSpawnInterval();

public:
	// Kill & Score System 
	UPROPERTY(BlueprintAssignable, Category = "Wave|Score")
	FOnScoreUpdated OnScoreUpdated;

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int32 KillCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int32 Score = 0;

	// GameOver 시 기록용
	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int32 FinalStage = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int32 FinalKillCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int32 FinalScore = 0;

	// Enemy 사망 시 호출
	UFUNCTION()
	void RegisterKill(class AEnemy* DeadEnemy);

	// 점수 계산
	int32 CalculateScore(class AEnemy* DeadEnemy);

	// GameOver 처리
	void HandleGameOver();

public:
	// score, stage, kill getter함수
	int32 GetFinalScore() const { return FinalScore; };
	int32 GetFinalStage() const { return FinalStage; };
	int32 GetFinalKill() const { return FinalKillCount; };
	

};
