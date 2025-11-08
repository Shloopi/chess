#include "gui.hpp"

namespace GUI {
    void init() {

    }
    void drawChessBoard(SDL_Renderer* renderer) {
        SDL_Color light = { 240, 217, 181, 255 }; // light beige
        SDL_Color dark = { 181, 136, 99, 255 };  // brown

        for (int row = 0; row < Chess::BOARD_SIZE; ++row) {
            for (int col = 0; col < Chess::BOARD_SIZE; ++col) {
                bool isLight = (row + col) % 2 == 0;
                SDL_Color color = isLight ? light : dark;

                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

                SDL_Rect square = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                SDL_RenderFillRect(renderer, &square);
            }
        }
    }
    void GUI::mainLoop(App& app) {
        bool quit = false;
        SDL_Event e;
        while (!quit) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
            }

            SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
            SDL_RenderClear(app.renderer);

            // TODO: draw chess board here
			drawChessBoard(app.renderer);

            SDL_RenderPresent(app.renderer);
            SDL_Delay(16);
        }

        SDL_DestroyRenderer(app.renderer);
        SDL_DestroyWindow(app.window);
        SDL_Quit();
    }

}