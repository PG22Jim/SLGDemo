// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapManager.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/Actor.h"
#include "SLGDemo/SLGDemoPlayerController.h"
#include "GameManager.generated.h"

UCLASS()
class SLGDEMO_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//

	UPROPERTY()
	APlayerActor* PlayerActorRef;

	UPROPERTY()
	ASLGDemoPlayerController* PlayerControllerRef;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ABaseCharacter*> PlayerControllingCharacterList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ABaseCharacter*> PlayerAvailableCharacterList;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ABaseCharacter*> EnemyControllingCharacterList;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Searching Class")
	TSubclassOf<ABaseCharacter>  SearchingBaseCharacterClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Searching Class")
	TSubclassOf<APlayerActor>  SearchingPlayerActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AMapManager> MapManagerClass;

	UPROPERTY()
	AMapManager* MapManagerRef;

	UPROPERTY()
	TArray<AMapPositionSquare*> CurrentReachableSquares;

	
	TMap<CharacterClass, TArray<FCharacterAbility>> AbilitiesForEachClass;
	
	UPROPERTY()
	APostProcessVolume* PostProcessVolumeRef;

	void PreGameStart();

	TArray<FCharacterAbility> GetAbilitiesByClass(CharacterClass RequestClass);

	
	
	void OnGameStart();

	void OnPlayerTurnBegin();

	void OnPlayerSuccessfullySwitchCharacter();

	UFUNCTION()
	void OnPlayerFinishCurrentCharacterAction();

	UFUNCTION()
	AMapPositionSquare* CheckStandingSquareOccupied(ABaseCharacter* CheckingCharacter);
	
	UFUNCTION()
	bool IsSquareReachable(FString RequestSquareName);
	
	void TrySetPlayerControllingCharacter(int32 SetIndex);

	bool TryCachePlayerActor();
	bool TryCachePlayerController();

	void TryAssignPlayerDelegate();

	void TryAllocateAllBaseCharactersOnMap();
	
	bool CharacterListIndexCheck(int32 SelectingIndex, bool IsPlayerList);

	void InitializeMap();

	bool TryCacheMapManager();
	
	bool TrySpawnMapManager();

	bool TryCachePostProcessVolume();

	void ClearAllPlayerCharacterOutline();

	void ClearSingleCharacterOutline(ABaseCharacter* ChangingCharacter);
	
	void ChangeCharacterAvailability(ABaseCharacter* ChangingCharacter, CharacterAvailability SettingAvailability);

	UFUNCTION()
	void ExecuteNextOrPreviousKey(bool IsNext);
	
	void TryGetNextAvailablePlayerCharacter();
	
	void TryGetPreviousAvailablePlayerCharacter();

	TArray<FCharacterAbility> GetCharacterAbilityByClass(CharacterClass SearchingClass);

	UFUNCTION()
	int32 GetAvailableCharacterNum() {return PlayerAvailableCharacterList.Num();}

	UFUNCTION()
	void OnTryToEndPlayerTurn();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	void DebugMenuDisplay(float DeltaTime);

};
