#include "sdl2.hpp"
#include <iostream>
namespace GUI {
    bool initApp(App& app, const char* title, int width, int height) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        app.window = SDL_CreateWindow(title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            SDL_WINDOW_SHOWN);

        if (app.window == nullptr) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }

        app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED);
        if (app.renderer == nullptr) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(app.window);
            SDL_Quit();
            return false;
        }

        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);

        return true;
    }

    SDL_Texture* loadTexture(App& app, const std::string& path) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            std::cerr << "IMG_Load failed for " << path << ": " << IMG_GetError() << std::endl;
            return nullptr;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(app.renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    }
}



