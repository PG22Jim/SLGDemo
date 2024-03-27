// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/GameManager.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	TryCachePostProcessVolume();
	TryAllocateAllBaseCharactersOnMap();
	InitializeMap();

	PreGameStart();
	OnGameStart();
}

void AGameManager::TryAllocateAllBaseCharactersOnMap()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	TArray<AActor*> ReturnActors;
	
	UGameplayStatics::GetAllActorsOfClass(World, SearchingBaseCharacterClass, ReturnActors);

	if(ReturnActors.Num() <= 0) return;

	for(AActor* EachReturnActor : ReturnActors)
	{
		ABaseCharacter* CastedResult = Cast<ABaseCharacter>(EachReturnActor);
		if(!CastedResult) continue;

		if(CastedResult->GetBelongingTeam() == CharacterTeam::Player)
			PlayerControllingCharacterList.Add(CastedResult);
		else
			EnemyControllingCharacterList.Add(CastedResult);
	}
}

bool AGameManager::CharacterListIndexCheck(int32 SelectingIndex, bool IsPlayerList)
{
	if(IsPlayerList) return PlayerAvailableCharacterList.Num() > SelectingIndex && SelectingIndex >= 0;
	else return EnemyControllingCharacterList.Num() > SelectingIndex && SelectingIndex >= 0;
}

void AGameManager::InitializeMap()
{
	if(!TryCacheMapManager()) return;

	
}

bool AGameManager::TryCacheMapManager()
{
	if(MapManagerRef) return true;

	const UWorld* World = GetWorld();
	if(!World) return false;

	
	AActor* ReturnResult = UGameplayStatics::GetActorOfClass(World, MapManagerClass);
	if(!ReturnResult)
	{
		if(TrySpawnMapManager()) return true;
		else return false;
	}

	AMapManager* CastResult = Cast<AMapManager>(ReturnResult);
	if(!CastResult) return false;

	MapManagerRef = CastResult;
	return true;
}

bool AGameManager::TrySpawnMapManager()
{
	if(MapManagerRef) return true;

	UWorld* World = GetWorld();
	if(!World) return false;
	
	
	// Spawning Map Manager
	// set spawn parameter
	FActorSpawnParameters MapManagerSpawnParams;
	MapManagerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// get game manager's location and rotation
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
	
	// spawn Map Manager
	AMapManager* SpawnResult = World->SpawnActor<AMapManager>(MapManagerClass, SpawnLocation, SpawnRotation);
	
	if(!SpawnResult) return false;

	MapManagerRef = SpawnResult;
	return true;
}

bool AGameManager::TryCachePostProcessVolume()
{
	if(PostProcessVolumeRef) return true;

	const UWorld* World = GetWorld();
	if(!World) return false;

	
	AActor* ReturnResult = UGameplayStatics::GetActorOfClass(World, APostProcessVolume::StaticClass());
	if(!ReturnResult) return false;
	

	APostProcessVolume* CastResult = Cast<APostProcessVolume>(ReturnResult);
	if(!CastResult) return false;

	PostProcessVolumeRef = CastResult;
	return true;
}

void AGameManager::ClearAllPlayerCharacterOutline()
{
	//if(!PostProcessVolumeRef) if(!TryCachePostProcessVolume()) return;

	for (ABaseCharacter* IteratePlayerCharacter : PlayerControllingCharacterList)
	{
		ClearSingleCharacterOutline(IteratePlayerCharacter);
	}
	
}

void AGameManager::ClearSingleCharacterOutline(ABaseCharacter* ChangingCharacter)
{
	USkeletalMeshComponent* ChangingCharacterMesh = ChangingCharacter->GetMesh();

	ChangingCharacterMesh->SetCustomDepthStencilValue(0);
	
}

void AGameManager::ChangeCharacterAvailability(ABaseCharacter* ChangingCharacter,
                                               CharacterAvailability SettingAvailability)
{
	USkeletalMeshComponent* ChangingCharacterMesh = ChangingCharacter->GetMesh();

	switch (SettingAvailability)
	{
		case CharacterAvailability::Controlling:
			ChangingCharacterMesh->SetCustomDepthStencilValue(3);
			break;
		case CharacterAvailability::Available:
			ChangingCharacterMesh->SetCustomDepthStencilValue(2);
			break;
		case CharacterAvailability::Unavailable:
			ChangingCharacterMesh->SetCustomDepthStencilValue(4);
			break;
		default:
			break;
	}
}

