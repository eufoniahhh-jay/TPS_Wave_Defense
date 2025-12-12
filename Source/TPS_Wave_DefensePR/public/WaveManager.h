// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

/*WaveManager는 enum 기반 상태 머신과
Delegate 이벤트 구조를 통해
UI·AI·사운드 시스템과 느슨하게 결합된
중앙 게임 루프 컨트롤러 역할을 합니다.*/

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
	float WaveDuration = 60.f;

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

public:
	// 타이머

	// 웨이브 타이머 핸들
	FTimerHandle WaveTimerHandle;

};
