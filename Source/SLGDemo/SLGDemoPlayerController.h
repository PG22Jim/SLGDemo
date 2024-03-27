// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/PlayerActor.h"
#include "GameFramework/PlayerController.h"
#include "SLGDemoPlayerController.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal(int32, FOnGetControlCharacterCount);
DECLARE_DYNAMIC_DELEGATE(FOnTryEndPlayerTurn);
DECLARE_DYNAMIC_DELEGATE(FOnTryFinishCharacterAction);



UCLASS()
class ASLGDemoPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASLGDemoPlayerController();

protected:

	bool AvailableToControl = true;
	
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CameraMovement = 40.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerActor* PlayerActorRef;

	bool TryCachePlayerActor();

	virtual void BeginPlay() override;

	
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface
	
	
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	UFUNCTION()
	void TryExecuteNextKey();

	UFUNCTION()
	void TryExecutePreviousKey();

	UFUNCTION()
	void TryMoveForward(float Value);
	
	UFUNCTION()
	void TryMoveRight(float Value);

	UFUNCTION()
	void TryCameraMoveForward(float Value);
	
	UFUNCTION()
	void TryCameraMoveRight(float Value);

	UFUNCTION()
	void TryEndPlayerTurn();

	UFUNCTION()
	void TryFinishCharacterAction();
	
public:

	void SetAvailableToControl(bool IsAvailable) { AvailableToControl = IsAvailable;}
	bool GetAvailableToControl() const {return AvailableToControl;}

	APlayerActor* GetPlayerActorRef() const {return PlayerActorRef;}
	
	
	// Delegate
	FOnGetControlCharacterCount OnGetControlCharacterCountSignature;
	FOnTryEndPlayerTurn OnTryEndPlayerTurnDelegate;
	FOnTryFinishCharacterAction OnTryFinishCharacterActionDelegate;
	
};


