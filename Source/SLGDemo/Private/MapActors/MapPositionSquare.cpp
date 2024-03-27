// Fill out your copyright notice in the Description page of Project Settings.


#include "MapActors/MapPositionSquare.h"

// Sets default values
AMapPositionSquare::AMapPositionSquare()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MapSquareMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("Map Square Mesh Comp"));
	SetRootComponent(MapSquareMeshComp);
}

// Called when the game starts or when spawned
void AMapPositionSquare::BeginPlay()
{
	Super::BeginPlay();

	TryCacheDynamicMaterialRef();
}

bool AMapPositionSquare::TryCacheDynamicMaterialRef()
{
	if(DynamicMaterialRef) return true;
	
	if(!SourceMaterial) return false;
	
	UMaterialInstanceDynamic* ReturnMIDynamic = MapSquareMeshComp->CreateDynamicMaterialInstance(0);
	if(!ReturnMIDynamic) return false;

	DynamicMaterialRef = ReturnMIDynamic;
	return true;
}

// Called every frame
void AMapPositionSquare::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapPositionSquare::SetMapSquareColorByCondition(MapSquareCondition NewCondition)
{
	if(!TryCacheDynamicMaterialRef()) return;
	
	switch (NewCondition)
	{
	case MapSquareCondition::Initial:
		DynamicMaterialRef->SetVectorParameterValue("SquareColor", ConditionColorInitial);
		break;
	case MapSquareCondition::Available:
		DynamicMaterialRef->SetVectorParameterValue("SquareColor", ConditionColorAvailable);
		break;
	case MapSquareCondition::Unavailable:
		DynamicMaterialRef->SetVectorParameterValue("SquareColor", ConditionColorUnavailable);
		break;
	case MapSquareCondition::Normal:
		DynamicMaterialRef->SetVectorParameterValue("SquareColor", ConditionColorNormal);
		break;
	case MapSquareCondition::AbleAttack:
		DynamicMaterialRef->SetVectorParameterValue("SquareColor", ConditionColorAttacking);
		break;
	default: ;
	}
}

