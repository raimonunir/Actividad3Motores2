// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actividad3Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AActividad3Character

AActividad3Character::AActividad3Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = DefaultJumpZVelocity; 
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// ---------- CAMERA TOP DOWN ----------
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.0f; // The camera follows at this distance behind the character	
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f)); // top down view
	CameraBoom->bUsePawnControlRotation = false; // Don't rotate the arm based on the controller
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AActividad3Character::ToggleCameraMode()
{
	if (bIsTopDownCamera)
	{
		CameraBoom->TargetArmLength = ThirdPersonArmLength;
		CameraBoom->SetRelativeRotation(ThirdPersonRotation);
		CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
		CameraBoom->bInheritPitch = true;
		CameraBoom->bInheritRoll = true;
		CameraBoom->bInheritYaw = true;
		bUseCameraRelativeMovement = true;

		// Rotate camera to character
		FRotator TargetRotation = CameraBoom->GetComponentRotation();
		TargetRotation.Pitch = 0.f;
		TargetRotation.Roll = 0.f;

		SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 10.f));
	}
	else
	{
		CameraBoom->TargetArmLength = TopDownArmLength;
		CameraBoom->SetRelativeRotation(TopDownRotation);
		CameraBoom->bUsePawnControlRotation = false; // Don't rotate the arm based on the controller
		CameraBoom->bInheritPitch = false;
		CameraBoom->bInheritRoll = false;
		CameraBoom->bInheritYaw = false;
		bUseCameraRelativeMovement = false;
	}

	bIsTopDownCamera = !bIsTopDownCamera; 
}


void AActividad3Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AActividad3Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AActividad3Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AActividad3Character::Look);

		// Toogle TopDown
		EnhancedInputComponent->BindAction(
			ToggleCameraAction,
			ETriggerEvent::Triggered,
			this,
			&AActividad3Character::ToggleCameraMode
		);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AActividad3Character::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		FVector Forward, Right;

		if (bUseCameraRelativeMovement)
		{
			// Movimiento relativo a la cámara
			FRotator Rotation = CameraBoom->GetComponentRotation();
			Rotation.Pitch = 0.f;
			Rotation.Roll = 0.f;

			Forward = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
			Right = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
		}
		else
		{
			// Movimiento relativo al mundo
			Forward = FVector::ForwardVector;
			Right = FVector::RightVector;
		}

		AddMovementInput(Forward, MovementVector.Y);
		AddMovementInput(Right, MovementVector.X);
	}
}

void AActividad3Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}