// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

// 이건 없어도..?
class AEnemy;
class AWaveManager;
class UEnemyDataAsset;

UCLASS()
class TPS_WAVE_DEFENSEPR_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 랜덤 시간 간격 최솟값
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	float minTime = 1;
	// 랜덤 시간 간격 최댓값
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	float maxTime = 5;
	// 스폰할 위치 정보 배열
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TArray<class AActor*> spawnPoints;
	// AEnemy 타입의 블루프린트 할당받을 변수
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TSubclassOf<class AEnemy> enemyFactory;

	// 스폰을 위한 알람 타이머
	FTimerHandle spawnTimerHandle;

	// 적 생성 함수
	void CreateEnemy();

	// 스폰할 위치 동적 찾아 할당하기
	void FindSpawnPoints();

public:
	// 웨이브 종료 후 enemy 제거를 위함
	void ClearAllEnemies();
	// 스포닝 시작/종료 (웨이브 타이밍에 맞춰서)
	void StartSpawning();
	void StopSpawning();

	UPROPERTY()
	class AWaveManager* WaveManager;

public:
	// Enemy Data 관리

	// StarLevel(1~5)에 대응하는 EnemyDataAsset 배열
	// [0] = Star1, [1] = Star2, ...
	UPROPERTY(EditAnywhere, Category = "Enemy|Data")
	TArray<TObjectPtr<UEnemyDataAsset>> EnemyDataAssets;

	// StarLevel로 EnemyDataAsset 가져오기
	UEnemyDataAsset* GetEnemyDataByStar(int32 StarLevel) const;
};
