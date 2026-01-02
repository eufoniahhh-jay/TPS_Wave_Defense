// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TPS_WAVE_DEFENSEPR_API UEnemyDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 외형
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<USkeletalMesh> SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TSubclassOf<UAnimInstance> AnimBlueprint = nullptr;

	// 기본 스탯(종족 기본값)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float BaseHP = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float BaseAttackRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float BaseAttackDelay = 3.0f;

	// (선택) 이동속도도 베이스로 두고 싶다면 추가
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	//float BaseMoveSpeed = 450.f;
};
