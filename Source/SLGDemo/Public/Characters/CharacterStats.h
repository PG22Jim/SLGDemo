// Copyright © 2024 by Jim Chen. All rights reserved.


#pragma once
#include "CharacterEnum.h"

struct StatusEffect
{
protected:

	BuffDebuffType AbilityAffectType  = BuffDebuffType::None;
	int32 RemainTurn = 1;
	CharacterStatistic AffectingStatistic = CharacterStatistic::PhysicalAttack;
	float AffectingValue = 0.0f;
	
public:

	StatusEffect()
	{
		
	}

	StatusEffect(BuffDebuffType NewType, int32 NewRemainTurn, CharacterStatistic NewStatistic, float NewAffectValue)
	{
		AbilityAffectType = NewType, RemainTurn = NewRemainTurn, AffectingStatistic = NewStatistic, AffectingValue = NewAffectValue;
	}

	// getter and setter
	BuffDebuffType GetAbilityAffectType() const {return AbilityAffectType;}
	int32 GetRemainTurn() const {return RemainTurn;}
	CharacterStatistic GetAffectingStatistic() const {return AffectingStatistic;}
	float GetPhysicalArmor() const {return AffectingValue;}
	
	void SetAbilityAffectType(BuffDebuffType NewType) { AbilityAffectType = NewType;}
	void SetRemainTurn(int32 NewRemainTurn) { RemainTurn = NewRemainTurn;}
	void SetAffectingStatistic(CharacterStatistic NewStatistic) { AffectingStatistic = NewStatistic;}
	void SetAffectingValue(float NewAffectValue) { AffectingValue = NewAffectValue;}
};

struct AddingStatusEffect
{
protected:

	AbilityTarget AddingTarget = AbilityTarget::None;
	StatusEffect NewStatusEffect = StatusEffect();

	
public:
	AddingStatusEffect()
	{
		
	}

	AddingStatusEffect(AbilityTarget NewTarget, StatusEffect NewEffect)
	{
		AddingTarget = NewTarget;
		NewStatusEffect = NewEffect;
	}

	AbilityTarget GetTarget() const {return AddingTarget;}
	StatusEffect GetStatusEffect() const {return NewStatusEffect;}
	
	void SetTarget(AbilityTarget NewTarget) { AddingTarget = NewTarget;}
	void SetStatusEffect(StatusEffect NewEffect) { NewStatusEffect = NewEffect;}

	
};


struct AbilityDetermination
{
protected:

	AbilityTarget ComparingComparand = AbilityTarget::Ally;
	CharacterStatistic ComparandStatisticType = CharacterStatistic::HealthPoint;
	
	AbilityTarget ComparingReference = AbilityTarget::None;
	CharacterStatistic ReferenceStatisticType = CharacterStatistic::HealthPoint;

	bool IsRequireBiggerOrEqual = true;
	float CompareRequirement = 0;

public:

	AbilityDetermination()
	{
		
	}

	AbilityDetermination(AbilityTarget NewComparingComparand, CharacterStatistic NewComparandStatisticType,
		AbilityTarget NewComparingReference, CharacterStatistic NewReferenceStatisticType, bool NewIsRequireBiggerOrEqual, float NewCompareRequirement)
	{
		ComparingComparand = NewComparingComparand;
		ComparandStatisticType = NewComparandStatisticType;
		ComparingReference = NewComparingReference;
		ReferenceStatisticType = NewReferenceStatisticType;
		IsRequireBiggerOrEqual = NewIsRequireBiggerOrEqual;
		CompareRequirement = NewCompareRequirement;
	}

	AbilityTarget GetComparingComparandRequest() const {return ComparingComparand;}
	CharacterStatistic GetComparandStatisticTypeRequest() const {return ComparandStatisticType;}
	AbilityTarget GetComparingReferenceRequest() const {return ComparingComparand;}
	CharacterStatistic GetReferenceStatisticTypeRequest() const {return ReferenceStatisticType;}

