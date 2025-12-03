#include "engine.hpp"
#include "fmt/format.h"
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include <utility>

constexpr auto BOARD_IMAGE_SIZE = std::array{142, 142};
constexpr auto BOARD_IMAGE_OFFSET = std::array{7, 20};
constexpr auto SCALE = 3;

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Texture *board_image;
SDL_Texture *white_pieces_image;
SDL_Texture *black_pieces_image;
SDL_Texture *selector_image;

void init_sdl();
void quit_sdl();
void load_assets();
void run();

SDL_Texture *piece_texture(Piece piece);
SDL_FRect piece_src_rect(Piece piece);
void draw_piece(Piece piece, int rank, int file, bool hovered);

int main(int, char *[]) {
    try {
        init_sdl();
        load_assets();
        run();
    } catch (const std::exception &e) {
        fmt::print("{}\n", e.what());
    }

    quit_sdl();
}

void init_sdl() {
    if (!SDL_Init(SDL_INIT_VIDEO))
        throw std::runtime_error(fmt::format("Failed to initialize SDL: {}", SDL_GetError()));

    window = SDL_CreateWindow("Holy Hell", BOARD_IMAGE_SIZE[0] * SCALE, BOARD_IMAGE_SIZE[1] * SCALE, 0);
    if (!window) throw std::runtime_error(fmt::format("Failed to create window: {}", SDL_GetError()));

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) throw std::runtime_error(fmt::format("Failed to create renderer: {}", SDL_GetError()));
}

void quit_sdl() { SDL_Quit(); }

void load_assets() {
    board_image = IMG_LoadTexture(renderer, "assets/pixel_chess/boards/board_persp_01.png");
    if (!board_image) throw std::runtime_error(fmt::format("Failed to load board image: {}", SDL_GetError()));
    SDL_SetTextureScaleMode(board_image, SDL_SCALEMODE_NEAREST);

    white_pieces_image = IMG_LoadTexture(renderer, "assets/pixel_chess/16x32_pieces/WhitePieces-Sheet.png");
    if (!white_pieces_image)
        throw std::runtime_error(fmt::format("Failed to load white pieces image: {}", SDL_GetError()));
    SDL_SetTextureScaleMode(white_pieces_image, SDL_SCALEMODE_NEAREST);

    black_pieces_image = IMG_LoadTexture(renderer, "assets/pixel_chess/16x32_pieces/BlackPieces-Sheet.png");
    if (!black_pieces_image)
        throw std::runtime_error(fmt::format("Failed to load black pieces image: {}", SDL_GetError()));
    SDL_SetTextureScaleMode(black_pieces_image, SDL_SCALEMODE_NEAREST);

    selector_image = IMG_LoadTexture(renderer, "assets/selector.png");
    if (!selector_image) throw std::runtime_error(fmt::format("Failed to load selector image: {}", SDL_GetError()));
    SDL_SetTextureScaleMode(selector_image, SDL_SCALEMODE_NEAREST);
}

void run() {
    Engine engine;

    bool running = true;

    SDL_Event event;
    do {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT: running = false; break;
            default: break;
            }

            SDL_SetRenderDrawColor(renderer, 0x96, 0xa2, 0xb3, 0xff);
            SDL_RenderClear(renderer);

            SDL_RenderTexture(renderer, board_image, nullptr, nullptr);

            float mouse_x, mouse_y;
            SDL_GetMouseState(&mouse_x, &mouse_y);

            for (int rank = 7; rank >= 0; --rank) {
                for (int file = 0; file < 8; ++file) {
                    auto piece = engine.board[rank, file];
                    if (piece == Piece::NO) continue;

                    bool hovered = (mouse_x >= BOARD_IMAGE_OFFSET[0] * SCALE + file * 16 * SCALE) &&
                                   (mouse_x < BOARD_IMAGE_OFFSET[0] * SCALE + (file + 1) * 16 * SCALE) &&
                                   (mouse_y >= BOARD_IMAGE_OFFSET[1] * SCALE + ((7 - rank) * 12) * SCALE) &&
                                   (mouse_y < BOARD_IMAGE_OFFSET[1] * SCALE + ((7 - rank + 1) * 12) * SCALE);

                    draw_piece(piece, rank, file, hovered);
                }

                // break;
            }

            SDL_RenderPresent(renderer);
        }
    } while (running);
}

SDL_Texture *piece_texture(Piece piece) {
    switch (piece) {
    case Piece::BK:
    case Piece::BQ:
    case Piece::BR:
    case Piece::BB:
    case Piece::BN:
    case Piece::BP: return black_pieces_image;
    case Piece::WK:
    case Piece::WQ:
    case Piece::WR:
    case Piece::WB:
    case Piece::WN:
    case Piece::WP: return white_pieces_image;
    case Piece::NO: return nullptr;
    }
    std::unreachable();
}

SDL_FRect piece_src_rect(const Piece piece) {
    switch (piece) {
    case Piece::BK:
    case Piece::WK: return SDL_FRect(80, 0, 16, 32);
    case Piece::BQ:
    case Piece::WQ: return SDL_FRect(64, 0, 16, 32);
    case Piece::BR:
    case Piece::WR: return SDL_FRect(32, 0, 16, 32);
    case Piece::BB:
    case Piece::WB: return SDL_FRect(48, 0, 16, 32);
    case Piece::BN:
    case Piece::WN: return SDL_FRect(16, 0, 16, 32);
    case Piece::BP:
    case Piece::WP: return SDL_FRect(0, 0, 16, 32);
    case Piece::NO: return SDL_FRect(0, 0, 0, 0);
    }
    std::unreachable();
}

void draw_piece(Piece piece, int rank, int file, bool hovered) {
    auto texture = piece_texture(piece);
    auto src_rect = piece_src_rect(piece);
    auto dst_rect = SDL_FRect((BOARD_IMAGE_OFFSET[0] + file * 16) * SCALE,
                              (BOARD_IMAGE_OFFSET[1] + ((8 - rank) * 12) - 32 - 3) * SCALE,
                              16 * SCALE,
                              32 * SCALE);

    if (hovered) {
        // Draw top half of selector
        auto selector_top_src_rect = SDL_FRect(0, 0, 16, 8);
        auto selector_top_dst_rect = SDL_FRect(dst_rect.x, dst_rect.y + (16 + 5) * SCALE, 16 * SCALE, 8 * SCALE);
        SDL_RenderTexture(renderer, selector_image, &selector_top_src_rect, &selector_top_dst_rect);
    }

    // Draw the piece
    SDL_RenderTexture(renderer, texture, &src_rect, &dst_rect);

    if (hovered) {
        // Draw bottom half of selector
        auto selector_bottom_src_rect = SDL_FRect(0, 8, 16, 8);
        auto selector_bottom_dst_rect = SDL_FRect(dst_rect.x, dst_rect.y + (16 + 5 + 8) * SCALE, 16 * SCALE, 8 * SCALE);
        SDL_RenderTexture(renderer, selector_image, &selector_bottom_src_rect, &selector_bottom_dst_rect);
    }
}
