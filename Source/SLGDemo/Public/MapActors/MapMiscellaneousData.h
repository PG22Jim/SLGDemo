// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

struct SquareData
{
protected:

	FVector2D RowColumn;
	FVector SquarePosition;
	
public:
	// default constructor
	SquareData()
	{
		RowColumn = FVector2D(0,0), SquarePosition = FVector(0,0,0);
	}

	// constructor with parameter
	SquareData(int32 NewRow, int32 NewColumn, FVector NewPos)
	{
		RowColumn = FVector2D(NewRow, NewColumn);
		SquarePosition = NewPos;
	}
	
	// getter and setter
	FVector2D GetRowAndColumn() const {return RowColumn;}
	FVector GetSquarePosition() const {return SquarePosition;}
	
	void SetRowAndColumn(FVector2D NewRowColumn) {RowColumn = NewRowColumn;}
	void SetSquarePosition(FVector NewPos) {SquarePosition = NewPos;}
};


UENUM(BlueprintType)
enum class MapSquareCondition: uint8
{
	Initial = 0 UMETA(DisplayName = "Initial"),
	Available = 1 UMETA(DisplayName = "Available"),
	Unavailable = 2 UMETA(DisplayName = "Unavailable"),
	Normal = 3 UMETA(DisplayName = "Normal"),
	AbleAttack = 4 UMETA(DisplayName = "Attackable"),
};