	bool IsComparisonReturnTrue(int32 ComparandValue, int32 ReferenceValue)
	{
		if(IsRequireBiggerOrEqual)
		{
			return (ComparandValue - ReferenceValue) >= CompareRequirement;
		}
		
		return (ComparandValue - ReferenceValue) <= CompareRequirement;
	}
	
};

struct FCharacterAbility
{
protected:

	FString AbilityName = "";
	int32 PhysicalPower = 0;
	int32 MagicPower = 0;
	int32 ArmorDamage = 0;
	AbilityTarget Target = AbilityTarget::Enemy;
	int32 CastingRange = 4;
	int32 AffectingRange = 1;
	int32 HitRate = 100;
	TArray<AddingStatusEffect> AbilityStatusEffects;
	bool IsStatusEffectOnlyHappenIfConditionMet = false;
	
	// TODO: MAYBE THIS WILL GET ERROR
	//AbilityDetermination AbilityDeterminationRequest = AbilityDetermination();

	// FString NewAbilityName, int32 NewPhyPower, int32 NewMagPower, int32 NewArmorDamage, AbilityTarget NewTarget, int32 NewCastRange, int32 NewAffectRange, int32 NewHitRate, StatusEffect NewEffect
public:
	// default constructor
	FCharacterAbility()
	{
		
	}
	
	// constructor with parameter
	FCharacterAbility(FString NewAbilityName, int32 NewPhyPower, int32 NewMagPower, int32 NewArmorDamage,
		AbilityTarget NewTarget, int32 NewCastRange, int32 NewAffectRange, int32 NewHitRate, /*AbilityDetermination NewAbilityDetermination,*/ bool NewConditionMetCheckMark)
	{
		AbilityName = NewAbilityName, PhysicalPower = NewPhyPower,
		MagicPower = NewMagPower, ArmorDamage = NewArmorDamage, Target = NewTarget,
		CastingRange = NewCastRange, AffectingRange = NewAffectRange, HitRate = NewHitRate;
		//AbilityDeterminationRequest = NewAbilityDetermination;
		IsStatusEffectOnlyHappenIfConditionMet = NewConditionMetCheckMark;
	}

	// getter and setter
	FString GetAbilityName() const {return AbilityName;}
	int32 GetPhysicalPower() const {return PhysicalPower;}
	int32 GetMagicPower() const {return MagicPower;}
	int32 GetArmorDamage() const {return ArmorDamage;}
	AbilityTarget GetTarget() const {return Target;}
	int32 GetCastingRange() const {return CastingRange;}
	int32 GetAffectingRange() const {return AffectingRange;}
	int32 GetHitRate() const {return HitRate;}
	TArray<AddingStatusEffect> GetAbilityStatusEffects() const {return AbilityStatusEffects;}
	//AbilityDetermination GetAbilityDeterminationRequest() const {return AbilityDeterminationRequest;}
	bool GetConditionMetCheckMark() const {return IsStatusEffectOnlyHappenIfConditionMet;}
	
	void SetAbilityName(FString NewAbilityName) { AbilityName = NewAbilityName;}
	void SetPhysicalPower(int32 NewPhyPower) { PhysicalPower = NewPhyPower;}
	void SetMagicPower(int32 NewMagPower) { MagicPower = NewMagPower;}
	void SetArmorDamage(int32 NewArmorDamage) { ArmorDamage = NewArmorDamage;}
	void SetTarget(AbilityTarget NewTarget) { Target = NewTarget;}
	void SetCastingRange(int32 NewCastRange) { CastingRange = NewCastRange;}
	void SetAffectingRange(int32 NewAffectRange) { AffectingRange = NewAffectRange;}
	void SetHitRate(int32 NewHitRate) { HitRate = NewHitRate;}
	void AddAbilityStatusEffect(AddingStatusEffect NewEffect) { AbilityStatusEffects.Add(NewEffect);}
	void ClearAbilityStatusEffect() { AbilityStatusEffects.Empty();}
	//void SetAbilityDeterminationRequest(AbilityDetermination NewAbilityDetermination) { AbilityDeterminationRequest = NewAbilityDetermination;}
	void SetConditionMetCheckMark(bool NewConditionMetCheckMark) {IsStatusEffectOnlyHappenIfConditionMet = NewConditionMetCheckMark;}
};

