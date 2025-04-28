#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "MinesweeperGame.h"

enum class ETileState : uint8
{
    Hidden,
    Revealed,
    Flagged,
    Bomb
};

class MINESWEEPERTOOL_API SMinesweeperTile : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMinesweeperTile) {}
        SLATE_ATTRIBUTE(int32, X)
        SLATE_ATTRIBUTE(int32, Y)
        SLATE_ATTRIBUTE(bool, IsBomb)
        SLATE_ARGUMENT(TSharedPtr<SMinesweeperGame>, Game) 
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    FReply OnTileClicked();
    FReply OnTileRightClicked();

    void Reveal();
    void RevealAsWin();
    void ShowIncorrectFlag();
    void SetHighlight(bool bHighlight);
    bool IsRevealed() const { return State == ETileState::Revealed; }
    bool IsBomb() const { return State == ETileState::Bomb; }
    bool IsFlagged() const { return State == ETileState::Flagged; }

private:
    TWeakPtr<SMinesweeperGame> Game;
    ETileState State = ETileState::Hidden;
    int32 AdjacentBombs = 0;

    TAttribute<int32> X;
    TAttribute<int32> Y;
    TAttribute<bool> bIsBomb;

    TSharedPtr<class STextBlock> TileText;

    TSharedPtr<FButtonStyle> HiddenButtonStyle;
    TSharedPtr<FButtonStyle> RevealedButtonStyle;
    TSharedPtr<FButtonStyle> BombButtonStyle;
    TSharedPtr<SButton> TileButton;
};