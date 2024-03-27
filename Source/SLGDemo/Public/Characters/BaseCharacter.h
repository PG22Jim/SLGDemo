// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterEnum.h"
#include "CharacterStats.h"
#include "GameFramework/Character.h"
#include "MapActors/MapPositionSquare.h"
#include "BaseCharacter.generated.h"

UCLASS(Blueprintable)
class SLGDEMO_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Team)
	CharacterTeam BelongingTeam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Debug)
	CharacterClass DebugClass;

	UPROPERTY()
	AMapPositionSquare* InitialSquare;

	CharacterStats CurrentCharacterStats = CharacterStats();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	CharacterTeam GetBelongingTeam() const {return BelongingTeam;}

	AMapPositionSquare* GetInitialSquare() const {return InitialSquare;}
	void SetInitialSquare(AMapPositionSquare* NewSquare) {InitialSquare = NewSquare;}


	CharacterStats GetCurrentCharacterStats() const {return CurrentCharacterStats;}
};
