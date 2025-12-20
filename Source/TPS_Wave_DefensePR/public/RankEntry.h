// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RankEntry.generated.h"

/**
 RankEntry는 월드에 존재하지 않는 순수 데이터이기 때문에
 Actor나 UObject가 아닌 USTRUCT로 정의
 */
USTRUCT(BlueprintType)
struct FRankEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite)
	int32 Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Stage;

	UPROPERTY(BlueprintReadWrite)
	int32 KillCount;
};

class TPS_WAVE_DEFENSEPR_API RankEntry
{
public:
	RankEntry();
	~RankEntry();
};
