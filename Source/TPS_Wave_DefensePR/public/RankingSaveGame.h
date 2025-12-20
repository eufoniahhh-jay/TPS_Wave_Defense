// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RankEntry.h"
#include "RankingSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class TPS_WAVE_DEFENSEPR_API URankingSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	URankingSaveGame();

	// 저장되는 Top 랭킹 목록 (최대 10개 유지)
	UPROPERTY(BlueprintReadWrite, Category = "Ranking")
	TArray<FRankEntry> TopRanks;

	// (선택) 저장 슬롯 이름/인덱스 기본값
	UPROPERTY(BlueprintReadWrite, Category = "Ranking")
	FString SlotName = TEXT("RankingSlot");

	UPROPERTY(BlueprintReadWrite, Category = "Ranking")
	int32 UserIndex = 0;

public:
	// 새 기록을 추가하고(Insert), 정렬 후 TopN으로 자르는 유틸
	UFUNCTION(BlueprintCallable, Category = "Ranking")
	void AddEntryAndSort(const FRankEntry& NewEntry, int32 MaxEntries = 10);

	// 디버그용 출력(선택)
	UFUNCTION(BlueprintCallable, Category = "Ranking")
	void DebugPrintRanks() const;

public:
	void SortRanks();
	void TrimRanks(int32 MaxEntries);
	// 상위 10등 중 몇위인지
	int32 FindEntryRank(const FRankEntry& Entry);
};
