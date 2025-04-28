#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

// Forward declarations
class SMinesweeperTile;
enum class ETileState : uint8;

class MINESWEEPERTOOL_API SMinesweeperGame : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMinesweeperGame) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    // Game functions
    void InitializeGame(int32 InWidth, int32 InHeight, int32 InBombCount);
    void RevealTile(int32 X, int32 Y);
    void RevealAdjacentTiles(int32 X, int32 Y);
    void RevealAdjacentTilesImmediate(int32 X, int32 Y);
    void GameOver(bool bWon);
    void ResetGame();

    // Helper functions
    int32 CountAdjacentBombs(int32 X, int32 Y) const;
    bool IsValidTile(int32 X, int32 Y) const;

private:
    TArray<TArray<TSharedPtr<SMinesweeperTile>>> Grid;
    int32 Width;
    int32 Height;
    int32 BombCount;
    int32 RevealedTiles;
    bool bGameOver;

    TSharedPtr<class SEditableTextBox> WidthInput;
    TSharedPtr<class SEditableTextBox> HeightInput;
    TSharedPtr<class SEditableTextBox> BombCountInput;
    TSharedPtr<class SButton> StartButton;

    TSharedPtr<SScrollBox> ScrollBox;
    TSharedPtr<SBox> ContentBox;

    TSharedPtr<class STextBlock> GameStatusText;
};