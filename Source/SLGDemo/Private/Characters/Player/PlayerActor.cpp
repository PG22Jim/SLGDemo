// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/PlayerActor.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProfilingDebugging/CookStats.h"

// Sets default values
APlayerActor::APlayerActor()
{
	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

}

// Called when the game starts or when spawned
void APlayerActor::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat TimeStopEffectProgressUpdate;
	TimeStopEffectProgressUpdate.BindDynamic(this, &APlayerActor::OnCameraMovementUpdate);
	CameraQuickMovementTimeline.AddInterpFloat(CameraQuickMovementCurveFloat,TimeStopEffectProgressUpdate);
}

void APlayerActor::AssignMovingStartEnd()
{
	if(!ControllingCharacterRef) return;

	MovingStartPosition = GetActorLocation();
	
	const FVector ControlChaRefPos = ControllingCharacterRef->GetActorLocation();
	MovingEndPosition = FVector(ControlChaRefPos.X, ControlChaRefPos.Y, MovingStartPosition.Z);
}

// Called every frame
void APlayerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CursorToWorld != nullptr)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
	}

	CameraQuickMovementTimeline.TickTimeline(DeltaTime);
}

void APlayerActor::TickCheckingStandingSquare()
{

	// if(!ControllingCharacterRef) return;
	// if(!ControllingCharacterRef->GetInitialSquare()) return;
	//
	// const UWorld* World = GetWorld();
	// if(World == nullptr) return;
	//
	//
	// TArray<AActor*> IgnoreActors;
	// IgnoreActors.Add(ControllingCharacterRef);
	// const FVector Start = ControllingCharacterRef->GetActorLocation();
	// const FVector Offset = (ControllingCharacterRef->GetActorUpVector() * -1) * 200.0f;
	// const FVector End = Start + Offset;
	//
	// FHitResult Hit;
	// const bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(World, Start, End, 35.0f, 50, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::Persistent, Hit, true);
	//
	// if(bHit)
	// {
	// 	FString HitResultName = Hit.Actor->GetName();
	// 	FString InitialSquareName = ControllingCharacterRef->GetInitialSquare()->GetName();
	// 	if(HitResultName != InitialSquareName)
	// 	{
	// 	}
	// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Some variable values: HitResultName: %s, InitialSquareName: %s"), *HitResultName, *InitialSquareName));
	// }
}

bool APlayerActor::CheckSquareWithDirection(FVector CheckingDirection)
{
	if(!ControllingCharacterRef) return false;
	if(!ControllingCharacterRef->GetInitialSquare()) return false;
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return false;
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(ControllingCharacterRef);
	const FVector Start = ControllingCharacterRef->GetActorLocation() + (CheckingDirection * 50);
	const FVector Offset = (ControllingCharacterRef->GetActorUpVector() * -1) * 200.0f;
	const FVector End = Start + Offset;
	
	FHitResult Hit;
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(World, Start, End, 20.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);

	if(bHit)
	{
		FString HitActorName = Hit.Actor->GetName();
		// TODO: Execute Delegate to see if square is
		if(OnCheckPointReachableDelegate.IsBound())
		{
			if(OnCheckPointReachableDelegate.Execute(HitActorName))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

bool APlayerActor::CheckSquareByPosition(FVector DecalCompLocation)
{
	if(!ControllingCharacterRef) return false;
	if(!ControllingCharacterRef->GetInitialSquare()) return false;
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return false;
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(ControllingCharacterRef);
	const FVector Start = DecalCompLocation;
	const FVector Offset = (ControllingCharacterRef->GetActorUpVector() * -1) * 200.0f;
	const FVector End = Start + Offset;
	
	FHitResult Hit;
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(World, Start, End, 20.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);

	if(bHit)
	{
		FString HitActorName = Hit.Actor->GetName();
		// TODO: Execute Delegate to see if square is
		if(OnCheckPointReachableDelegate.IsBound())
		{
			if(OnCheckPointReachableDelegate.Execute(HitActorName))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

void APlayerActor::OverwriteControllingCharacterRef(ABaseCharacter* NewCharacter)
{
	if(!NewCharacter) return;

	ControllingCharacterRef = NewCharacter;
	
	// Try Set camera
	AssignMovingStartEnd();
	CameraQuickMovementTimeline.PlayFromStart();
}

void APlayerActor::OnRequestNextOrPrevious(bool IsNext)
{
	OnRequestNextOrPreviousSignature.ExecuteIfBound(IsNext);
}

void APlayerActor::OnBeginTickStandingCheck()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;
	
	World->GetTimerManager().SetTimer(StandingSquareCheckTimerHandle,this, &APlayerActor::TickCheckingStandingSquare, 0.05, true, -1);
}

void APlayerActor::MoveForward(float Value)
{
	if(ControllingCharacterRef)
		if ((Controller != nullptr) && (Value != 0.0f))
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			
			ControllingCharacterRef->AddMovementInput(Direction, Value);
		}
}

void APlayerActor::MoveRight(float Value)
{
	if(ControllingCharacterRef)
		if ( (Controller != nullptr) && (Value != 0.0f))
		{
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
		
			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			
			// add movement in that direction
			ControllingCharacterRef->AddMovementInput(Direction, Value);
		}
}

void APlayerActor::OnCameraMovementUpdate(float Value)
{
	const FVector LerpResult = UKismetMathLibrary::VLerp(MovingStartPosition, MovingEndPosition, Value);
	SetActorLocation(LerpResult);
}

