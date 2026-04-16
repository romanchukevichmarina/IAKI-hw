#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeGenerator.generated.h"

USTRUCT(BlueprintType)
struct FMazeCell
{
    GENERATED_BODY()

    bool bVisited = false;

    bool bWallTop = true;
    bool bWallRight = true;
    bool bWallBottom = true;
    bool bWallLeft = true;
};

UCLASS()
class MAZEGEN_API AMazeGenerator : public AActor
{
    GENERATED_BODY()

public:
    AMazeGenerator();

protected:
    virtual void BeginPlay() override;

public:

    UPROPERTY(EditAnywhere)
    int32 MazeWidth = 20;

    UPROPERTY(EditAnywhere)
    int32 MazeHeight = 20;

    UPROPERTY(EditAnywhere)
    float CellSize = 300.f;

    UPROPERTY(EditAnywhere)
    int32 Seed = 123;

    UPROPERTY(EditAnywhere)
    UStaticMesh* FloorMesh;

    UPROPERTY(EditAnywhere)
    UStaticMesh* WallMesh;

    UPROPERTY()
    UInstancedStaticMeshComponent* FloorISM;

    UPROPERTY()
    UInstancedStaticMeshComponent* WallISM;

    UFUNCTION(CallInEditor)
    void GenerateMaze();

private:

    TArray<FMazeCell> Grid;
    TArray<FIntPoint> Stack;

    void InitializeGrid();
    int32 GetIndex(int32 X, int32 Y) const;
    bool IsInside(int32 X, int32 Y) const;
    TArray<FIntPoint> GetUnvisitedNeighbors(int32 X, int32 Y);
    void RemoveWall(int32 X1, int32 Y1, int32 X2, int32 Y2);
    void BuildMaze();
    FVector GetCellPosition(int32 X, int32 Y) const;
};
