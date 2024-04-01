// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldGen/DynamicMeshUtil.h"

//Mesh Creation Functions
UE::Geometry::FDynamicMesh3 UDynamicMeshUtil::CreateDynamicMeshDataFromSimpleMeshData(SimpleMeshData NewMeshData, bool trigroup)
{
	//Set Dynamic Mesh Data
	UE::Geometry::FDynamicMesh3 DynamicWorldMesh = UE::Geometry::FDynamicMesh3(true, true, true, trigroup);
	if (trigroup) DynamicWorldMesh.AllocateTriangleGroup();
	FVector3f DefaultColor = FVector3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < NewMeshData.Vertices.Num(); ++i)
	{
		UE::Geometry::FVertexInfo VertInfo = UE::Geometry::FVertexInfo(NewMeshData.Vertices[i], NewMeshData.Normals[i], DefaultColor, NewMeshData.UV0[i]);
		DynamicWorldMesh.AppendVertex(VertInfo);
	}
	for (int i = 0; i < NewMeshData.Triangles.Num(); i += 3)
	{
		if (trigroup)
			DynamicWorldMesh.AppendTriangle(NewMeshData.Triangles[i], NewMeshData.Triangles[i + 1], NewMeshData.Triangles[i + 2], 1);
		else
			DynamicWorldMesh.AppendTriangle(NewMeshData.Triangles[i], NewMeshData.Triangles[i + 1], NewMeshData.Triangles[i + 2]);
	}

	DynamicWorldMesh.EnableAttributes();
	UE::Geometry::CopyVertexUVsToOverlay(DynamicWorldMesh, *DynamicWorldMesh.Attributes()->PrimaryUV(), false);
	UE::Geometry::CopyVertexNormalsToOverlay(DynamicWorldMesh, *DynamicWorldMesh.Attributes()->PrimaryNormals());

	//DynamicWorldMesh.EnableVertexColors(DefaultColor);
	DynamicWorldMesh.Attributes()->EnablePrimaryColors();
	if (DynamicWorldMesh.Attributes()->PrimaryColors()->ElementCount() > 0)
	{
		DynamicWorldMesh.Attributes()->PrimaryColors()->ClearElements();
	}
	DynamicWorldMesh.Attributes()->PrimaryColors()->BeginUnsafeElementsInsert();
	for (int32 Vid : DynamicWorldMesh.VertexIndicesItr())
	{
		FVector3f Color = DynamicWorldMesh.GetVertexColor(Vid);
		DynamicWorldMesh.Attributes()->PrimaryColors()->InsertElement(Vid, &Color.X, true);
	}
	DynamicWorldMesh.Attributes()->PrimaryColors()->EndUnsafeElementsInsert();
	for (int32 Tid : DynamicWorldMesh.TriangleIndicesItr())
	{
		DynamicWorldMesh.Attributes()->PrimaryColors()->SetTriangle(Tid, DynamicWorldMesh.GetTriangle(Tid));
	}

	//UE_LOG(LogTemp, Warning, TEXT("Triangle grounps on? %s"), (DynamicWorldMesh.HasTriangleGroups() ? TEXT("True") : TEXT("False")));


	return DynamicWorldMesh;
}

UStaticMesh* UDynamicMeshUtil::CreateStaticMeshDataFromSimpleMeshData(SimpleMeshData NewMeshData, bool trigroup)
{
	UE::Geometry::FDynamicMesh3 DynamicWorldMesh = CreateDynamicMeshDataFromSimpleMeshData(NewMeshData, trigroup);

	UStaticMesh* StaticMesh = NewObject<UStaticMesh>();
	const FDynamicMesh3* Mesh = &DynamicWorldMesh;

	FMeshDescription MeshDescription;
	FStaticMeshAttributes StaticMeshAttributes(MeshDescription);
	StaticMeshAttributes.Register();

	FDynamicMeshToMeshDescription Converter;
	Converter.Convert(Mesh, MeshDescription);

	// Build the static mesh render data, one FMeshDescription* per LOD.
	TArray<const FMeshDescription*> MeshDescriptionPtrs;
	MeshDescriptionPtrs.Emplace(&MeshDescription);
	StaticMesh->BuildFromMeshDescriptions(MeshDescriptionPtrs);

	return StaticMesh;
}

