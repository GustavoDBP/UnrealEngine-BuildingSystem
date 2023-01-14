// Fill out your copyright notice in the Description page of Project Settings.


#include "AConstructorPawn.h"

// Sets default values
AConstructorPawn::AConstructorPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Default Scene Root"));

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(FName("Floating Pawn Movement"));
	FloatingPawnMovement->MaxSpeed = 2500;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(CameraBoom);

	WallConstructGuide = CreateDefaultSubobject<UStaticMeshComponent>(FName("Wall Construct Guide"));
	WallConstructGuide->SetVisibility(false);
}

void AConstructorPawn::BeginRotateCamera()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	PlayerController->SetShowMouseCursor(false);

	PlayerController->GetMousePosition(OldCursorPosition.X, OldCursorPosition.Y);

	FVector2d ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	PlayerController->SetMouseLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
}

void AConstructorPawn::EndRotateCamera()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	PlayerController->SetMouseLocation(OldCursorPosition.X, OldCursorPosition.Y);
	PlayerController->SetShowMouseCursor(true);
}

void AConstructorPawn::RotateCamera(const FInputActionValue& ActionValue)
{
	const UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	const bool bIsRotatingCamera = PlayerEnhancedInputComponent->GetBoundActionValue(InputAction_BeginRotateCamera).Get<
		bool>();

	if (!bIsRotatingCamera) return;

	const FVector2d Value = ActionValue.Get<FVector2d>();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	PlayerController->AddYawInput(Value.X);
	PlayerController->AddPitchInput(-Value.Y);

	FVector2d ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	PlayerController->SetMouseLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
}

void AConstructorPawn::MovePawn(const FInputActionValue& ActionValue)
{
	const FVector2d Value = ActionValue.Get<FVector2d>();

	FVector Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value.Y);

	Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value.X);
}

void AConstructorPawn::DoCameraZoom(const FInputActionValue& ActionValue)
{
	const float Value = FMath::Clamp(CameraBoom->TargetArmLength + ActionValue.Get<float>() * 10000, 100, 2500);
	CameraBoom->TargetArmLength = Value;
}

FVector AConstructorPawn::GetCursorLocationOnFloor() const
{
	const APlayerController* PlayerController = Cast<APlayerController>(Controller);

	FVector MouseWorldLocation;
	FVector MouseForwardVector;

	PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseForwardVector);

	const float HeightDifference = MouseWorldLocation.Z - FloorHeight;
	const float Cos = MouseForwardVector.Dot(FVector::DownVector);
	const float ForwardDistance = HeightDifference / Cos;

	return MouseForwardVector * ForwardDistance + MouseWorldLocation;
}

void AConstructorPawn::UpdateWallConstructGuideLocation() const
{
	const FVector CursorLocationOnFlor = GetCursorLocationOnFloor();
	WallConstructGuide->SetWorldLocation(CursorLocationOnFlor);
}

// Called when the game starts or when spawned
void AConstructorPawn::BeginPlay()
{
	Super::BeginPlay();

	SetFloorHeight(0);
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	PlayerController->SetShowMouseCursor(true);
}

// Called every frame
void AConstructorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Mode == WallConstruct) UpdateWallConstructGuideLocation();
}

// Called to bind functionality to input
void AConstructorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const APlayerController* PlayerController = Cast<APlayerController>(Controller);
	UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = PlayerController->GetLocalPlayer()->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>();
	check(PlayerEnhancedInputComponent && Subsystem);

	PlayerEnhancedInputComponent->BindAction(InputAction_BeginRotateCamera, ETriggerEvent::Started, this,
	                                         &AConstructorPawn::BeginRotateCamera);
	PlayerEnhancedInputComponent->BindAction(InputAction_BeginRotateCamera, ETriggerEvent::Completed, this,
	                                         &AConstructorPawn::EndRotateCamera);
	PlayerEnhancedInputComponent->BindAction(InputAction_RotateCamera, ETriggerEvent::Triggered, this,
	                                         &AConstructorPawn::RotateCamera);
	PlayerEnhancedInputComponent->BindAction(InputAction_MovePawn, ETriggerEvent::Triggered, this,
	                                         &AConstructorPawn::MovePawn);
	PlayerEnhancedInputComponent->BindAction(InputAction_CameraZoom, ETriggerEvent::Triggered, this,
	                                         &AConstructorPawn::DoCameraZoom);
	PlayerEnhancedInputComponent->BindActionValue(InputAction_BeginRotateCamera);

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(MappingContext, 0);
}

void AConstructorPawn::EnterWallConstructMode()
{
	Mode = WallConstruct;
	Cast<APlayerController>(Controller)->SetShowMouseCursor(false);
	WallConstructGuide->SetVisibility(true);
}

void AConstructorPawn::ExitWallConstructMode()
{
	Mode = None;
	Cast<APlayerController>(Controller)->SetShowMouseCursor(true);
	WallConstructGuide->SetVisibility(false);
}

void AConstructorPawn::SetFloorHeight(const float Value)
{
	FloorHeight = Value;

	const FVector OldLocation = GetActorLocation();

	SetActorLocation(FVector(OldLocation.X, OldLocation.Y, Value + 50));
}
