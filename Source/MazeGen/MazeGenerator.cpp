#include "MazeGenerator.h"
#include "Components/InstancedStaticMeshComponent.h"

AMazeGenerator::AMazeGenerator()
{
    PrimaryActorTick.bCanEverTick = false;

    FloorISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloorISM"));
    RootComponent = FloorISM;

    WallISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallISM"));
    WallISM->SetupAttachment(RootComponent);
}

void AMazeGenerator::BeginPlay()
{
    Super::BeginPlay();
}

void AMazeGenerator::GenerateMaze()
{
    if (!FloorMesh || !WallMesh) return;

    FloorISM->SetStaticMesh(FloorMesh);
    WallISM->SetStaticMesh(WallMesh);

    FloorISM->ClearInstances();
    WallISM->ClearInstances();

    InitializeGrid();
    BuildMaze();

    float Scale = CellSize / 100.f;
    float WallThickness = Scale * 0.2f;
    float WallHeight = Scale * 2.5f;

    for (int32 Y = 0; Y < MazeHeight; Y++)
    {
        for (int32 X = 0; X < MazeWidth; X++)
        {
            int32 Index = GetIndex(X, Y);
            const FMazeCell& Cell = Grid[Index];

            FVector BasePos = GetCellPosition(X, Y);

            {
                FTransform T;
                T.SetLocation(BasePos);
                T.SetScale3D(FVector(Scale, Scale, 0.05f));
                FloorISM->AddInstance(T);
            }


            FVector Pos;
            FTransform T;

            if (Cell.bWallTop)
            {
                Pos = BasePos + FVector(0, CellSize * 0.5f, WallHeight * 50.f);

                T.SetLocation(Pos);
                T.SetScale3D(FVector(Scale, WallThickness, WallHeight));

                WallISM->AddInstance(T);
            }

            if (Cell.bWallBottom)
            {
                Pos = BasePos + FVector(0, -CellSize * 0.5f, WallHeight * 50.f);

                T.SetLocation(Pos);
                T.SetScale3D(FVector(Scale, WallThickness, WallHeight));

                WallISM->AddInstance(T);
            }

            if (Cell.bWallRight)
            {
                Pos = BasePos + FVector(CellSize * 0.5f, 0, WallHeight * 50.f);

                T.SetLocation(Pos);
                T.SetScale3D(FVector(WallThickness, Scale, WallHeight));

                WallISM->AddInstance(T);
            }

            if (Cell.bWallLeft)
            {
                Pos = BasePos + FVector(-CellSize * 0.5f, 0, WallHeight * 50.f);

                T.SetLocation(Pos);
                T.SetScale3D(FVector(WallThickness, Scale, WallHeight));

                WallISM->AddInstance(T);
            }
        }
    }
}

void AMazeGenerator::InitializeGrid()
{
    Grid.Empty();

    for (int32 Y = 0; Y < MazeHeight; Y++)
    {
        for (int32 X = 0; X < MazeWidth; X++)
        {
            Grid.Add(FMazeCell());
        }
    }
}

int32 AMazeGenerator::GetIndex(int32 X, int32 Y) const
{
    return Y * MazeWidth + X;
}


bool AMazeGenerator::IsInside(int32 X, int32 Y) const
{
    return X >= 0 && X < MazeWidth && Y >= 0 && Y < MazeHeight;
}


TArray<FIntPoint> AMazeGenerator::GetUnvisitedNeighbors(int32 X, int32 Y)
{
    TArray<FIntPoint> Result;

    const TArray<FIntPoint> Directions = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0}
    };

    for (const FIntPoint& Dir : Directions)
    {
        int32 NX = X + Dir.X;
        int32 NY = Y + Dir.Y;

        if (IsInside(NX, NY))
        {
            if (!Grid[GetIndex(NX, NY)].bVisited)
            {
                Result.Add({NX, NY});
            }
        }
    }

    return Result;
}

void AMazeGenerator::RemoveWall(int32 X1, int32 Y1, int32 X2, int32 Y2)
{
    FMazeCell& A = Grid[GetIndex(X1, Y1)];
    FMazeCell& B = Grid[GetIndex(X2, Y2)];

    if (X1 == X2)
    {
        if (Y1 < Y2)
        {
            A.bWallTop = false;
            B.bWallBottom = false;
        }
        else
        {
            A.bWallBottom = false;
            B.bWallTop = false;
        }
    }
    else if (Y1 == Y2)
    {
        if (X1 < X2)
        {
            A.bWallRight = false;
            B.bWallLeft = false;
        }
        else
        {
            A.bWallLeft = false;
            B.bWallRight = false;
        }
    }
}

void AMazeGenerator::BuildMaze()
{
    FRandomStream Rand(Seed);

    Stack.Empty();

    int32 StartX = 0;
    int32 StartY = 0;

    Grid[GetIndex(StartX, StartY)].bVisited = true;
    Stack.Add({StartX, StartY});

    while (Stack.Num() > 0)
    {
        FIntPoint Current = Stack.Last();

        TArray<FIntPoint> Neighbors = GetUnvisitedNeighbors(Current.X, Current.Y);

        if (Neighbors.Num() > 0)
        {
            int32 RandIndex = Rand.RandRange(0, Neighbors.Num() - 1);
            FIntPoint Next = Neighbors[RandIndex];

            RemoveWall(Current.X, Current.Y, Next.X, Next.Y);

            Grid[GetIndex(Next.X, Next.Y)].bVisited = true;
            Stack.Add(Next);
        }
        else
        {
            Stack.Pop();
        }
    }
}

FVector AMazeGenerator::GetCellPosition(int32 X, int32 Y) const
{
    return FVector(X * CellSize, Y * CellSize, 0.f);
}
