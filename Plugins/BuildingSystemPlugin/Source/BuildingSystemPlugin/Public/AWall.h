// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "ProceduralMeshComponent.h"
#include "AWall.generated.h"

UCLASS(BlueprintType)
class BUILDINGSYSTEMPLUGIN_API AAWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAWall();

protected:
	UPROPERTY(EditAnywhere)
	USplineComponent* Spline;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* CurvePivot;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	float MaxPartLength = 100;
	UPROPERTY(EditAnywhere)
	float Height = 300;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetStartPoint(const FVector& StartPoint, const bool bRegenerate = true);
	UFUNCTION(BlueprintCallable)
	void SetEndPoint(const FVector& EndPoint, const bool bRegenerate = true);

	UFUNCTION(BlueprintCallable)
	void Regenerate();
	UFUNCTION(BlueprintCallable)
	void SetCurvePivotLocation(const FVector NewLocation, const bool bRegenerate = true);
private:
	UPROPERTY()
	TArray<UProceduralMeshComponent*> Parts;
	
	UFUNCTION()
	int GetPartsCount();
	UFUNCTION()
	void RegeneratePart(const int PartIndex);
	UFUNCTION()
	FVector GetPartLocation(const int PartIndex);
	UFUNCTION()
	float GetPartLength();
};
