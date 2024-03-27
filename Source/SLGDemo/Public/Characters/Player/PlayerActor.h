// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "PlayerActor.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestNextOrPrevious, bool, IsNext);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FOnCheckPointReachable, FString, RequestName);

UCLASS()
class SLGDEMO_API APlayerActor : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AssignMovingStartEnd();

	FTimerHandle StandingSquareCheckTimerHandle;
	
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABaseCharacter* ControllingCharacterRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SelectingIndex = 0;

	UPROPERTY()
	FTimeline CameraQuickMovementTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TimeStopSetting)
	UCurveFloat* CameraQuickMovementCurveFloat;

	FVector MovingStartPosition = FVector(0,0,0);
	FVector MovingEndPosition = FVector(0,0,0);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void TickCheckingStandingSquare();

	bool CheckSquareWithDirection(FVector CheckingDirection);
	bool CheckSquareByPosition(FVector DecalCompLocation);
	
	void OverwriteControllingCharacterRef(ABaseCharacter* NewCharacter);


	UFUNCTION()
	void OnRequestNextOrPrevious(bool IsNext);

	void OnBeginTickStandingCheck();


	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION()
	void OnCameraMovementUpdate(float Value);
	

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }


	ABaseCharacter* GetControllingCharacter() const {return ControllingCharacterRef;}
	void SetControllingCharacter(ABaseCharacter* SettingCharacter) {ControllingCharacterRef = SettingCharacter;}
	
	int32 GetSelectingIndex() const {return SelectingIndex;}
	void SetSelectingIndex(int32 NewIndex) {SelectingIndex = NewIndex;}



	// Delegate
	FOnRequestNextOrPrevious OnRequestNextOrPreviousSignature;
	FOnCheckPointReachable OnCheckPointReachableDelegate;
};