struct CharacterStats
{
protected:

	CharacterClass CurrentClass = CharacterClass::Breaker;
	int32 HealthPoint = 100;
	int32 Armor = 100;
	int32 PhysicalAttack = 100;
	int32 PhysicalArmor = 100;
	int32 MagicAttack = 100;
	int32 MagicArmor = 100;
	int32 MovementSpeed = 6;
	int32 DodgeChance = 100;
	int32 HitRate = 100;
	int32 CriticalChance = 100;
	int32 CriticalDamage = 150;
	TArray<StatusEffect> CurrentStatusEffects;
	
public:
	// default constructor
	CharacterStats()
	{
		
	}

	// constructor with parameter
	CharacterStats(CharacterClass NewClass, int32 NewHealthPoint, int32 NewArmor, int32 NewPhysicalAttack, int32 NewPhysicalArmor,
		int32 NewMagicAttack, int32 NewMagicArmor, int32 NewMovementSpeed, int32 NewDodgeChance, int32 NewHitRate, int32 NewCriticalChance, int32 NewCriticalDamage )
	{
		CurrentClass = NewClass;
		HealthPoint = NewHealthPoint;
		Armor = NewArmor;
		PhysicalAttack = NewPhysicalAttack;
		PhysicalArmor = NewPhysicalArmor;
		MagicAttack = NewMagicAttack;
		MagicArmor = NewMagicArmor;
		MovementSpeed = NewMovementSpeed;
		DodgeChance = NewDodgeChance;
		HitRate = NewHitRate;
		CriticalChance = NewCriticalChance;
		CriticalDamage = NewCriticalDamage;
	}
	
	// getter and setter
	CharacterClass GetCurrentClass() const {return CurrentClass;}
	int32 GetHealthPoint() const {return HealthPoint;}
	int32 GetArmor() const {return Armor;}
	int32 GetPhysicalAttack() const {return PhysicalAttack;}
	int32 GetPhysicalArmor() const {return PhysicalArmor;}
	int32 GetMagicAttack() const {return MagicAttack;}
	int32 GetMagicArmor() const {return MagicArmor;}
	int32 GetMovementSpeed() const {return MovementSpeed;}
	int32 GetDodgeChance() const {return DodgeChance;}
	int32 GetHitRate() const {return HitRate;}
	int32 GetCriticalChance() const {return CriticalChance;}
	int32 GetCriticalDamage() const {return CriticalDamage;}
	TArray<StatusEffect> GetCurrentStatusEffects() const {return CurrentStatusEffects;}

	
	void SetCurrentClass(CharacterClass NewClass) { CurrentClass = NewClass;}
	void SetHealthPoint(int32 NewHealthPoint) { HealthPoint = NewHealthPoint;}
	void SetArmor(int32 NewArmor) { Armor = NewArmor;}
	void SetPhysicalAttack(int32 NewPhysicalAttack) { PhysicalAttack = NewPhysicalAttack;}
	void SetMagicAttack(int32 NewMagicAttack) { MagicAttack = NewMagicAttack;}
	void SetMagicArmor(int32 NewMagicArmor) { HealthPoint = NewMagicArmor;}
	void SetMovementSpeed(int32 NewMovementSpeed) { MovementSpeed = NewMovementSpeed;}
	void SetDodgeChance(int32 NewDodgeChance) { DodgeChance = NewDodgeChance;}
	void SetHitRate(int32 NewHitRate) { HitRate = NewHitRate;}
	void SetCriticalChance(int32 NewCriticalChance) { CriticalChance = NewCriticalChance;}
	void SetCriticalDamage(int32 NewCriticalDamage) { CriticalDamage = NewCriticalDamage;}
	void AddStatusEffect(StatusEffect AddingStatusEffect) { CurrentStatusEffects.Add(AddingStatusEffect); }

	// Adder, Remover, and getter from Array


	
};
