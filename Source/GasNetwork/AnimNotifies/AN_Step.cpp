// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_Step.h"
#include "GasNetworkCharacter.h"
#include "ActorComponents/FootstepComponent.h"

void UAN_Step::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	check(MeshComp);

	AGasNetworkCharacter* Character = MeshComp ? Cast<AGasNetworkCharacter>(MeshComp->GetOwner()) : nullptr;
	if (Character)
	{
		if (UFootstepComponent* FootStepComponent =  Character->GetFootstepComponent())
		{
			FootStepComponent->HandleFootStep(Foot);
		}
	}
}
