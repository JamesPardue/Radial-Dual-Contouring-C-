// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ProceduralMeshComponent.h"
#include "StaticMeshAttributes.h"
#include "DynamicMeshToMeshDescription.h"
#include "GeometryFramework/Public/Components/DynamicMeshComponent.h"
#include "DynamicMesh/MeshAttributeUtil.h"
#include "IndexTypes.h"
#include "DynamicMesh/MeshNormals.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "DynamicMesh/DynamicMeshAttributeSet.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DynamicMeshUtil.generated.h"

#pragma region ENUMS

enum MeshType
{
	Static,
	Procedural,
	Dynamic
};

#pragma endregion ENUMS

struct SimpleMeshData
{
	//Try switching this back to 3f
	TArray<FVector3d> Vertices;
	TArray<FVector3f> Normals;
	TArray<FVector2f> UV0;
	TArray<int> Triangles;

	SimpleMeshData()
	{
		Vertices = TArray<FVector3d, TInlineAllocator<25112>>();
		Normals = TArray<FVector3f, TInlineAllocator<25112>>();
		UV0 = TArray<FVector2f, TInlineAllocator<25112>>();
		Triangles = TArray<int, TInlineAllocator<351757>>();
	}

};

struct SimpleMeshDataProcedural
{
	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<int> Triangles;
};

UCLASS()
class TERRAUNKNOWN_API UDynamicMeshUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//Mesh Creation Functions
	static UE::Geometry::FDynamicMesh3 CreateDynamicMeshDataFromSimpleMeshData(SimpleMeshData NewMeshData, bool trigroup = false);

	static UStaticMesh* CreateStaticMeshDataFromSimpleMeshData(SimpleMeshData NewMeshData, bool trigroup = false);

	//Mesh Component Creation Functions
	static void CreateStaticMeshCompFromSimpleMeshData(SimpleMeshData NewMeshData, UStaticMeshComponent* StaticWorldMeshComponent, UMaterial* WorldMaterial, bool trigroup = false);

	static void CreateProceduralMeshCompFromSimpleMeshData(SimpleMeshData NewMeshData, UProceduralMeshComponent* ProceduralWorldMeshComponent, UMaterial* WorldMaterial, bool trigroup = false);

	static void CreateDynamicMeshCompFromSimpleMeshData(SimpleMeshData NewMeshData, UDynamicMeshComponent* DynamicWorldMeshComponent, UMaterial* WorldMaterial, bool trigroup = false);
};