//Mesh Component Creation Functions
void UDynamicMeshUtil::CreateStaticMeshCompFromSimpleMeshData(SimpleMeshData NewMeshData, UStaticMeshComponent* StaticWorldMeshComponent, UMaterial* WorldMaterial, bool trigroup)
{
	UStaticMesh* StaticMesh = CreateStaticMeshDataFromSimpleMeshData(NewMeshData, trigroup);

	StaticWorldMeshComponent->SetStaticMesh(StaticMesh);

	// add one material slot
	UMaterialInterface* WorldMaterialInterface = UMaterialInstanceDynamic::Create(WorldMaterial, StaticWorldMeshComponent);
	StaticMesh->AddMaterial(WorldMaterialInterface);

	UE_LOG(LogTemp, Warning, TEXT("Created Static Mesh Comp"));
}

void UDynamicMeshUtil::CreateProceduralMeshCompFromSimpleMeshData(SimpleMeshData NewMeshData, UProceduralMeshComponent* ProceduralWorldMeshComponent, UMaterial* WorldMaterial, bool trigroup)
{
	UE::Geometry::FDynamicMesh3 DynamicWorldMesh = CreateDynamicMeshDataFromSimpleMeshData(NewMeshData, trigroup);

	UProceduralMeshComponent* Component = ProceduralWorldMeshComponent;
	const FDynamicMesh3* Mesh = &DynamicWorldMesh;
	bool bUseFaceNormals = false;
	bool bInitializeUV0 = true;
	bool bInitializePerVertexColors = true;
	bool bCreateCollision = true;

	Component->ClearAllMeshSections();

	int32 NumTriangles = Mesh->TriangleCount();
	int32 NumVertices = NumTriangles * 3;

	TArray<FVector> Vertices = TArray<FVector>();
	TArray<FVector> Normals = TArray<FVector>();
	Vertices.SetNumUninitialized(NumVertices);
	Normals.SetNumUninitialized(NumVertices);

	UE::Geometry::FMeshNormals PerVertexNormals(Mesh);
	bool bUsePerVertexNormals = false;
	const UE::Geometry::FDynamicMeshNormalOverlay* NormalOverlay = nullptr;
	if (Mesh->HasAttributes() == false && bUseFaceNormals == false)
	{
		PerVertexNormals.ComputeVertexNormals();
		bUsePerVertexNormals = true;
		//UE_LOG(LogTemp, Warning, TEXT("Mesh->HasAttributes() == false"));
	}
	else if (Mesh->HasAttributes())
	{
		NormalOverlay = Mesh->Attributes()->PrimaryNormals();
		//UE_LOG(LogTemp, Warning, TEXT("Mesh->HasAttributes() == true"));
	}
	//UE_LOG(LogTemp, Warning, TEXT("PerVertexNormals? %s"), (bUsePerVertexNormals ? TEXT("True") : TEXT("False")));
	//UE_LOG(LogTemp, Warning, TEXT("NormalOverlay? %s"), (NormalOverlay ? TEXT("True") : TEXT("False")));
	//UE_LOG(LogTemp, Warning, TEXT("bUseFaceNormals? %s"), (bUseFaceNormals ? TEXT("True") : TEXT("False")));

	const UE::Geometry::FDynamicMeshUVOverlay* UVOverlay = (Mesh->HasAttributes()) ? Mesh->Attributes()->PrimaryUV() : nullptr;
	TArray<FVector2D> UV0 = TArray<FVector2D>();
	if (UVOverlay && bInitializeUV0)
	{
		UV0.SetNum(NumVertices);
	}

	TArray<FLinearColor> VtxColors = TArray<FLinearColor>();
	bool bUsePerVertexColors = false;
	if (bInitializePerVertexColors && Mesh->HasVertexColors())
	{
		VtxColors.SetNum(NumVertices);
		bUsePerVertexColors = true;
	}

	TArray<FProcMeshTangent> Tangents = TArray<FProcMeshTangent>();		// not supporting this for now

	TArray<int32> Triangles = TArray<int32>();
	Triangles.SetNumUninitialized(NumTriangles * 3);

	FVector3d Position[3];
	FVector3f Normal[3];
	FVector2f UV[3];
	int32 BufferIndex = 0;
	for (int32 tid : Mesh->TriangleIndicesItr())
	{
		int32 k = 3 * (BufferIndex++);

		UE::Geometry::FIndex3i TriVerts = Mesh->GetTriangle(tid);

		Mesh->GetTriVertices(tid, Position[0], Position[1], Position[2]);
		Vertices[k] = (FVector)Position[0];
		Vertices[k + 1] = (FVector)Position[1];
		Vertices[k + 2] = (FVector)Position[2];


		if (bUsePerVertexNormals)
		{
			Normals[k] = (FVector)PerVertexNormals[TriVerts.A];
			Normals[k + 1] = (FVector)PerVertexNormals[TriVerts.B];
			Normals[k + 2] = (FVector)PerVertexNormals[TriVerts.C];

			//UE_LOG(LogTemp, Warning, TEXT("Using PerVertexNormals"));
		}
		else if (NormalOverlay != nullptr && bUseFaceNormals == false)
		{
			NormalOverlay->GetTriElements(tid, Normal[0], Normal[1], Normal[2]);
			Normals[k] = (FVector)Normal[0];
			Normals[k + 1] = (FVector)Normal[1];
			Normals[k + 2] = (FVector)Normal[2];

			//UE_LOG(LogTemp, Warning, TEXT("Using NormalOverlay->GetTriElements"));
		}
		else
		{
			FVector3d TriNormal = Mesh->GetTriNormal(tid);
			Normals[k] = (FVector)TriNormal;
			Normals[k + 1] = (FVector)TriNormal;
			Normals[k + 2] = (FVector)TriNormal;

			//UE_LOG(LogTemp, Warning, TEXT("Using GetTriNormal"));
		}

		if (UVOverlay != nullptr && UVOverlay->IsSetTriangle(tid))
		{
			UVOverlay->GetTriElements(tid, UV[0], UV[1], UV[2]);
			UV0[k] = (FVector2D)UV[0];
			UV0[k + 1] = (FVector2D)UV[1];
			UV0[k + 2] = (FVector2D)UV[2];
		}

		if (bUsePerVertexColors)
		{
			VtxColors[k] = (FLinearColor)Mesh->GetVertexColor(TriVerts.A);
			VtxColors[k + 1] = (FLinearColor)Mesh->GetVertexColor(TriVerts.B);
			VtxColors[k + 2] = (FLinearColor)Mesh->GetVertexColor(TriVerts.C);
		}

		Triangles[k] = k;
		Triangles[k + 1] = k + 1;
		Triangles[k + 2] = k + 2;
	}

	Component->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VtxColors, Tangents, bCreateCollision);

	UMaterialInterface* WorldMaterialInterface = UMaterialInstanceDynamic::Create(WorldMaterial, ProceduralWorldMeshComponent);
	ProceduralWorldMeshComponent->SetMaterial(0, WorldMaterialInterface);

	//UE_LOG(LogTemp, Warning, TEXT("Created Procedral Mesh"));
}

void UDynamicMeshUtil::CreateDynamicMeshCompFromSimpleMeshData(SimpleMeshData NewMeshData, UDynamicMeshComponent* DynamicWorldMeshComponent, UMaterial* WorldMaterial, bool trigroup)
{
	UE::Geometry::FDynamicMesh3 DynamicWorldMesh = CreateDynamicMeshDataFromSimpleMeshData(NewMeshData, trigroup);

	DynamicWorldMeshComponent->GetDynamicMesh()->SetMesh(DynamicWorldMesh);
	UMaterialInterface* WorldMaterialInterface = UMaterialInstanceDynamic::Create(WorldMaterial, DynamicWorldMeshComponent);
	
	DynamicWorldMeshComponent->SetMaterial(0, WorldMaterialInterface);
	DynamicWorldMeshComponent->NotifyMeshUpdated();
	UE_LOG(LogTemp, Warning, TEXT("Created Dynamic Mesh"));
}


