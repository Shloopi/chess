#ifndef SLD_HPP
#define SLD_HPP
#define SDL_MAIN_HANDLED
#include <memory>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <string>

namespace GUI {
	struct App {
		SDL_Renderer* renderer;
		SDL_Window* window;

		App() : renderer(nullptr), window(nullptr) {}
	};

	bool initApp(App& app, const char* title, int width, int height);
    SDL_Texture* loadTexture(App& app, const std::string& path);
}




#endif 