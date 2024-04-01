// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshGenerator.generated.h"


class UProceduralMeshComponent;
class UMaterialInterface;

UCLASS()
class TERRAUNKNOWN_API AMeshGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeshGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int XSize = 10;
	int YSize = 10;
	float Scale = 100.0f;
	float UVScale = 1.0f;

	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UV0;
	UMaterialInterface* Material;
	UProceduralMeshComponent* ProceduralMesh;

	void CreateVerticies();
	void CreateTriangles();
};
