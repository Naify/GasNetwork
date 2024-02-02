// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/FootstepComponent.h"
#include "Materials/GASNPhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "GasNetworkCharacter.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<int32> ShowFootsteps(
	TEXT("ShowDebugFootspeps"),
	0,
	TEXT("Debug about footsteps")
	TEXT(" 0:off/n")
	TEXT(" 1:off/n"),
	ECVF_Cheat);

// Sets default values for this component's properties
UFootstepComponent::UFootstepComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UFootstepComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UFootstepComponent::HandleFootStep(EFoot Foot)
{
	if (AGasNetworkCharacter* Character = Cast<AGasNetworkCharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			const int32 DebugShowFootsteps = ShowFootsteps.GetValueOnAnyThread();
			
			FHitResult HitResult;
			const FVector SocketLocation = Mesh->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
			const FVector Location = SocketLocation + FVector::UpVector * 20;

			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;
			QueryParams.AddIgnoredActor(Character);

			if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + FVector::UpVector * -50.f, ECollisionChannel::ECC_WorldStatic, QueryParams))
			{
				if (HitResult.bBlockingHit)
				{
					if (HitResult.PhysMaterial.Get())
					{
						UGASNPhysicalMaterial* PhysicalMaterial = Cast<UGASNPhysicalMaterial>(HitResult.PhysMaterial.Get());

						if (PhysicalMaterial)
						{
							UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootstepSound, Location, 1.f);
						}

						if (DebugShowFootsteps > 0)
						{
							DrawDebugString(GetWorld(), Location, PhysicalMaterial->GetName(), nullptr, FColor::White, 4.f);
						}
					}

					if (DebugShowFootsteps > 0)
					{
						DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
					}
				}
				else
				{
					if (DebugShowFootsteps > 0)
					{
						DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.f, FColor::Red, false, 4, 0, 1);
					}
				}
			}
			else
			{
				if (DebugShowFootsteps > 0)
				{
					DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.f, FColor::Red, false, 4, 0, 1);
					DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
				}
			}
		}		
	}
}