void AGameManager::ExecuteNextOrPreviousKey(bool IsNext)
{
	// TODO:: Check which condition is

	if(IsNext) TryGetNextAvailablePlayerCharacter();
	else TryGetPreviousAvailablePlayerCharacter();

	OnPlayerSuccessfullySwitchCharacter();
}

void AGameManager::TryGetNextAvailablePlayerCharacter()
{
	if(!TryCachePlayerActor()) return;

	const int32 PlayerSelectedIndex = PlayerActorRef->GetSelectingIndex();
	int32 NextIndex = PlayerSelectedIndex + 1;
	if(NextIndex > (PlayerAvailableCharacterList.Num() - 1)) NextIndex = 0;
	
	if(!CharacterListIndexCheck(NextIndex, true)) return;
	
	
	TrySetPlayerControllingCharacter(NextIndex);
	// TODO: Set available square color
}

void AGameManager::TryGetPreviousAvailablePlayerCharacter()
{
	if(!TryCachePlayerActor()) return;
	
	const int32 PlayerSelectedIndex = PlayerActorRef->GetSelectingIndex();
	int32 PreviousIndex = PlayerSelectedIndex - 1;
	if(PreviousIndex < 0) PreviousIndex = 0;
	
	if(!CharacterListIndexCheck(PreviousIndex, true)) return;
	
	TrySetPlayerControllingCharacter(PreviousIndex);
}

TArray<FCharacterAbility> AGameManager::GetCharacterAbilityByClass(CharacterClass SearchingClass)
{
	return AbilitiesForEachClass.FindRef(SearchingClass);
}

bool AGameManager::TryCachePlayerActor()
{
	if(PlayerActorRef) return true;
	
	const UWorld* World = GetWorld();
	if(!World) return false;

	AActor* ReturnActor = UGameplayStatics::GetActorOfClass(World, SearchingPlayerActorClass);
	if(!ReturnActor) return false;

	APlayerActor* CastedResult = Cast<APlayerActor>(ReturnActor);
	if(!CastedResult) return false;

	PlayerActorRef = CastedResult;
	TryCachePlayerController();
	TryAssignPlayerDelegate();
	return true;
	
}

bool AGameManager::TryCachePlayerController()
{
	if(PlayerControllerRef) return true;

	if(!PlayerActorRef)
		if(!TryCachePlayerActor()) return false;
	
	const UWorld* World = GetWorld();
	if(!World) return false;

	AController* ReturnController = PlayerActorRef->GetController();
	if(!ReturnController) return false;

	ASLGDemoPlayerController* CastedResult = Cast<ASLGDemoPlayerController>(ReturnController);
	if(!CastedResult) return false;

	PlayerControllerRef = CastedResult;
	return true;
}

void AGameManager::TryAssignPlayerDelegate()
{
	if(!PlayerActorRef)
		if(!TryCachePlayerActor()) return;

	if(!PlayerControllerRef)
		if(!TryCachePlayerController()) return;
	

	if(!PlayerActorRef->OnRequestNextOrPreviousSignature.IsBound())
		PlayerActorRef->OnRequestNextOrPreviousSignature.BindDynamic(this, &AGameManager::ExecuteNextOrPreviousKey);

	if(!PlayerActorRef->OnCheckPointReachableDelegate.IsBound())
		PlayerActorRef->OnCheckPointReachableDelegate.BindDynamic(this, &AGameManager::IsSquareReachable);


	if(!PlayerControllerRef->OnGetControlCharacterCountSignature.IsBound())
		PlayerControllerRef->OnGetControlCharacterCountSignature.BindDynamic(this, &AGameManager::GetAvailableCharacterNum);

	if(!PlayerControllerRef->OnTryFinishCharacterActionDelegate.IsBound())
		PlayerControllerRef->OnTryFinishCharacterActionDelegate.BindDynamic(this, &AGameManager::OnPlayerFinishCurrentCharacterAction);
}

void AGameManager::PreGameStart()
{
	for (int32 i = 0; i <7; i++)
	{
		CharacterClass RequestClass = static_cast<CharacterClass>(i);
		TArray<FCharacterAbility> ReturnAbilities = GetAbilitiesByClass(RequestClass);
		
		AbilitiesForEachClass.Add(RequestClass, ReturnAbilities);
	}
}

