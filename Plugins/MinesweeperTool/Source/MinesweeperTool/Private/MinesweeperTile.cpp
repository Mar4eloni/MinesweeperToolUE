#include "MinesweeperTile.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "MinesweeperGame.h"

#define LOCTEXT_NAMESPACE "Minesweeper"

void SMinesweeperTile::Construct(const FArguments& InArgs)
{
    Game = InArgs._Game;
    X = InArgs._X;
    Y = InArgs._Y;
    bIsBomb = InArgs._IsBomb;

    State = bIsBomb.Get() ? ETileState::Bomb : ETileState::Hidden;
    
    ChildSlot
    [
        SNew(SButton)
        .OnClicked(this, &SMinesweeperTile::OnTileClicked)
        [
            SAssignNew(TileText, STextBlock)
            .Text(FText::GetEmpty())
            .Justification(ETextJustify::Center)
        ]
    ];
    HiddenButtonStyle = MakeShared<FButtonStyle>(FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Button"));
    HiddenButtonStyle->Normal.TintColor = FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f));
    HiddenButtonStyle->Hovered.TintColor = FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f));
    HiddenButtonStyle->Pressed.TintColor = FSlateColor(FLinearColor(0.6f, 0.6f, 0.6f));

    RevealedButtonStyle = MakeShared<FButtonStyle>(FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Button"));
    RevealedButtonStyle->Normal.TintColor = FSlateColor(FLinearColor(0.9f, 0.9f, 0.9f, 0.5f));
    RevealedButtonStyle->Hovered = RevealedButtonStyle->Normal;
    RevealedButtonStyle->Pressed = RevealedButtonStyle->Normal;

    BombButtonStyle = MakeShared<FButtonStyle>(*RevealedButtonStyle);
    BombButtonStyle->Normal.TintColor = FSlateColor(FLinearColor(1.0f, 0.3f, 0.3f, 0.7f));

    // Create the button with hidden style
    ChildSlot
    [
        SAssignNew(TileButton, SButton)
        .ButtonStyle(HiddenButtonStyle.Get())
        .OnClicked(this, &SMinesweeperTile::OnTileClicked)
        //.OnRightClick(this, &SMinesweeperTile::OnTileRightClicked)
        [
            SAssignNew(TileText, STextBlock)
            .Text(FText::GetEmpty())
            .Justification(ETextJustify::Center)
        ]
    ];
}

FReply SMinesweeperTile::OnTileClicked()
{
    if (auto GamePtr = Game.Pin())
    {
        GamePtr->RevealTile(X.Get(), Y.Get());
    }
    return FReply::Handled();
}

FReply SMinesweeperTile::OnTileRightClicked()
{
    if (State == ETileState::Hidden)
    {
        State = ETileState::Flagged;
        TileText->SetText(LOCTEXT("FlagSymbol", "F"));
        TileButton->SetButtonStyle(HiddenButtonStyle.Get()); // Or create a special flag style
    }
    else if (State == ETileState::Flagged)
    {
        State = ETileState::Hidden;
        TileText->SetText(FText::GetEmpty());
    }
    return FReply::Handled();
}

void SMinesweeperTile::Reveal()
{
    UE_LOG(LogTemp, Warning, TEXT("Revealing tile at %d,%d - Bomb:%d"), X.Get(), Y.Get(), bIsBomb.Get());
    
    if (State == ETileState::Revealed || State == ETileState::Flagged)
    {
        return;
    }

    State = ETileState::Revealed;

    if (bIsBomb.Get())
    {
        TileText->SetText(LOCTEXT("BombSymbol", "X"));
        TileText->SetColorAndOpacity(FLinearColor::Red);
        TileButton->SetButtonStyle(BombButtonStyle.Get());
    }
    else
    {

        TileButton->SetButtonStyle(RevealedButtonStyle.Get());
        
        int32 BombCount = Game.IsValid() ? Game.Pin()->CountAdjacentBombs(X.Get(), Y.Get()) : 0;
        if (BombCount > 0)
        {
            static const TArray<FLinearColor> NumberColors = {
                FLinearColor::Blue,
                FLinearColor::Green,
                FLinearColor::Red,
                FLinearColor::MakeRandomColor(),
                FLinearColor::MakeRandomColor(),
                FLinearColor::MakeRandomColor(),
                FLinearColor::Black,
                FLinearColor::Gray
            };
            
            TileText->SetText(FText::AsNumber(BombCount));
            TileText->SetColorAndOpacity(NumberColors[FMath::Clamp(BombCount - 1, 0, NumberColors.Num() - 1)]);
        }
    }
}

void SMinesweeperTile::RevealAsWin()
{
    TileText->SetText(LOCTEXT("BombSymbol", "X"));
    TileText->SetColorAndOpacity(FLinearColor::Green);
}

void SMinesweeperTile::ShowIncorrectFlag()
{
    TileText->SetText(LOCTEXT("IncorrectFlag", "✗"));
    TileText->SetColorAndOpacity(FLinearColor::Red);
}

void SMinesweeperTile::SetHighlight(bool bHighlight)
{
    if (TileButton.IsValid() && State == ETileState::Hidden)
    {
        auto Style = MakeShared<FButtonStyle>(*HiddenButtonStyle);
        Style->Normal.TintColor = bHighlight ? 
            FSlateColor(FLinearColor(0.85f, 0.85f, 0.85f)) : 
            HiddenButtonStyle->Normal.TintColor;
        TileButton->SetButtonStyle(RevealedButtonStyle.Get());
    }
}

#undef LOCTEXT_NAMESPACE
