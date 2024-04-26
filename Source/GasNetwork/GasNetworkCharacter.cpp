// Copyright Epic Games, Inc. All Rights Reserved.

#include "GasNetworkCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ActorComponents/GasCharacterMovementComponent.h"
#include "DataAssets/CharacterDataAsset.h"
#include "GAS/Components/GASComponentBase.h"
#include "GAS/AttributeSets/GASAttributeSetBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"
#include "InputActionValue.h"

#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////
// AGasNetworkCharacter

AGasNetworkCharacter::AGasNetworkCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UGasCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	AbilitySystemComponent = CreateDefaultSubobject<UGASComponentBase>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UGASAttributeSetBase>(TEXT("AttributeSet"));

	FootstepComponent = CreateDefaultSubobject<UFootstepComponent>(TEXT("FootstepComponent"));
}

void AGasNetworkCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

UAbilitySystemComponent* AGasNetworkCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool AGasNetworkCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> Effect,
                                             FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get()) return false;

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);
	if (SpecHandle.IsValid())
	{		
		FActiveGameplayEffectHandle ActiveEffect = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		return ActiveEffect.WasSuccessfullyApplied();
	}
	
	return false;
}

void AGasNetworkCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* PLayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PLayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AGasNetworkCharacter::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (auto DefaultAbility : CharacterData.Abilities)
		{
			FGameplayAbilitySpec();
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void AGasNetworkCharacter::ApplyStartEffects()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);

		for (auto CharacterEffect : CharacterData.Effects)
		{
			ApplyEffectToSelf(CharacterEffect, EffectContextHandle);
		}		
	}
}

//server init
void AGasNetworkCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	GiveAbilities();
	ApplyStartEffects();
}

//client init
void AGasNetworkCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGasNetworkCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveForwardInputAction)
		{
			EnhancedInputComponent->BindAction(MoveForwardInputAction, ETriggerEvent::Triggered, this, &AGasNetworkCharacter::OnMoveForwardAction);
		}
		if (MoveSideInputAction)
		{
			EnhancedInputComponent->BindAction(MoveSideInputAction, ETriggerEvent::Triggered, this, &AGasNetworkCharacter::OnMoveSideAction);
		}
		if (TurnInputAction)
		{
			EnhancedInputComponent->BindAction(TurnInputAction, ETriggerEvent::Triggered, this, &AGasNetworkCharacter::OnTurnAction);
		}
		if (LookUpInputAction)
		{
			EnhancedInputComponent->BindAction(LookUpInputAction, ETriggerEvent::Triggered, this, &AGasNetworkCharacter::OnLookUpAction);
		}
		if (JumpInputAction)
		{
			EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &AGasNetworkCharacter::OnJumpActionStarted);
			EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Completed, this, &AGasNetworkCharacter::OnJumpActionFinished);
		}
	}
}

void AGasNetworkCharacter::OnMoveForwardAction(const FInputActionValue& Value)
{
	const float Magnitude = Value.GetMagnitude();
	if ((Controller != nullptr) && (Magnitude != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Magnitude);
	}
}

void AGasNetworkCharacter::OnMoveSideAction(const FInputActionValue& Value)
{
	const float Magnitude = Value.GetMagnitude();
	if ( (Controller != nullptr) && (Magnitude != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Magnitude);
	}
}

void AGasNetworkCharacter::OnTurnAction(const FInputActionValue& Value)
{
	AddControllerYawInput(Value.GetMagnitude() * MouseSensitivity * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AGasNetworkCharacter::OnLookUpAction(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value.GetMagnitude() * MouseSensitivity * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AGasNetworkCharacter::OnJumpActionStarted(const FInputActionValue& Value)
{
	Jump();
	FGameplayEventData Payload;

	Payload.Instigator = this;
	Payload.EventTag = JumpEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, JumpEventTag, Payload);
}

void AGasNetworkCharacter::OnJumpActionFinished(const FInputActionValue& Value)
{
	//StopJumping();
}

FCharacterData AGasNetworkCharacter::GetCharacterData() const
{
	return CharacterData;
}

void AGasNetworkCharacter::SetCharacterData(const FCharacterData InCharacterData)
{
	CharacterData = InCharacterData;

	InitFromCharacterData(CharacterData);
}

UFootstepComponent* AGasNetworkCharacter::GetFootstepComponent() const
{
	return FootstepComponent;
}

void AGasNetworkCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
	}
}

void AGasNetworkCharacter::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
}

void AGasNetworkCharacter::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData, true);
}

void AGasNetworkCharacter::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGasNetworkCharacter, CharacterData);
}