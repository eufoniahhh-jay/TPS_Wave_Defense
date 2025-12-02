// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "EnhancedInputSubSystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Bullet.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스캘레탈메시 데이터를 불러오고 싶다
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
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
		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
		//gunMeshComp->SetRelativeLocation(FVector(-17, 10, -3));
		//gunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));
	}
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
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayerMove();
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput) {
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);
		// 점프 입력 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &ATPSPlayer::InputJump);
		// 총알 발사 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);
	}
}

void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerYawInput(value);
}

void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value);
}

void ATPSPlayer::Move(const struct FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	// 상하 입력 이벤트 처리
	direction.X = value.X;
	// 좌우 입력 이벤트 처리
	direction.Y = value.Y;
}

void ATPSPlayer::InputJump(const struct FInputActionValue& inputValue)
{
	Jump();
}

void ATPSPlayer::PlayerMove()
{
	//	// 플레이어 이동 처리
	// 등속 운동
	// P(결과위치) = P0(현재위치) + v(속도) X t(시간)
	direction = FTransform(GetControlRotation()).TransformVector(direction);
	/*FVector P0 = GetActorLocation();
	FVector vt = direction * walkSpeed * DeltaTime;
	FVector P = P0 + vt;
	SetActorLocation(P);*/
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void ATPSPlayer::InputFire(const FInputActionValue& inputValue)
{
	// 총알 발사 처리
	FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
}
