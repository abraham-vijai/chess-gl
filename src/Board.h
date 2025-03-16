#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include "ShapeManager.h"

class Board
{
public:
    int Square[64]; // Non-static array

    Board()
    {
        // Initialize the board
        Square[0] = Piece::Black | Piece::Rook;
        Square[1] = Piece::Black | Piece::Knight;
        Square[2] = Piece::Black | Piece::Bishop;
        Square[3] = Piece::Black | Piece::Queen;
        Square[4] = Piece::Black | Piece::King;
        Square[5] = Piece::Black | Piece::Bishop;
        Square[6] = Piece::Black | Piece::Knight;
        Square[7] = Piece::Black | Piece::Rook;
        // Black pawns
        for (int i = 8; i < 16; i++)
            Square[i] = Piece::Black | Piece::Pawn;
        // Empty spaces
        for (int i = 16; i < 56; i++)
            Square[i] = Piece::None;
        // White pieces
        Square[56] = Piece::White | Piece::Rook;
        Square[57] = Piece::White | Piece::Knight;
        Square[58] = Piece::White | Piece::Bishop;
        Square[59] = Piece::White | Piece::Queen;
        Square[60] = Piece::White | Piece::King;
        Square[61] = Piece::White | Piece::Bishop;
        Square[62] = Piece::White | Piece::Knight;
        Square[63] = Piece::White | Piece::Rook;
        // White pawns
        for (int i = 48; i < 56; i++)
            Square[i] = Piece::White | Piece::Pawn;
    }

    ~Board() {}
};

#endif