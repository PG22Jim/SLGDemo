// Copyright © 2024 by Jim Chen. All rights reserved.


#include "Managers/MapManager.h"

#include <complex.h>

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMapManager::AMapManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMapManager::BeginPlay()
{
	Super::BeginPlay();
	if(InitializeMapActors())
	{
		
	}	
}

// Called every frame
void AMapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AMapManager::InitializeMapActors()
{
	UWorld* World = GetWorld();
	if(!World) return false;

	for (int32 r = 0; r < Row; r++)
	{
		for (int32 c = 0; c < Column; c++)
		{
			
			// Spawning Map Square
			// set spawn parameter
			FActorSpawnParameters MapManagerSpawnParams;
			MapManagerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// get Map Square's spawn location and rotation
			FRotator SpawnRotation = GetActorRotation();

			// Get X, Y to form Spawn Location
			const float SpawnY = SpawnPosOrigin.Y + (r * SpawnRowDistance);
			const float SpawnX = SpawnPosOrigin.X - (c * SpawnColumnDistance);
			const FVector SpawnLocation = FVector(SpawnX, SpawnY, SpawnPosOrigin.Z);
			
			// spawn Map Square
			AMapPositionSquare* SpawnResult = World->SpawnActor<AMapPositionSquare>(MapSquareClass, SpawnLocation, SpawnRotation);
			if(!SpawnResult) return false;
			
			SquareData SpawnSquareData = SquareData(r,c, SpawnLocation);
			SpawnResult->SetStoringMapSquareData(SpawnSquareData);
			

			MapGrid.Add(SpawnSquareData.GetRowAndColumn(), SpawnResult);
		}
	}

	return true;
}

AMapPositionSquare* AMapManager::GetSpecificSquareByRowColumn(FVector2D RequestGridKey)
{
	AMapPositionSquare* DataResult = MapGrid.FindRef(RequestGridKey);
	return DataResult;
}

// TODO: Add CharacterTeam and AttackRange into Parameter 
TArray<AMapPositionSquare*> AMapManager::GetAllAvailableSquares(int32 SelectedCharacterMovement,
	AMapPositionSquare* InitialSquare)
{
	TArray<AMapPositionSquare*> ReturnArray;
	FVector2D InitialSquareRowColumn = InitialSquare->GetStoringMapSquareData().GetRowAndColumn();

	// TODO: Find more optimized way to iterate
	for(const TPair<FVector2D, AMapPositionSquare*>& IterateMapPair : MapGrid)
	{
		if(AMapPositionSquare* IterateSquare = IterateMapPair.Value)
			if(IsSquareReachable(InitialSquareRowColumn, IterateMapPair.Key, SelectedCharacterMovement))
			{
				// TODO: Optimize more
				if(IsSquareContainEnemy(IterateSquare, CharacterTeam::Player))
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));

					IterateSquare->SetMapSquareColorByCondition(MapSquareCondition::AbleAttack);
					continue;
				}
				IterateSquare->SetMapSquareColorByCondition(MapSquareCondition::Available);
				ReturnArray.Add(IterateSquare);
			}
			else
			{
				if(IsSquareContainEnemy(IterateSquare, CharacterTeam::Player))
				{
					//
					if(IsSquareAttackable(InitialSquareRowColumn, IterateMapPair.Key, SelectedCharacterMovement, 1))
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));

						IterateSquare->SetMapSquareColorByCondition(MapSquareCondition::AbleAttack);
						continue;
					}
				}
				
				IterateSquare->SetMapSquareColorByCondition(MapSquareCondition::Normal);
			}
	}

	
	return ReturnArray;
}

bool AMapManager::IsSquareReachable(FVector2D InitialSquarePos, FVector2D CheckingSquarePos,
	int32 CharacterMovingPoint)
{
	int32 RowOffset = UKismetMathLibrary::Abs(InitialSquarePos.X - CheckingSquarePos.X);
	int32 ColumnOffset = UKismetMathLibrary::Abs(InitialSquarePos.Y - CheckingSquarePos.Y);
	return CharacterMovingPoint >= (RowOffset + ColumnOffset);
}

bool AMapManager::IsSquareAttackable(FVector2D InitialSquarePos, FVector2D CheckingSquarePos,
	int32 CharacterMovingPoint, int32 CharacterAttackRange)
{
	int32 RowOffset = UKismetMathLibrary::Abs(InitialSquarePos.X - CheckingSquarePos.X);
	int32 ColumnOffset = UKismetMathLibrary::Abs(InitialSquarePos.Y - CheckingSquarePos.Y);
	return CharacterMovingPoint + CharacterAttackRange >= (RowOffset + ColumnOffset);
}

bool AMapManager::IsSquareContainEnemy(AMapPositionSquare* CheckingSquare, CharacterTeam CheckingCharacterTeam)
{
	if(AActor* StoringActor = CheckingSquare->GetStoringOccupiedCharacter())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));

		if(ABaseCharacter* CastResult = Cast<ABaseCharacter>(StoringActor))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));

			return CastResult->GetBelongingTeam() != CheckingCharacterTeam; 
		}
	}
	
	return false;
}

