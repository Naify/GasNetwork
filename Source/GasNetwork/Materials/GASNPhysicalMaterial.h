// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GASNPhysicalMaterial.generated.h"

/**
 * 
 */
UCLASS()
class GASNETWORK_API UGASNPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicalMaterial")
	class USoundBase* FootstepSound = nullptr;
};