TArray<FCharacterAbility> AGameManager::GetAbilitiesByClass(CharacterClass RequestClass)
{
	TArray<FCharacterAbility> ReturnAbilities;

	switch (RequestClass)
	{
	case CharacterClass::Breaker:
		{
			FCharacterAbility BreakerFirstAbility =
				FCharacterAbility("RapidStrike", 100, 0, 25,
					AbilityTarget::Enemy, 1,1,100/*, AbilityDetermination()*/,true);

			BreakerFirstAbility.AddAbilityStatusEffect(AddingStatusEffect(AbilityTarget::Self ,
				StatusEffect(BuffDebuffType::Buff,3, CharacterStatistic::PhysicalAttack, 10)));
			
			FCharacterAbility BreakerSecondAbility =
				FCharacterAbility("TeamExecution", 75, 0,25,
				AbilityTarget::Enemy, 1, 2, 80/*, AbilityDetermination()*/, true);

			FCharacterAbility BreakerThirdAbility =
				FCharacterAbility("LeechingStrike", 100, 0, 25,
				AbilityTarget::Enemy, 1,1,100/*, AbilityDetermination()*/, false);

			BreakerThirdAbility.AddAbilityStatusEffect(AddingStatusEffect(AbilityTarget::Self ,
				StatusEffect(BuffDebuffType::Buff,3, CharacterStatistic::PhysicalAttack, 10)));
			BreakerThirdAbility.AddAbilityStatusEffect(AddingStatusEffect(AbilityTarget::Self ,
				StatusEffect(BuffDebuffType::Buff,3, CharacterStatistic::PhysicalArmor, 10)));
			BreakerThirdAbility.AddAbilityStatusEffect(AddingStatusEffect(AbilityTarget::Self ,
				StatusEffect(BuffDebuffType::Buff,3, CharacterStatistic::DodgeChance, 10)));
			BreakerThirdAbility.AddAbilityStatusEffect(AddingStatusEffect(AbilityTarget::Self ,
				StatusEffect(BuffDebuffType::Buff,3, CharacterStatistic::HitRate, 10)));

			BreakerThirdAbility.AddAbilityStatusEffect(AddingStatusEffect(AbilityTarget::Enemy ,
				StatusEffect(BuffDebuffType::Buff,3, CharacterStatistic::PhysicalAttack, -10)));
			BreakerThirdAbility.AddAbilityStatusEffect(AddingStatusEffect(AbilityTarget::Enemy ,
				StatusEffect(BuffDebuffType::Buff,3, CharacterStatistic::PhysicalArmor, -10)));
			BreakerThirdAbility.AddAbilityStatusEffect(AddingStatusEffect(AbilityTarget::Enemy ,
				StatusEffect(BuffDebuffType::Buff,3, CharacterStatistic::DodgeChance, -10)));
			BreakerThirdAbility.AddAbilityStatusEffect(AddingStatusEffect(AbilityTarget::Enemy ,
				StatusEffect(BuffDebuffType::Buff,3, CharacterStatistic::HitRate, -10)));

			FCharacterAbility BreakerFourthAbility =
				FCharacterAbility("TacticBreak", 50, 0, 0,
				AbilityTarget::Self, 0,1,100/*, AbilityDetermination()*/, false);
			
			ReturnAbilities.Add(BreakerFirstAbility);
			ReturnAbilities.Add(BreakerSecondAbility);
			ReturnAbilities.Add(BreakerThirdAbility);
			ReturnAbilities.Add(BreakerFourthAbility);
			return ReturnAbilities;
			
		}
	case CharacterClass::Smasher:
		{
			FCharacterAbility SmasherFirstAbility =
				FCharacterAbility("ArmorCrusher", 100, 0, 50,
				AbilityTarget::Enemy, 1,1,80/*, AbilityDetermination()*/, true);
			FCharacterAbility SmasherSecondAbility =
				FCharacterAbility("StraightSmash", 80, 0, 25,
				AbilityTarget::Enemy, 1,2,70/*, AbilityDetermination()*/, true);
			FCharacterAbility SmasherThirdAbility =
				FCharacterAbility("ArmorCrusher", 100, 0, 0,
				AbilityTarget::Enemy, 1,1,50/*, AbilityDetermination()*/, true);
			FCharacterAbility SmasherFourthAbility =
				FCharacterAbility("ArmorBreak", 50, 0, 0,
				AbilityTarget::Self, 0,1,100/*, AbilityDetermination()*/, false);

			ReturnAbilities.Add(SmasherFirstAbility);
			ReturnAbilities.Add(SmasherSecondAbility);
			ReturnAbilities.Add(SmasherThirdAbility);
			ReturnAbilities.Add(SmasherFourthAbility);
			return ReturnAbilities;
		}
	
	case CharacterClass::Sniper:
		{
			FCharacterAbility SniperFirstAbility = FCharacterAbility("SnipingShot", 100, 0, 25, AbilityTarget::Enemy, 4,1,100/*, AbilityDetermination()*/, true);
			FCharacterAbility SniperSecondAbility = FCharacterAbility("PiercingBullet", 75, 0, 25, AbilityTarget::Enemy, 4,2,100/*, AbilityDetermination()*/, true);
			FCharacterAbility SniperThirdAbility = FCharacterAbility("IntimidatingShot", 0, 0, 50, AbilityTarget::Enemy, 1,3,80/*, AbilityDetermination()*/, false);
			FCharacterAbility SniperFourthAbility = FCharacterAbility("HawkEye", 0, 0, 0, AbilityTarget::Self, 0,1,100/*, AbilityDetermination()*/, false);
			
			ReturnAbilities.Add(SniperFirstAbility);
			ReturnAbilities.Add(SniperSecondAbility);
			ReturnAbilities.Add(SniperThirdAbility);
			ReturnAbilities.Add(SniperFourthAbility);
			return ReturnAbilities;
		}

	case CharacterClass::Ranger:
		{
			FCharacterAbility RangerFirstAbility = FCharacterAbility("PoisonStrike", 100, 0, 25, AbilityTarget::Enemy, 4,1,100/*, AbilityDetermination()*/, false);
			FCharacterAbility RangerSecondAbility = FCharacterAbility("RapidArrow", 75, 0, 25, AbilityTarget::Enemy, 4,1,100/*, AbilityDetermination()*/, true);
			FCharacterAbility RangerThirdAbility = FCharacterAbility("ArrowRain", 50, 0, 25, AbilityTarget::Enemy, 4,2,80/*, AbilityDetermination()*/, true);
			FCharacterAbility RangerFourthAbility = FCharacterAbility("PoisonPrepare", 0, 0, 0, AbilityTarget::Self, 1,1,100/*, AbilityDetermination()*/, false);

			ReturnAbilities.Add(RangerFirstAbility);
			ReturnAbilities.Add(RangerSecondAbility);
			ReturnAbilities.Add(RangerThirdAbility);
			ReturnAbilities.Add(RangerFourthAbility);

			return ReturnAbilities;
		}
	case CharacterClass::Mage:
		{
			FCharacterAbility MageFirstAbility = FCharacterAbility("Fireball", 0, 100, 25, AbilityTarget::Enemy, 4,1,100/*, AbilityDetermination()*/, false);
			FCharacterAbility MageSecondAbility = FCharacterAbility("MagicRain", 0, 75, 25, AbilityTarget::Enemy, 4,2,80/*, AbilityDetermination()*/, false);
			FCharacterAbility MageThirdAbility = FCharacterAbility("CloseRangeExplosion", 0, 200, 0, AbilityTarget::Enemy, 1,1,75/*, AbilityDetermination()*/, true);
			FCharacterAbility MageFourthAbility = FCharacterAbility("FocusCharge", 0, 0, 0, AbilityTarget::Self, 1,1,100/*, AbilityDetermination()*/, false);
			
			ReturnAbilities.Add(MageFirstAbility);
			ReturnAbilities.Add(MageSecondAbility);
			ReturnAbilities.Add(MageThirdAbility);
			ReturnAbilities.Add(MageFourthAbility);

			return ReturnAbilities;
		}
	
	case CharacterClass::Supporter:
		{
			FCharacterAbility SupporterFirstAbility = FCharacterAbility("LifeSupport", 0, 100, 0, AbilityTarget::Ally, 3,2,100/*, AbilityDetermination()*/, false);
			FCharacterAbility SupporterSecondAbility = FCharacterAbility("AttackSupport", 0, 0, 0, AbilityTarget::Ally, 3,2,100/*, AbilityDetermination()*/, false);
			FCharacterAbility SupporterThirdAbility = FCharacterAbility("DefenceSupport", 0, 0, 0, AbilityTarget::Ally, 3,2,100/*, AbilityDetermination()*/, false);
			
			ReturnAbilities.Add(SupporterFirstAbility);
			ReturnAbilities.Add(SupporterSecondAbility);
			ReturnAbilities.Add(SupporterThirdAbility);
			return ReturnAbilities;
		}

	
	case CharacterClass::Warlock:
		{
			FCharacterAbility WarlockFirstAbility = FCharacterAbility("LifeLeeching", 0, 75, 25, AbilityTarget::Enemy, 3,1,100/*, AbilityDetermination()*/, false);
			FCharacterAbility WarlockSecondAbility = FCharacterAbility("MovementObstruction", 0, 0, 0, AbilityTarget::Enemy, 3,2,100/*, AbilityDetermination()*/, false);
			FCharacterAbility WarlockThirdAbility = FCharacterAbility("AttackObstruction", 0, 0, 0, AbilityTarget::Enemy, 3,2,100/*, AbilityDetermination()*/, false);
			FCharacterAbility WarlockFourthAbility = FCharacterAbility("DefenceObstruction", 0, 0, 0, AbilityTarget::Enemy, 3,2,100/*, AbilityDetermination()*/, false);
				
			ReturnAbilities.Add(WarlockFirstAbility);
			ReturnAbilities.Add(WarlockSecondAbility);
			ReturnAbilities.Add(WarlockThirdAbility);
			ReturnAbilities.Add(WarlockFourthAbility);

			return ReturnAbilities;
		}
	default: break;
	}
	

	
	return ReturnAbilities;
}

