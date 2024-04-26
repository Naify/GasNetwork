// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GASJump.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

UGASJump::UGASJump()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UGASJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{	
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	return Character->CanJump();
}

void UGASJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		ACharacter * Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
		Character->Jump();

		if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			FGameplayEffectContextHandle EffectHandle = AbilityComponent->MakeEffectContext();

			FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(JumpEffect, 1, EffectHandle);

			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				if (ActiveGEHandle.WasSuccessfullyApplied())
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to apply Jump effect! %s"), *GetNameSafe(Character));
				}
			}
		}
	}
}
