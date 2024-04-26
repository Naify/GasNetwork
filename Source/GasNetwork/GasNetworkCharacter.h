// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameDataTypes.h"
#include "GameFramework/Character.h"
#include "Abilities/GameplayAbility.h"
#include "ActorComponents/FootstepComponent.h"
#include "InputActionValue.h"
#include "GasNetworkCharacter.generated.h"

class UGASComponentBase;
class UGASAttributeSetBase;

class UGameplayEffect;
class UGameplayAbility;

UCLASS(config=Game)
class AGasNetworkCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:

	AGasNetworkCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void PostInitializeComponents() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float MouseSensitivity = 0.4f;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	bool ApplyEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	virtual void PawnClientRestart() override;
	
protected:

	void GiveAbilities();
	void ApplyStartEffects();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> DefaultAttributeSet;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GAS")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UGASComponentBase* AbilitySystemComponent;

	UPROPERTY(Transient)
	UGASAttributeSetBase* AttributeSet;
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	UPROPERTY(ReplicatedUsing=OnRep_CharacterData)
	FCharacterData CharacterData;

	UFUNCTION()
	void OnRep_CharacterData();

	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

	UPROPERTY(EditDefaultsOnly)
	class UCharacterDataAsset* CharacterDataAsset;	

	UPROPERTY(BlueprintReadOnly)
	class UFootstepComponent* FootstepComponent;

	//Input

	UPROPERTY(EditAnywhere)
	class UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere)
	class UInputAction* MoveForwardInputAction;
	
	UPROPERTY(EditAnywhere)
	class UInputAction* MoveSideInputAction;
	
	UPROPERTY(EditAnywhere)
	class UInputAction* TurnInputAction;
	
	UPROPERTY(EditAnywhere)
	class UInputAction* LookUpInputAction;
	
	UPROPERTY(EditAnywhere)
	class UInputAction* JumpInputAction;

	void OnMoveForwardAction(const FInputActionValue& Value);
	void OnMoveSideAction(const FInputActionValue& Value);
	void OnTurnAction(const FInputActionValue& Value);
	void OnLookUpAction(const FInputActionValue& Value);
	void OnJumpActionStarted(const FInputActionValue& Value);
	void OnJumpActionFinished(const FInputActionValue& Value);
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
	FCharacterData GetCharacterData() const;

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData InCharacterData);
		
	class UFootstepComponent* GetFootstepComponent() const;

	virtual void Landed(const FHitResult& Hit) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;
};