void AGameManager::OnGameStart()
{
	OnPlayerTurnBegin();
}

void AGameManager::OnPlayerTurnBegin()
{
	if(!TryCachePlayerActor()) return;
	PlayerAvailableCharacterList.Empty();
	
	for (ABaseCharacter* IterateCharacter : PlayerControllingCharacterList)
	{
		if(!IterateCharacter) continue;
		
		// TODO: Check is character dead
		
		PlayerAvailableCharacterList.Add(IterateCharacter);
		ChangeCharacterAvailability(IterateCharacter, CharacterAvailability::Available);

		// TODO: Remove later Randomize character position
		const int32 RndRow = UKismetMathLibrary::RandomInteger(10);
		const int32 RndColumn = UKismetMathLibrary::RandomInteger(10);

		if(!MapManagerRef) if(!TryCacheMapManager()) continue;;
		if(AMapPositionSquare* RequestedSquare = MapManagerRef->GetSpecificSquareByRowColumn(FVector2D(RndRow, RndColumn)))
		{
			const FVector SquareCenter = RequestedSquare->GetStoringMapSquareData().GetSquarePosition();

			IterateCharacter->SetActorLocation(SquareCenter);
			IterateCharacter->SetInitialSquare(RequestedSquare);
			RequestedSquare->SetStoringOccupiedCharacter(IterateCharacter);
		}
	}

	// TODO: Later put to other place
	for (ABaseCharacter* EachEnemyCharacter : EnemyControllingCharacterList)
	{
		if(!EachEnemyCharacter) continue;
		
		if(!MapManagerRef) if(!TryCacheMapManager()) continue;;
		if(AMapPositionSquare* RequestedSquare = MapManagerRef->GetSpecificSquareByRowColumn(FVector2D(5, 5)))
		{
			if(!RequestedSquare->GetStoringOccupiedCharacter())
			{
				const FVector SquareCenter = RequestedSquare->GetStoringMapSquareData().GetSquarePosition();
				
				EachEnemyCharacter->SetActorLocation(SquareCenter);
				EachEnemyCharacter->SetInitialSquare(RequestedSquare);
				RequestedSquare->SetStoringOccupiedCharacter(EachEnemyCharacter);

			}
			

		}
	}
	
	if(TryCachePlayerActor())
	{
		TrySetPlayerControllingCharacter(0);
		PlayerActorRef->OnBeginTickStandingCheck();

		OnPlayerSuccessfullySwitchCharacter();
	}
}

