// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyDataAsset.h"
#include "Enemy.generated.h"

USTRUCT(BlueprintType)
struct FEnemyDifficulty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StarLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HPMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MoveSpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackSpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackRangeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnWeight = 1.0f;
};


UCLASS()
class TPS_WAVE_DEFENSEPR_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// 적 AI 관리 컴포넌트 클래스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSMComponent)
	class UEnemyFSM* fsm;

public:
	// 늘 발생하던 크래시 해결해보기 (기존의 FClassFinder로 AnimBP를 로드하는 방식을 대체하기)
	/*UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSoftClassPtr<UAnimInstance> AnimBP;*/

public:
	// difficulty 적용 함수
	void ApplyDifficulty(const FEnemyDifficulty& Difficulty);

	// Data Asset으로부터 difficulty 적용 함수
	void ApplyEnemyData(class UEnemyDataAsset* Data);

	// 시작 difficulty
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 StarLevel = 1;
};
