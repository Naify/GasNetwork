// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameDataTypes.h"
#include "Components/ActorComponent.h"
#include "FootstepComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GASNETWORK_API UFootstepComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFootstepComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	FName LeftFootSocketName = TEXT("LeftFootSocketName");
	UPROPERTY(EditDefaultsOnly)
	FName RightFootSocketName = TEXT("RightFootSocketName");
	
public:	
	void HandleFootStep(EFoot Foot);

		
};
