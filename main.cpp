#include "engine.hpp"
#include "fmt/format.h"
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"

constexpr auto BOARD_IMAGE_SIZE = std::array{142, 142};
constexpr auto SCALE = 3;

SDL_Window *window;
SDL_Renderer *renderer;

void init_sdl();
void quit_sdl();
void run();

int main(int, char *[]) {
    try {
        init_sdl();
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

void run() {
    bool running = true;

    SDL_Event event;
    do {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT: running = false; break;
            default: break;
            }

            SDL_RenderPresent(renderer);
        }
    } while (running);
}
