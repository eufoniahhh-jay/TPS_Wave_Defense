// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyFSM.h"
#include "EnemyDataAsset.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스켈레탈메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Enemy/Model/vampire_a_lusth.vampire_a_lusth'"));
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	// 1-1. 데이터 로드가 성공하면
	if (tempMesh.Succeeded()) {
		// 1-2. 데이터 할당
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		// 1-3. 메시 위치 및 회전 설정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		// 1-4. 메시 크기 수정
		GetMesh()->SetRelativeScale3D(FVector(0.84f));
	}

	// EnemyFSM 컴포넌트 추가
	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	// 애니메이션 블루프린트 할당하기 -> 자꾸 크래시 발생하는 부분. 변경해보기
	//ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/ABP_Enemy.ABP_Enemy_C'"));
	////ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("AnimBlueprint'/Game/Blueprints/ABP_Enemy.ABP_Enemy_C'"));
	//if (tempClass.Succeeded()) {
	//	GetMesh()->SetAnimInstanceClass(tempClass.Class);
	//}

	// 월드에 배치되거나 스폰될 때 자동으로 AIController부터 Possess될 수 있도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	/*if (AnimBP.IsValid())
	{
		GetMesh()->SetAnimInstanceClass(AnimBP.Get());
	}*/
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::ApplyDifficulty(const FEnemyDifficulty& Difficulty)
{
	/*fsm->hp *= Difficulty.HPMultiplier;
	fsm->attackRange *= Difficulty.AttackRangeMultiplier;
	fsm->attackDelayTime /= Difficulty.AttackSpeedMultiplier;*/
	//GetCharacterMovement()->MaxWalkSpeed *= Difficulty.MoveSpeedMultiplier;

	// StarLevel 저장 (이게 핵심)
	StarLevel = Difficulty.StarLevel;

	// 반드시 Base 기준으로 재계산
	fsm->hp = fsm->BaseHP * Difficulty.HPMultiplier;
	fsm->attackRange = fsm->BaseAttackRange * Difficulty.AttackRangeMultiplier;
	fsm->attackDelayTime = fsm->BaseAttackDelay / Difficulty.AttackSpeedMultiplier;

	UE_LOG(LogTemp, Warning,
		TEXT("[Enemy] ApplyDifficulty | Star=%d | HP=%.1d | Range=%.1f | Delay=%.2f"),
		StarLevel,
		fsm->hp,
		fsm->attackRange,
		fsm->attackDelayTime
	);
}

void AEnemy::ApplyEnemyData(UEnemyDataAsset* Data)
{
	if (!Data)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Enemy] ApplyEnemyData failed: Data is null"));
		return;
	}

	if (!fsm)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Enemy] ApplyEnemyData failed: FSM is null"));
		return;
	}

	// =========================
	// 1. Base 스탯 적용
	// =========================
	fsm->BaseHP = Data->BaseHP;
	fsm->BaseAttackRange = Data->BaseAttackRange;
	fsm->BaseAttackDelay = Data->BaseAttackDelay;

	// =========================
	// 2. (선택) 외형 적용
	// 지금은 없어도 됨
	// =========================
	if (Data->SkeletalMesh)
	{
		GetMesh()->SetSkeletalMesh(Data->SkeletalMesh);
	}

	if (Data->AnimBlueprint)
	{
		GetMesh()->SetAnimInstanceClass(Data->AnimBlueprint);

		/*UE_LOG(LogTemp, Warning,
			TEXT("[Enemy] Set AnimBP = %s"),
			*GetNameSafe(Data->AnimBlueprint)
		);*/
		UE_LOG(LogTemp, Warning, TEXT("[Enemy] After SetAnimBP AnimInstance=%s"),
			*GetNameSafe(GetMesh()->GetAnimInstance()));
	}

	UE_LOG(LogTemp, Log,
		TEXT("[Enemy] ApplyEnemyData | BaseHP=%d | BaseRange=%.1f | BaseDelay=%.2f"),
		fsm->BaseHP,
		fsm->BaseAttackRange,
		fsm->BaseAttackDelay
	);
}