void AGameManager::OnPlayerSuccessfullySwitchCharacter()
{
	ABaseCharacter* CurrentControlCharacter = PlayerActorRef->GetControllingCharacter();
	if(CurrentControlCharacter)
	{
		int32 CurrentCharacterMovement = CurrentControlCharacter->GetCurrentCharacterStats().GetMovementSpeed();
		AMapPositionSquare* StoredInitialSquare = CurrentControlCharacter->GetInitialSquare();
		CurrentReachableSquares.Empty();
		CurrentReachableSquares = MapManagerRef->GetAllAvailableSquares(CurrentCharacterMovement, StoredInitialSquare);
			
	}
}

void AGameManager::OnPlayerFinishCurrentCharacterAction()
{
	// Cache control character reference and check if valid
	ABaseCharacter* CurrentControlCharacter = PlayerActorRef->GetControllingCharacter();
	if(CurrentControlCharacter)
	{
		if(AMapPositionSquare* UnOccupiedSquare = CheckStandingSquareOccupied(CurrentControlCharacter))
		{
			// Clear Character From list, change its outline, and clear the reference
			PlayerAvailableCharacterList.Remove(CurrentControlCharacter);
			ChangeCharacterAvailability(CurrentControlCharacter, CharacterAvailability::Unavailable);
			PlayerActorRef->SetControllingCharacter(nullptr);

			// and always use next key
			ExecuteNextOrPreviousKey(true);

			//
			FVector StandingSquarePos = UnOccupiedSquare->GetActorLocation();
			CurrentControlCharacter->SetActorLocation(StandingSquarePos);
		}
	}
}

