#include "board.hpp"
#include "fmt/color.h"
#include "fmt/format.h"

Board::Board()
    : state_{{{Piece::BR, Piece::BN, Piece::BB, Piece::BQ, Piece::BK, Piece::BB, Piece::BN, Piece::BR},
              {Piece::BP, Piece::BP, Piece::BP, Piece::BP, Piece::BP, Piece::BP, Piece::BP, Piece::BP},
              {Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO},
              {Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO},
              {Piece::NO, Piece::NO, Piece::NO, Piece::WP, Piece::NO, Piece::NO, Piece::NO, Piece::NO},
              {Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO, Piece::NO},
              {Piece::WP, Piece::WP, Piece::WP, Piece::WP, Piece::WP, Piece::WP, Piece::WP, Piece::WP},
              {Piece::WR, Piece::WN, Piece::WB, Piece::WK, Piece::WQ, Piece::WB, Piece::WN, Piece::WR}}} {}

Piece Board::operator[](std::size_t rank, std::size_t file) const { return state_[7 - rank][file]; }
