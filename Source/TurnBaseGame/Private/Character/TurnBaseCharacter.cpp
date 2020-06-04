// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBaseCharacter.h"
#include "TurnBaseGameModeBase.h"


// Sets default values
ATurnBaseCharacter::ATurnBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OrderProcessComponent = CreateDefaultSubobject<UOrderProcessComponent>(TEXT("OrderProcessComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSet"));

	CurrentGameState = ETurnBasePlayState::EUnknow;
	bAbilitiesInitialized = false;
}

// Called when the game starts or when spawned
void ATurnBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (ATurnBaseGameModeBase* TestGameMode = Cast<ATurnBaseGameModeBase>(GetWorld()->GetAuthGameMode())) {
		TestGameMode->OnGameStateChange.AddDynamic(this, &ATurnBaseCharacter::OnGameStateChangeDelegate);
	}
}

void ATurnBaseCharacter::PossessedBy(AController* controller)
{
	Super::PossessedBy(controller);

	if (AbilitySystemComponent) {
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}
}

// Called every frame
void ATurnBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATurnBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATurnBaseCharacter::CameraMove(float XValue, float YValue){
}

float ATurnBaseCharacter::GetHealth() const
{
	return AttributeSet->GetHealth();
}

float ATurnBaseCharacter::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

int32 ATurnBaseCharacter::GetCharacterLevel() const
{
	return 1;
}

bool ATurnBaseCharacter::MoveToTargetLocation(bool bForce) {
	FVector CurrentLocation = GetActorLocation();

	if (abs(CurrentLocation.X - GridTargetLocation.X) < 10.f && abs(CurrentLocation.Y - GridTargetLocation.Y) < 10.f) return true;
	
	// find out which way is forward
	const FRotator Rotation = (GridTargetLocation - CurrentLocation).Rotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, 1.0f, bForce);
	return false;
}

void ATurnBaseCharacter::OrderBackspace()
{
}

void ATurnBaseCharacter::OnGameStateChangeDelegate(ETurnBasePlayState NewState)
{
	if (CurrentGameState == NewState) return;
	CurrentGameState = NewState;
}

void ATurnBaseCharacter::AddStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (!bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server	
		for (TSubclassOf<UGameplayAbility>& StartupAbility : GameplayAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetCharacterLevel(), INDEX_NONE, this));
		}

		// Now apply passives
		for (TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		bAbilitiesInitialized = true;
	}
}