AMapPositionSquare* AGameManager::CheckStandingSquareOccupied(ABaseCharacter* CheckingCharacter)
{
	if(!TryCachePlayerActor() || !CheckingCharacter) return nullptr;
	
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return nullptr;
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(CheckingCharacter);
	const FVector Start = CheckingCharacter->GetActorLocation();
	const FVector Offset = (CheckingCharacter->GetActorUpVector() * -1) * 200.0f;
	const FVector End = Start + Offset;
	
	FHitResult Hit;
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(World, Start, End, 20.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);

	// if there is any square hit
	if(bHit)
	{
		AMapPositionSquare* CurrentStandingSquare = Cast<AMapPositionSquare>(Hit.Actor);
		if(CurrentStandingSquare)
		{
			// start second trace check to see if there is any other character is on the square
			
			const FVector HitActorLocation = Hit.Actor->GetActorLocation();
			const FVector SecondOffset = FVector(0,0,1) * 80.0f;
			const FVector SecondEnd = HitActorLocation + SecondOffset;
			const bool bSecondHit = UKismetSystemLibrary::SphereTraceSingle(World, HitActorLocation, SecondEnd, 80.0f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
			
			// Then check if square is already occupied
			if(bSecondHit) return nullptr;
			else return CurrentStandingSquare;
		}
		return nullptr;
	}

	return nullptr;
}

bool AGameManager::IsSquareReachable(FString RequestSquareName)
{
	for(AMapPositionSquare* IterateSquare : CurrentReachableSquares)
	{
		FString IterateSquareName = IterateSquare->GetName();
		if(IterateSquareName == RequestSquareName) return true;
	}

	return false;
}

void AGameManager::TrySetPlayerControllingCharacter(int32 SetIndex)
{
	if(!TryCachePlayerActor()) return;
	ABaseCharacter* CurrentControllingCharacter = PlayerActorRef->GetControllingCharacter();
	
	ABaseCharacter* SettingCharacter = PlayerAvailableCharacterList[SetIndex];
	
	if(SettingCharacter)
	{
		if(CurrentControllingCharacter)
			ChangeCharacterAvailability(CurrentControllingCharacter, CharacterAvailability::Available);
		ChangeCharacterAvailability(SettingCharacter, CharacterAvailability::Controlling);
		PlayerActorRef->OverwriteControllingCharacterRef(SettingCharacter);
		PlayerActorRef->SetSelectingIndex(SetIndex);
	}
}


void AGameManager::OnTryToEndPlayerTurn()
{
	if(!PlayerControllerRef) if(!TryCachePlayerActor()) return;

	PlayerControllerRef->SetAvailableToControl(false);
	
	// TODO: clear outline color
	ClearAllPlayerCharacterOutline();

	
	// TODO: MoveCameraToCenterPoint

	
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DebugMenuDisplay(DeltaTime);
	
}

void AGameManager::DebugMenuDisplay(float DeltaTime)
{

	// TODO: REMOVE LATER
	if(TryCachePlayerActor())
	{
		if(ABaseCharacter* CurrentControlCha = PlayerActorRef->GetControllingCharacter())
		{
			CharacterClass CurrentControlClass = CurrentControlCha->GetCurrentCharacterStats().GetCurrentClass();
			TArray<FCharacterAbility> ResultAbilities = GetCharacterAbilityByClass(CurrentControlClass);
			if(ResultAbilities.Num() > 0)
			{
				for (FCharacterAbility IterateAbility : ResultAbilities)
				{
					FString AbilityName = IterateAbility.GetAbilityName();
					GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Ability: %s"), *AbilityName));
				}	
			}
		}
	}
}

