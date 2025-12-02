// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class TPS_WAVE_DEFENSEPR_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// 이 액터의 속성에 대한 기본값을 설정. Sets default values for this actor's properties
	ABullet();

protected:
	// 게임이 시작되거나 스폰할 때 호출됨. Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// 매 프레임마다 호출됨. Called every frame
	virtual void Tick(float DeltaTime) override;

	// 발사체의 이동을 담당할 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* movementComp;
	// 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Collision)
	class USphereComponent* collisionComp;
	// 외관 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = BodyMesh)
	class UStaticMeshComponent* bodyMeshComp;

	// 총알 제거 함수
	void Die();

	// 총알 속도
	UPROPERTY(EditAnywhere, Category = Settings)
	float speed = 5000;
};
