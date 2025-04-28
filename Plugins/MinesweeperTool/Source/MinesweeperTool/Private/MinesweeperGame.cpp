#include "MinesweeperGame.h"
#include "MinesweeperTile.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/DefaultValueHelper.h"

#define LOCTEXT_NAMESPACE "Minesweeper"

void SMinesweeperGame::Construct(const FArguments& InArgs)
{
	// Default game settings
	Width = FMath::Clamp(10, 5, 50);
    Height = FMath::Clamp(10, 5, 50);
    BombCount = FMath::Clamp(15, 1, Width * Height - 1);  // Ensure at least 1 non-bomb tile
    RevealedTiles = 0;
    bGameOver = false;

    ChildSlot
    [
        SNew(SVerticalBox)
        
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SHorizontalBox)
          
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("WidthLabel", "Width:"))
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SAssignNew(WidthInput, SEditableTextBox)
                .MinDesiredWidth(50)
                .Text(FText::FromString(FString::FromInt(Width)))
                .OnTextCommitted_Lambda([this](const FText&, ETextCommit::Type)
                {
                    // Validate input immediately
                    int32 NewWidth;
                    if (FDefaultValueHelper::ParseInt(WidthInput->GetText().ToString(), NewWidth))
                    {
                        NewWidth = FMath::Clamp(NewWidth, 5, 30);
                        WidthInput->SetText(FText::FromString(FString::FromInt(NewWidth)));
                    }
                })
            ]
            
            
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("HeightLabel", "Height:"))
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SAssignNew(HeightInput, SEditableTextBox)
                .MinDesiredWidth(50)
                .Text(FText::FromString(FString::FromInt(Height)))
                .OnTextCommitted_Lambda([this](const FText&, ETextCommit::Type)
                {
                    int32 NewHeight;
                    if (FDefaultValueHelper::ParseInt(HeightInput->GetText().ToString(), NewHeight))
                    {
                        NewHeight = FMath::Clamp(NewHeight, 5, 30);
                        HeightInput->SetText(FText::FromString(FString::FromInt(NewHeight)));
                    }
                })
            ]
            
            
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("BombsLabel", "Bombs:"))
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SAssignNew(BombCountInput, SEditableTextBox)
                .MinDesiredWidth(50)
                .Text(FText::FromString(FString::FromInt(BombCount)))
                .OnTextCommitted_Lambda([this](const FText&, ETextCommit::Type)
                {
                    int32 NewBombCount;
                    if (FDefaultValueHelper::ParseInt(BombCountInput->GetText().ToString(), NewBombCount))
                    {
                        int32 MaxBombs = Width * Height - 1;  // Always leave at least 1 non-bomb
                        NewBombCount = FMath::Clamp(NewBombCount, 1, MaxBombs);
                        BombCountInput->SetText(FText::FromString(FString::FromInt(NewBombCount)));
                    }
                })
            ]
            
            
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SAssignNew(StartButton, SButton)
                .Text(LOCTEXT("StartGame", "Start Game"))
                .OnClicked_Lambda([this]()
                {
                    int32 NewWidth = FMath::Clamp(FCString::Atoi(*WidthInput->GetText().ToString()), 5, 30);
                    int32 NewHeight = FMath::Clamp(FCString::Atoi(*HeightInput->GetText().ToString()), 5, 30);
                    int32 MaxBombs = NewWidth * NewHeight - 1;
                    int32 NewBombCount = FMath::Clamp(FCString::Atoi(*BombCountInput->GetText().ToString()), 1, MaxBombs);
                    
                    // Update UI to reflect clamped values
                    WidthInput->SetText(FText::FromString(FString::FromInt(NewWidth)));
                    HeightInput->SetText(FText::FromString(FString::FromInt(NewHeight)));
                    BombCountInput->SetText(FText::FromString(FString::FromInt(NewBombCount)));
                    
                    InitializeGame(NewWidth, NewHeight, NewBombCount);
                    return FReply::Handled();
                })
            ]
        ]
        
        
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SAssignNew(GameStatusText, STextBlock)
            .Text(LOCTEXT("GameStatus", "Game Status: Ready"))
            .Justification(ETextJustify::Center)
        ]
        
        
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .Padding(5)
        [
            SAssignNew(ScrollBox, SScrollBox)
            + SScrollBox::Slot()
            [
                SAssignNew(ContentBox, SBox)
                .MinDesiredWidth(Width * 30)
                .MinDesiredHeight(Height * 30)
            ]
        ]
    ];

    
    InitializeGame(Width, Height, BombCount);
}

