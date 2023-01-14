// Fill out your copyright notice in the Description page of Project Settings.


#include "AWall.h"

// Sets default values
AAWall::AAWall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root Scene Component");

	Spline = CreateDefaultSubobject<USplineComponent>(FName("Spline"));
	Spline->SetupAttachment(RootComponent);
	Spline->SetRelativeLocation(FVector(0, 0, 0));

	CurvePivot = CreateDefaultSubobject<USceneComponent>(FName("Pivot"));
	CurvePivot->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAWall::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAWall::SetStartPoint(const FVector& StartPoint, const bool bRegenerate)
{
	SetActorLocation(StartPoint);
	
	if(bRegenerate) Regenerate();
}

void AAWall::SetEndPoint(const FVector& EndPoint, const bool bRegenerate)
{
	Spline->SetLocationAtSplinePoint(1, EndPoint, ESplineCoordinateSpace::World);

	if(bRegenerate) Regenerate();
}

void AAWall::Regenerate()
{
	for (auto Part : Parts)
	{
		Part->DestroyComponent();
	}

	Parts.Empty();
	
	for (int PartIndex = 0; PartIndex < GetPartsCount(); PartIndex++)
	{
		RegeneratePart(PartIndex);
	}
}

void AAWall::SetCurvePivotLocation(const FVector NewLocation, const bool bRegenerate)
{
	CurvePivot->SetWorldLocation(NewLocation);
	const FVector PivotLocation = CurvePivot->GetRelativeLocation();
	const FVector EndPointLocation = Spline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local);
	const FVector StartPointTangent = FVector(PivotLocation.X * 2, PivotLocation.Y * 2, PivotLocation.Z * 2);
	const FVector EndPointTangent = FVector((EndPointLocation.X - PivotLocation.X) * 2,
											(EndPointLocation.Y - PivotLocation.Y) * 2, PivotLocation.Z * 2);
	Spline->SetTangentAtSplinePoint(0, StartPointTangent, ESplineCoordinateSpace::Local, true);
	Spline->SetTangentAtSplinePoint(1, EndPointTangent, ESplineCoordinateSpace::Local, true);

	if(bRegenerate) Regenerate();
}

int AAWall::GetPartsCount()
{
	return floor(Spline->GetSplineLength() / MaxPartLength) + 1;
}

void AAWall::RegeneratePart(const int PartIndex)
{
	const bool bIsLastPart = GetPartsCount() == PartIndex + 1;
	const FVector StartPoint = GetPartLocation(PartIndex);
	const FVector EndPoint = bIsLastPart
		                         ? Spline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local)
		                         : GetPartLocation(PartIndex + 1);

	TArray<FVector> Vertices;
	Vertices.Add(FVector(StartPoint.X, StartPoint.Y, StartPoint.Z));
	Vertices.Add(FVector(EndPoint.X, EndPoint.Y, EndPoint.Z));
	Vertices.Add(FVector(EndPoint.X, EndPoint.Y, EndPoint.Z + Height));
	Vertices.Add(FVector(StartPoint.X, StartPoint.Y, StartPoint.Z + Height));
	TArray<int> Triangles;
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);
	Triangles.Add(2);
	Triangles.Add(3);
	Triangles.Add(0);
	TArray<FVector> Normals;
	TArray<FVector2d> UV0;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	UProceduralMeshComponent* NewPart = Cast<UProceduralMeshComponent>(
		AddComponentByClass(UProceduralMeshComponent::StaticClass(), false, FTransform::Identity, false));

	NewPart->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);
	Parts.Add(NewPart);
}

FVector AAWall::GetPartLocation(const int PartIndex)
{
	const float Distance = GetPartLength() * PartIndex;
	return Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
}

float AAWall::GetPartLength()
{
	return Spline->GetSplineLength() / GetPartsCount();
}
