#pragma once

#include "piece.hpp"
#include <array>
#include <cstdint>

class Board {
public:
    Board();

    Piece operator[](std::size_t rank, std::size_t file) const;

private:
    std::array<std::array<Piece, 8>, 8> state_;
};