void SMinesweeperGame::InitializeGame(int32 InWidth, int32 InHeight, int32 InBombCount)
{
	Width = FMath::Clamp(InWidth, 5, 30);
	Height = FMath::Clamp(InHeight, 5, 30);
	BombCount = FMath::Clamp(InBombCount, 1, Width * Height - 1);

	RevealedTiles = 0;
	bGameOver = false;

	// Update UI inputs
	WidthInput->SetText(FText::FromString(FString::FromInt(Width)));
	HeightInput->SetText(FText::FromString(FString::FromInt(Height)));
	BombCountInput->SetText(FText::FromString(FString::FromInt(BombCount)));

	GameStatusText->SetText(LOCTEXT("GameStatusReady", "Game Status: Ready"));

	// Clear existing grid
	Grid.Empty();
	Grid.SetNum(Width);

	// Create bomb positions
	TArray<FIntPoint> AllPossiblePositions;
	for (int32 X = 0; X < Width; X++)
	{
		for (int32 Y = 0; Y < Height; Y++)
		{
			AllPossiblePositions.Add(FIntPoint(X, Y));
		}
	}

	// Shuffle and take first BombCount elements
	for (int32 i = 0; i < AllPossiblePositions.Num() - 1; i++)
	{
		int32 j = FMath::RandRange(i, AllPossiblePositions.Num() - 1);
		AllPossiblePositions.Swap(i, j);
	}
	TArray<FIntPoint> BombPositions = TArray<FIntPoint>(AllPossiblePositions.GetData(), BombCount);

	// Debug log bomb positions
	for (const FIntPoint& Pos : BombPositions)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bomb at: %d,%d"), Pos.X, Pos.Y);
	}

	// Create grid panel
	TSharedPtr<SUniformGridPanel> GridPanel = SNew(SUniformGridPanel);

	for (int32 X = 0; X < Width; X++)
	{
		Grid[X].SetNum(Height);

		for (int32 Y = 0; Y < Height; Y++)
		{
			bool bIsBomb = BombPositions.Contains(FIntPoint(X, Y));
            
			// Debug log each tile's bomb status
			UE_LOG(LogTemp, Warning, TEXT("Creating Tile at %d,%d - Bomb: %d"), X, Y, bIsBomb);

			Grid[X][Y] = SNew(SMinesweeperTile)
				.X(X)
				.Y(Y)
				.IsBomb(bIsBomb)
				.Game(SharedThis(this));

			GridPanel->AddSlot(X, Y)
			[
				Grid[X][Y].ToSharedRef()
			];
		}
	}
	
	if (ContentBox.IsValid())
	{
		ContentBox->SetContent(GridPanel.ToSharedRef());
	}

	// if (TSharedPtr<SBox> ContentBox = StaticCastSharedPtr<SBox>(StaticCastSharedPtr<SScrollBox>(StaticCastSharedPtr<SVerticalBox>(ChildSlot.GetWidget())->GetSlot(2).GetWidget())->GetChildren()->GetChildAt(0)))
	// {
	// 	ContentBox->SetContent(GridPanel.ToSharedRef());
	// }
	
}

void SMinesweeperGame::RevealTile(int32 X, int32 Y)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to reveal tile at %d,%d"), X, Y);

	if (bGameOver)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game is already over"));
		return;
	}

	if (!IsValidTile(X, Y))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid tile position %d,%d"), X, Y);
		return;
	}

	if (Grid[X][Y]->IsRevealed())
	{
		UE_LOG(LogTemp, Warning, TEXT("Tile already revealed at %d,%d"), X, Y);
		return;
	}

	
	Grid[X][Y]->Reveal();

	// Immediate game over check if this is a bomb
	if (Grid[X][Y]->IsBomb())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit bomb at %d,%d"), X, Y);
		GameOver(false);
		return;  // Important: return immediately
	}

	RevealedTiles++;
	UE_LOG(LogTemp, Warning, TEXT("Revealed tiles count: %d"), RevealedTiles);

	// Check for win condition
	if (RevealedTiles == (Width * Height - BombCount))
	{
		UE_LOG(LogTemp, Warning, TEXT("All non-bomb tiles revealed - WIN!"));
		GameOver(true);
		return;
	}

	// If no adjacent bombs, reveal adjacent tiles
	int32 AdjacentBombs = CountAdjacentBombs(X, Y);
	UE_LOG(LogTemp, Warning, TEXT("Adjacent bombs: %d"), AdjacentBombs);
    
	if (AdjacentBombs == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Revealing adjacent tiles"));
		//RevealAdjacentTiles(X, Y);
		RevealAdjacentTilesImmediate(X, Y);
	}
}

