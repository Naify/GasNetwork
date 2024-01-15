// Fill out your copyright notice in the Description page of Project Settings.
#include "AnimInstances/GasAnimInstance.h"

#include "GasNetworkCharacter.h"
#include "GameDataTypes.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/BlendSpace.h"
#include "DataAssets/CharacterDataAsset.h"
#include "DataAssets/CharacherAnimDataAsset.h"

UBlendSpace* UGasAnimInstance::GetLocomotionBlendspace() const
{
	if (AGasNetworkCharacter* Character = Cast<AGasNetworkCharacter>(GetOwningActor()))
	{
		FCharacterData Data = Character->GetCharacterData();

		if (Data.CharAnimDataAsset)
		{
			return Data.CharAnimDataAsset->CharacterAnimationData.MovementBlendspace;
		}
	}
	return DefaultCharAnimDataAsset ? DefaultCharAnimDataAsset->CharacterAnimationData.MovementBlendspace : nullptr;
}

UAnimSequenceBase* UGasAnimInstance::GetIdleAnimation() const
{
	if (AGasNetworkCharacter* Character = Cast<AGasNetworkCharacter>(GetOwningActor()))
	{
		FCharacterData Data = Character->GetCharacterData();

		if (Data.CharAnimDataAsset)
		{
			return Data.CharAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}
	}
	return DefaultCharAnimDataAsset ? DefaultCharAnimDataAsset->CharacterAnimationData.IdleAnimationAsset : nullptr;
}
