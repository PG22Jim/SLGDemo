// Copyright © 2024 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Engine/PostProcessVolume.h"
#include "GameFramework/Actor.h"
#include "MapActors/MapPositionSquare.h"
#include "MapManager.generated.h"

UCLASS()
class SLGDEMO_API AMapManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsSquareReachable(FVector2D InitialSquarePos, FVector2D CheckingSquarePos, int32 CharacterMovingPoint);
	bool IsSquareAttackable(FVector2D InitialSquarePos, FVector2D CheckingSquarePos, int32 CharacterMovingPoint, int32 CharacterAttackRange);
	bool IsSquareContainEnemy(AMapPositionSquare* CheckingSquare, CharacterTeam CheckingCharacterTeam);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AMapPositionSquare> MapSquareClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= SquareSpawnSetting)
	int32 Row = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= SquareSpawnSetting)
	int32 Column = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= SquareSpawnSetting)
	FVector SpawnPosOrigin = FVector(0,0,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= SquareSpawnSetting)
	float SpawnColumnDistance = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= SquareSpawnSetting)
	float SpawnRowDistance = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= SquareSpawnSetting)
	float SpawnZ = 320.0f;

	UPROPERTY()
	TMap<FVector2D, AMapPositionSquare*> MapGrid;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	bool InitializeMapActors();

	AMapPositionSquare* GetSpecificSquareByRowColumn(FVector2D RequestGridKey);

	TArray<AMapPositionSquare*> GetAllAvailableSquares(int32 SelectedCharacterMovement, AMapPositionSquare* InitialSquare);
};
