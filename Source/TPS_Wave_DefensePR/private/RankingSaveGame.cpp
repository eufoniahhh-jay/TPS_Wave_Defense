// Fill out your copyright notice in the Description page of Project Settings.


#include "RankingSaveGame.h"
#include "Kismet/KismetSystemLibrary.h" // DebugPrint 
#include "Engine/Engine.h"              // UE_LOG

URankingSaveGame::URankingSaveGame()
{
	// 필요하면 기본값 초기화
}

void URankingSaveGame::AddEntryAndSort(const FRankEntry& NewEntry, int32 MaxEntries)
{
	TopRanks.Add(NewEntry);

	SortRanks();
	TrimRanks(MaxEntries);
}

void URankingSaveGame::SortRanks()
{
	TopRanks.Sort([](const FRankEntry& A, const FRankEntry& B)
		{
			// 1) Score 내림차순
			if (A.Score != B.Score)
			{
				return A.Score > B.Score;
			}

			// 2) Score 같으면 Stage 내림차순
			if (A.Stage != B.Stage)
				return A.Stage > B.Stage;

			// 3) Stage 같으면 KillCount 내림차순
			if (A.KillCount != B.KillCount)
			{
				return A.KillCount > B.KillCount;
			}

			// 4) 그 외에는 순서 유지 (정렬 안정성은 UE Sort가 안정정렬이 아닐 수 있어도,
			//    동점이면 false 반환으로 "상대적으로" 큰 변화가 줄어드는 효과)
			return false;
		});
}

void URankingSaveGame::TrimRanks(int32 MaxEntries)
{
	if (MaxEntries <= 0) return;

	if (TopRanks.Num() > MaxEntries)
	{
		TopRanks.SetNum(MaxEntries);
	}
}

void URankingSaveGame::DebugPrintRanks() const
{
	for (int32 i = 0; i < TopRanks.Num(); ++i)
	{
		/*const FRankEntry& E = TopRanks[i];
		UE_LOG(LogTemp, Log, TEXT("[Rank %02d] %s | Score=%d | Kill=%d"),
			i + 1, *E.PlayerName, E.Score, E.KillCount);*/

		const FRankEntry& Entry = TopRanks[i];
		UE_LOG(LogTemp, Warning,
			TEXT("[%d] %s | Score=%d | Stage=%d | Kill=%d"),
			i + 1,
			*Entry.PlayerName,
			Entry.Score,
			Entry.Stage,
			Entry.KillCount
		);
	}
}

int32 URankingSaveGame::FindEntryRank(const FRankEntry& Entry)
{
	for (int32 i = 0; i < TopRanks.Num(); ++i)
	{
		if (TopRanks[i].Score == Entry.Score &&
			TopRanks[i].Stage == Entry.Stage &&
			TopRanks[i].KillCount == Entry.KillCount)
		{
			return i + 1; // 1등부터
		}
	}
	return -1;
}