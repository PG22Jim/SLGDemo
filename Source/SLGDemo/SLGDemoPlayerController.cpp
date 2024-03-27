// Copyright Epic Games, Inc. All Rights Reserved.

#include "SLGDemoPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "SLGDemoCharacter.h"
#include "Characters/BaseCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ASLGDemoPlayerController::ASLGDemoPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

bool ASLGDemoPlayerController::TryCachePlayerActor()
{
	if(PlayerActorRef) return true;
	
	const UWorld* World = GetWorld();
	if(!World) return false;

	AActor* ReturnActor = UGameplayStatics::GetPlayerPawn(World,0);
	if(!ReturnActor) return false;

	APlayerActor* CastedResult = Cast<APlayerActor>(ReturnActor);
	if(!CastedResult) return false;

	PlayerActorRef = CastedResult;
	return true;
}

void ASLGDemoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TryCachePlayerActor();
}

void ASLGDemoPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void ASLGDemoPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ASLGDemoPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ASLGDemoPlayerController::OnSetDestinationReleased);

	InputComponent->BindAxis("MoveForward", this, &ASLGDemoPlayerController::TryMoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASLGDemoPlayerController::TryMoveRight);

	InputComponent->BindAxis("CameraMoveForward", this, &ASLGDemoPlayerController::TryCameraMoveForward);
	InputComponent->BindAxis("CameraMoveRight", this, &ASLGDemoPlayerController::TryCameraMoveRight);
	
	InputComponent->BindAction("RightInMenu", IE_Pressed, this, &ASLGDemoPlayerController::TryExecuteNextKey);
	InputComponent->BindAction("LeftInMenu", IE_Pressed, this, &ASLGDemoPlayerController::TryExecutePreviousKey);
	InputComponent->BindAction("EndPlayerTurn", IE_Pressed, this, &ASLGDemoPlayerController::TryEndPlayerTurn);
	InputComponent->BindAction("FinishCharacterAction", IE_Pressed, this, &ASLGDemoPlayerController::TryFinishCharacterAction);


	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASLGDemoPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ASLGDemoPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &ASLGDemoPlayerController::OnResetVR);
}

void ASLGDemoPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASLGDemoPlayerController::MoveToMouseCursor()
{
	if(!TryCachePlayerActor()) return;
	
	
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		//if (ASLGDemoCharacter* MyPawn = Cast<ASLGDemoCharacter>(GetPawn()))
		if (APlayerActor* MyPawn = Cast<APlayerActor>(GetPawn()))
		{
			if(ABaseCharacter* ControllingCharacter = MyPawn->GetControllingCharacter())
				if (MyPawn->GetCursorToWorld())
				{
					UAIBlueprintHelperLibrary::SimpleMoveToLocation(ControllingCharacter->GetController(), MyPawn->GetCursorToWorld()->GetComponentLocation());
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));

				}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			if(TryCachePlayerActor())
			{
				const bool IsReachablePoint = PlayerActorRef->CheckSquareByPosition(Hit.ImpactPoint);
			
				// We hit square that is reachable, move there
				if(IsReachablePoint) SetNewMoveDestination(Hit.ImpactPoint);
			}
		}
	}
}

void ASLGDemoPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		if(TryCachePlayerActor())
		{
			const bool IsReachablePoint = PlayerActorRef->CheckSquareByPosition(PlayerActorRef->GetCursorToWorld()->GetComponentLocation());
			
			// We hit square that is reachable, move there
			if(IsReachablePoint) SetNewMoveDestination(HitResult.ImpactPoint);
		}
	}
}

void ASLGDemoPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APlayerActor* MyPawn = Cast<APlayerActor>(GetPawn());
	if (ABaseCharacter* ControllingCharacter = MyPawn->GetControllingCharacter())
	{
		float const Distance = FVector::Dist(DestLocation, ControllingCharacter->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		//if ((Distance > 120.0f))
		//{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(ControllingCharacter->GetController(), DestLocation);
		//}
	}
}

void ASLGDemoPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ASLGDemoPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void ASLGDemoPlayerController::TryExecuteNextKey()
{
	if(!TryCachePlayerActor()) return;

	PlayerActorRef->OnRequestNextOrPrevious(true);
}

void ASLGDemoPlayerController::TryExecutePreviousKey()
{
	if(!TryCachePlayerActor()) return;

	PlayerActorRef->OnRequestNextOrPrevious(false);
}

void ASLGDemoPlayerController::TryMoveForward(float Value)
{
	if(!TryCachePlayerActor()) return;

	const FVector CheckingDirection = (Value > 0.0f) ? PlayerActorRef->GetActorForwardVector() : (PlayerActorRef->GetActorForwardVector() * -1) ;
	if(PlayerActorRef->CheckSquareWithDirection(CheckingDirection))
		PlayerActorRef->MoveForward(Value);
}

void ASLGDemoPlayerController::TryMoveRight(float Value)
{
	if(!TryCachePlayerActor()) return;

	const FVector CheckingDirection = (Value > 0.0f) ? PlayerActorRef->GetActorRightVector() : (PlayerActorRef->GetActorRightVector() * -1) ;
	if(PlayerActorRef->CheckSquareWithDirection(CheckingDirection))
		PlayerActorRef->MoveRight(Value);
}

void ASLGDemoPlayerController::TryCameraMoveForward(float Value)
{
	if(APawn* OwningPawn = GetPawn())
	{
		const FVector CurrentPos = OwningPawn->GetActorLocation();
		const float NewLocationX = CurrentPos.X + (CameraMovement * Value);
		
		OwningPawn->SetActorLocation(FVector(NewLocationX, CurrentPos.Y, CurrentPos.Z));
	}
}

void ASLGDemoPlayerController::TryCameraMoveRight(float Value)
{
	if(APawn* OwningPawn = GetPawn())
	{
		const FVector CurrentPos = OwningPawn->GetActorLocation();
		const float NewLocationY = CurrentPos.Y + (CameraMovement * Value);
		
		OwningPawn->SetActorLocation(FVector(CurrentPos.X, NewLocationY, CurrentPos.Z));
	}
}

void ASLGDemoPlayerController::TryEndPlayerTurn()
{
	if(!OnGetControlCharacterCountSignature.IsBound()) return;

		
	const int32 AvailableNum = OnGetControlCharacterCountSignature.Execute();
	if(AvailableNum > 0)
	{
		
		// TODO: Show End Turn Confirmation if there is still available character
	}

	
	
}

void ASLGDemoPlayerController::TryFinishCharacterAction()
{
	OnTryFinishCharacterActionDelegate.ExecuteIfBound();
}