void SMinesweeperGame::RevealAdjacentTiles(int32 X, int32 Y)
{
	for (int32 DX = -1; DX <= 1; DX++)
	{
		for (int32 DY = -1; DY <= 1; DY++)
		{
			if (DX == 0 && DY == 0) continue;

			int32 NewX = X + DX;
			int32 NewY = Y + DY;

			if (IsValidTile(NewX, NewY))
			{
				Grid[NewX][NewY]->SetHighlight(true);
				FTimerHandle UnusedHandle;
				GWorld->GetTimerManager().SetTimer(UnusedHandle, 
					[this, NewX, NewY]() {
						if (IsValidTile(NewX, NewY)) {
							Grid[NewX][NewY]->SetHighlight(false);
							RevealTile(NewX, NewY);
						}
					}, 
					0.05f, false);
			}
		}
	}
}

void SMinesweeperGame::RevealAdjacentTilesImmediate(int32 X, int32 Y)
{
	for (int32 DX = -1; DX <= 1; DX++)
	{
		for (int32 DY = -1; DY <= 1; DY++)
		{
			if (DX == 0 && DY == 0) continue;

			int32 NewX = X + DX;
			int32 NewY = Y + DY;

			if (IsValidTile(NewX, NewY) && !Grid[NewX][NewY]->IsRevealed())
			{
				RevealTile(NewX, NewY);
			}
		}
	}
}

void SMinesweeperGame::GameOver(bool bWon)
{
	bGameOver = true;

	if (bWon)
	{
		GameStatusText->SetText(LOCTEXT("GameWon", "Game Status: You Won!"));
		// Reveal all bombs in different color to show win
		for (int32 X = 0; X < Width; X++)
		{
			for (int32 Y = 0; Y < Height; Y++)
			{
				if (Grid[X][Y]->IsBomb())
				{
					Grid[X][Y]->RevealAsWin();
				}
			}
		}
	}
	else
	{
		GameStatusText->SetText(LOCTEXT("GameLost", "Game Status: Game Over!"));
        
		// Reveal all bombs and show incorrect flags
		for (int32 X = 0; X < Width; X++)
		{
			for (int32 Y = 0; Y < Height; Y++)
			{
				if (Grid[X][Y]->IsBomb())
				{
					Grid[X][Y]->Reveal();
				}
				else if (Grid[X][Y]->IsFlagged())
				{
					Grid[X][Y]->ShowIncorrectFlag();
				}
			}
		}
	}
}

void SMinesweeperGame::ResetGame()
{
	InitializeGame(Width, Height, BombCount);
}

int32 SMinesweeperGame::CountAdjacentBombs(int32 X, int32 Y) const
{
	int32 Count = 0;

	// Check all 8 surrounding tiles
	const int32 Directions[8][2] = {{-1,-1}, {-1,0}, {-1,1},
								   {0,-1},          {0,1},
								   {1,-1},  {1,0},  {1,1}};

	for (const auto& Dir : Directions)
	{
		int32 NewX = X + Dir[0];
		int32 NewY = Y + Dir[1];
        
		if (IsValidTile(NewX, NewY) && Grid[NewX][NewY]->IsBomb())
		{
			Count++;
		}
	}

	return Count;
}

bool SMinesweeperGame::IsValidTile(int32 X, int32 Y) const
{
	//return X >= 0 && X < Width && Y >= 0 && Y < Height;
	return X >= 0 && Y >= 0 && X < Width && Y < Height && Grid.IsValidIndex(X) && Grid[X].IsValidIndex(Y);
}

#undef LOCTEXT_NAMESPACE
