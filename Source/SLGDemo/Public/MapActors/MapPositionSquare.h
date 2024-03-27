

#pragma once

#include "CoreMinimal.h"
#include "MapMiscellaneousData.h"
#include "GameFramework/Actor.h"
#include "MapPositionSquare.generated.h"

UCLASS()
class SLGDEMO_API AMapPositionSquare : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapPositionSquare();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool TryCacheDynamicMaterialRef();

	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* MapSquareMeshComp;
	
	SquareData StoringMapSquareData;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	AActor* StoringOccupiedCharacter;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialRef;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= Setting)
	UMaterialInstance* SourceMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= Setting)
	FLinearColor ConditionColorInitial = FLinearColor(1,1,1,0);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= Setting)
	FLinearColor ConditionColorAvailable = FLinearColor(0,1,0,0);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= Setting)
	FLinearColor ConditionColorUnavailable = FLinearColor(1,0,0,0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= Setting)
	FLinearColor ConditionColorNormal = FLinearColor(0,0.2,1,0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= Setting)
	FLinearColor ConditionColorAttacking = FLinearColor(0,0,0,0);
	
	
	// TODO: a small way to find changing square in a efficient way

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetMapSquareColorByCondition(MapSquareCondition NewCondition);
	
	SquareData GetStoringMapSquareData() const {return StoringMapSquareData;}
	void SetStoringMapSquareData(SquareData NewSquareData) {StoringMapSquareData = NewSquareData;}


	AActor* GetStoringOccupiedCharacter() const {return StoringOccupiedCharacter;}
	void SetStoringOccupiedCharacter(AActor* NewStoringActor) {StoringOccupiedCharacter = NewStoringActor;}
};
