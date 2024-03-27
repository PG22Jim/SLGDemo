// Copyright © 2024 by Jim Chen. All rights reserved.


#pragma once

UENUM(BlueprintType)
enum class CharacterTeam: uint8
{
	Player = 0 UMETA(DisplayName = "Player"),
	Enemy = 1 UMETA(DisplayName = "Enemy"),
};

UENUM(BlueprintType)
enum class CharacterStatistic : uint8
{
	HealthPoint = 0 UMETA(DisplayName = "HealthPoint"),
	Armor = 1 UMETA(DisplayName = "Armor"),
	PhysicalAttack = 2 UMETA(DisplayName = "PhysicalAttack"),
	PhysicalArmor = 3 UMETA(DisplayName = "PhysicalArmor"),
	MagicAttack = 4 UMETA(DisplayName = "MagicAttack"),
	MagicArmor = 5 UMETA(DisplayName = "MagicArmor"),
	MovementSpeed = 6 UMETA(DisplayName = "MovementSpeed"),
	DodgeChance = 7 UMETA(DisplayName = "DodgeChance"),
	HitRate = 8 UMETA(DisplayName = "HitRate"),
	CriticalChance = 9 UMETA(DisplayName = "CriticalChance"),
	CriticalDamage = 10 UMETA(DisplayName = "CriticalDamage"),
	DamageBoost = 11 UMETA(DisplayName = "DamageBoost"),
	DamageReduction = 12 UMETA(DisplayName = "DamageReduction"),
};

UENUM(BlueprintType)
enum class CharacterClass : uint8
{
	Breaker = 0 UMETA(DisplayName = "Breaker"),
	Smasher = 1 UMETA(DisplayName = "Smasher"),
	Sniper = 2 UMETA(DisplayName = "Sniper"),
	Ranger = 3 UMETA(DisplayName = "Ranger"),
	Mage = 4 UMETA(DisplayName = "Mage"),
	Supporter = 5 UMETA(DisplayName = "Supporter"),
	Warlock = 6 UMETA(DisplayName = "Warlock"),
};

UENUM(BlueprintType)
enum class AbilityTarget: uint8
{
	Ally = 0 UMETA(DisplayName = "Ally"),
	Enemy = 1 UMETA(DisplayName = "Enemy"),
	Self = 2 UMETA(DisplayName = "Self"),
	None = 3 UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class BuffDebuffType: uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Buff = 1 UMETA(DisplayName = "Buff"),
	Debuff = 2 UMETA(DisplayName = "Debuff"),
};

UENUM(BlueprintType)
enum class CharacterAvailability: uint8
{
	Controlling = 0 UMETA(DisplayName = "Controlling"),
	Available = 1 UMETA(DisplayName = "Available"),
	Unavailable = 2 UMETA(DisplayName = "Unavailable"),
};

