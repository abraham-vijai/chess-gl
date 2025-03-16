#ifndef PIECE_H
#define PIECE_H

namespace Piece
{        
    // 3 bits on the right will indicate the type of piece
    constexpr unsigned int None   = 0;
    constexpr unsigned int King   = 1;
    constexpr unsigned int Queen  = 2;
    constexpr unsigned int Bishop = 3;
    constexpr unsigned int Rook   = 4;
    constexpr unsigned int Pawn   = 5;
    constexpr unsigned int Knight = 6;
    // 2 bits on the left will tell us the color
    constexpr unsigned int White  = 8;
    constexpr unsigned int Black  = 16;
}

#endif 

