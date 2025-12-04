#include "engine.hpp"
#include "fmt/format.h"
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"
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

TTF_Font *font;

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

    if (!TTF_Init()) throw std::runtime_error(fmt::format("Failed to initialize SDL_ttf: {}", SDL_GetError()));

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

    font = TTF_OpenFont("assets/Silver.ttf", 19 * 2);
    if (!font) throw std::runtime_error(fmt::format("Failed to load font: {}", SDL_GetError()));
}

enum class State { Idle, Grabbed, Released };

void run() {
    Engine engine;

    auto state = State::Idle;
    auto active_piece = std::array{-1, -1};

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

            bool hovered = false;
            for (int rank = 7; rank >= 0; --rank) {
                for (int file = 0; file < 8; ++file) {
                    auto piece = engine.board[rank, file];
                    if (piece == Piece::NO) continue;

                    if (!hovered) {
                        hovered = (mouse_x >= BOARD_IMAGE_OFFSET[0] * SCALE + file * 16 * SCALE) &&
                                  (mouse_x < BOARD_IMAGE_OFFSET[0] * SCALE + (file + 1) * 16 * SCALE) &&
                                  (mouse_y >= BOARD_IMAGE_OFFSET[1] * SCALE + ((7 - rank) * 12) * SCALE) &&
                                  (mouse_y < BOARD_IMAGE_OFFSET[1] * SCALE + ((7 - rank + 1) * 12) * SCALE);
                        if (hovered) {
                            active_piece = {rank, file};
                        }
                    }

                    draw_piece(piece, rank, file, active_piece[0] == rank && active_piece[1] == file);
                }
            }
            if (!hovered) active_piece = {-1, -1};

            if (hovered) {
                SDL_Color white = {255, 255, 255, 255};
                SDL_Color transparent = {0x96, 0xa2, 0xb3, 0xff};
                SDL_Surface *text_surf = TTF_RenderText_Solid(
                    font, fmt::format("({}, {})", active_piece[0], active_piece[1]).c_str(), 0, white);
                if (!text_surf) throw std::runtime_error(fmt::format("Failed to render text: {}", SDL_GetError()));

                SDL_Texture *text_image = SDL_CreateTextureFromSurface(renderer, text_surf);
                if (!text_image)
                    throw std::runtime_error(fmt::format("Failed to create text image: {}", SDL_GetError()));
                SDL_DestroySurface(text_surf);

                float text_image_w, text_image_h;
                SDL_GetTextureSize(text_image, &text_image_w, &text_image_h);

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
                auto dst_rect = SDL_FRect(4, (BOARD_IMAGE_SIZE[1] * SCALE) - text_image_h, text_image_w, text_image_h);
                SDL_RenderTexture(renderer, text_image, nullptr, &dst_rect);

                SDL_DestroyTexture(text_image);
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
