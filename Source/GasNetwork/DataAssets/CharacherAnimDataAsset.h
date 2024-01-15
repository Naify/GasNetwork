// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameDataTypes.h"
#include "Engine/DataAsset.h"
#include "CharacherAnimDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class GASNETWORK_API UCharacherAnimDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FCharacterAnimationData CharacterAnimationData;
};
