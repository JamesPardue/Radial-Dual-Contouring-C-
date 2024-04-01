// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen/MeshGenerator.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AMeshGenerator::AMeshGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AMeshGenerator::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Mesh Generator Begin Play"));
	
	CreateVerticies();
	CreateTriangles();

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	ProceduralMesh->SetMaterial(0, Material);
}

// Called every frame
void AMeshGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("Mesh Generator Ticking"));
}

void AMeshGenerator::CreateVerticies()
{
	for (int X = 0; X <= XSize; ++X)
	{
		for (int Y = 0; Y <= YSize; ++Y)
		{
			Vertices.Add(FVector(X * Scale, Y * Scale, 0));
			UV0.Add(FVector2D(X * UVScale, Y * UVScale));

			DrawDebugSphere(GetWorld(), FVector(X * Scale, Y * Scale, 0), 25.0f, 16, FColor::Red, true, -1.0f, 0U, 0.0f);
		}
	}
}

void AMeshGenerator::CreateTriangles()
{
	int Vertex = 0;

	for (int X = 0; X < XSize; ++X)
	{
		for (int Y = 0; Y < YSize; ++Y)
		{
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 1);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 2);
			Triangles.Add(Vertex + YSize + 1);

			++Vertex;
		}
		++Vertex;
	}
}