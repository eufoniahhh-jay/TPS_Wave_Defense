// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMove.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include <Kismet/GameplayStatics.h>
#include "WaveManager.h"

UPlayerMove::UPlayerMove()
{
	// Tick 함수 호출되도록 처리
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerMove::BeginPlay()
{
	Super::BeginPlay();

	// 초기 속도를 걷기 속도로 설정
	moveComp->MaxWalkSpeed = walkSpeed;

	// 웨이브 매니저 관련 코드
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AWaveManager::StaticClass(),
		FoundActors
	);

	if (FoundActors.Num() > 0)
	{
		WaveManager = Cast<AWaveManager>(FoundActors[0]);
		UE_LOG(LogTemp, Warning, TEXT("[PlayerFire] WaveManager FOUND"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[PlayerFire] WaveManager NOT FOUND"));
	}
}

void UPlayerMove::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PlayerMove();
}

void UPlayerMove::SetupInputBinding(UEnhancedInputComponent* PlayerInput)
{
	PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &UPlayerMove::Turn);
	PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &UPlayerMove::LookUp);
	PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &UPlayerMove::Move);
	// 달리기 입력 이벤트 처리 함수 바인딩
	PlayerInput->BindAction(ia_Run, ETriggerEvent::Started, this, &UPlayerMove::InputRun);
	PlayerInput->BindAction(ia_Run, ETriggerEvent::Completed, this, &UPlayerMove::InputRun);
	// 점프 입력 이벤트 처리 함수 바인딩
	PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &UPlayerMove::InputJump);
}

void UPlayerMove::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerYawInput(value);
}

void UPlayerMove::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerPitchInput(value);
}

void UPlayerMove::Move(const struct FInputActionValue& inputValue)
{
	if (!CanProcessInput())
		return;

	FVector2D value = inputValue.Get<FVector2D>();
	// 상하 입력 이벤트 처리
	direction.X = value.X;
	// 좌우 입력 이벤트 처리
	direction.Y = value.Y;
}

void UPlayerMove::PlayerMove()
{
	if (!CanProcessInput())
		return;

	//	// 플레이어 이동 처리
	// 등속 운동
	// P(결과위치) = P0(현재위치) + v(속도) X t(시간)
	direction = FTransform(me->GetControlRotation()).TransformVector(direction);
	/*FVector P0 = GetActorLocation();
	FVector vt = direction * walkSpeed * DeltaTime;
	FVector P = P0 + vt;
	SetActorLocation(P);*/
	me->AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void UPlayerMove::InputRun()
{
	auto movement = me->GetCharacterMovement();

	// 현재 달리기 모드라면
	if (movement->MaxWalkSpeed > walkSpeed) {
		// 걷기 속도로 전환
		movement->MaxWalkSpeed = walkSpeed;
	}
	else {
		movement->MaxWalkSpeed = runSpeed;
	}
}

void UPlayerMove::InputJump(const FInputActionValue& inputValue)
{
	me->Jump();
}

bool UPlayerMove::CanProcessInput() const
{
	return WaveManager && WaveManager->IsInWave();
}