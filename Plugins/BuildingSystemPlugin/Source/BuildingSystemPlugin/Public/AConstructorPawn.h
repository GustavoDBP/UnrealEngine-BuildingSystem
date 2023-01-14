// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enums/EConstructorPawnModes.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "AConstructorPawn.generated.h"

UCLASS()
class BUILDINGSYSTEMPLUGIN_API AConstructorPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AConstructorPawn();

private:
	FVector2d OldCursorPosition;
	
	void BeginRotateCamera();
	void EndRotateCamera();
	void RotateCamera(const FInputActionValue& ActionValue);

	void MovePawn(const FInputActionValue& ActionValue);

	void DoCameraZoom(const FInputActionValue& ActionValue);
	
	UFUNCTION(BlueprintCallable)
	FVector GetCursorLocationOnFloor() const;
	
	UFUNCTION(BlueprintCallable)
	void UpdateWallConstructGuideLocation() const;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	float FloorHeight;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TEnumAsByte<EConstructorPawnModes> Mode = None;

	UPROPERTY(EditDefaultsOnly)
	UFloatingPawnMovement* FloatingPawnMovement;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USpringArmComponent* CameraBoom;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* WallConstructGuide;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere)
	UInputAction* InputAction_BeginRotateCamera;
	UPROPERTY(EditAnywhere)
	UInputAction* InputAction_RotateCamera;
	UPROPERTY(EditAnywhere)
	UInputAction* InputAction_MovePawn;
	UPROPERTY(EditAnywhere)
	UInputAction* InputAction_CameraZoom;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void EnterWallConstructMode();
	UFUNCTION(BlueprintCallable)
	void ExitWallConstructMode();

	UFUNCTION(BlueprintCallable)
	void SetFloorHeight(const float Value);
};
