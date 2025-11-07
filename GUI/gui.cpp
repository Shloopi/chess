#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <array>
#include <unordered_map>
#include <string>

constexpr int TILE_SIZE = 80;
constexpr int BOARD_SIZE = 8;
constexpr int WIDTH = TILE_SIZE * BOARD_SIZE;
constexpr int HEIGHT = TILE_SIZE * BOARD_SIZE;

using Board = std::array<char, 64>;

int idx(int r, int c) { return r * 8 + c; }

void initBoard(Board& b) {
    std::string start =
        "rnbqkbnr"
        "pppppppp"
        "........"
        "........"
        "........"
        "........"
        "PPPPPPPP"
        "RNBQKBNR";
    for (int i = 0; i < 64; ++i) b[i] = start[i];
}

std::string pieceFile(char p) {
    switch (p) {
    case 'K': return "pieces/w_king.png";
    case 'Q': return "pieces/w_queen.png";
    case 'R': return "pieces/w_rook.png";
    case 'B': return "pieces/w_bishop.png";
    case 'N': return "pieces/w_knight.png";
    case 'P': return "pieces/w_pawn.png";
    case 'k': return "pieces/b_king.png";
    case 'q': return "pieces/b_queen.png";
    case 'r': return "pieces/b_rook.png";
    case 'b': return "pieces/b_bishop.png";
    case 'n': return "pieces/b_knight.png";
    case 'p': return "pieces/b_pawn.png";
    default:  return "";
    }
}

int main(int, char**) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL init error: " << SDL_GetError() << "\n";
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image init error: " << IMG_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "SDL2 Chess GUI",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Board board;
    initBoard(board);

    // --- Load piece textures ---
    std::unordered_map<char, SDL_Texture*> pieceTextures;
    for (char c : {'K', 'Q', 'R', 'B', 'N', 'P', 'k', 'q', 'r', 'b', 'n', 'p'}) {
        std::string file = pieceFile(c);
        SDL_Surface* surf = IMG_Load(file.c_str());
        if (!surf) {
            std::cerr << "Failed to load " << file << ": " << IMG_GetError() << "\n";
            return 1;
        }
        pieceTextures[c] = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }

    bool running = true;
    SDL_Event e;
    bool dragging = false;
    int selFrom = -1;
    int mouseX = 0, mouseY = 0;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int c = e.button.x / TILE_SIZE;
                int r = e.button.y / TILE_SIZE;
                int i = idx(r, c);
                if (board[i] != '.') {
                    selFrom = i;
                    dragging = true;
                }
                else if (selFrom != -1) {
                    board[i] = board[selFrom];
                    board[selFrom] = '.';
                    selFrom = -1;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
                if (dragging) {
                    int c = e.button.x / TILE_SIZE;
                    int r = e.button.y / TILE_SIZE;
                    int to = idx(r, c);
                    board[to] = board[selFrom];
                    board[selFrom] = '.';
                    selFrom = -1;
                    dragging = false;
                }
            }
            else if (e.type == SDL_MOUSEMOTION) {
                mouseX = e.motion.x;
                mouseY = e.motion.y;
            }
        }

        // --- Render ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw board
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                bool light = ((r + c) % 2 == 0);
                if (light)
                    SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);
                SDL_Rect rect = { c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        // Draw pieces
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                int i = idx(r, c);
                char p = board[i];
                if (p == '.') continue;
                if (i == selFrom && dragging) continue; // skip dragging piece

                SDL_Texture* tex = pieceTextures[p];
                SDL_Rect dest = { c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                SDL_RenderCopy(renderer, tex, nullptr, &dest);
            }
        }

        // Draw dragging piece
        if (dragging && selFrom != -1) {
            char p = board[selFrom];
            SDL_Texture* tex = pieceTextures[p];
            SDL_Rect dest = { mouseX - TILE_SIZE / 2, mouseY - TILE_SIZE / 2, TILE_SIZE, TILE_SIZE };
            SDL_RenderCopy(renderer, tex, nullptr, &dest);
        }

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    for (auto& kv : pieceTextures) SDL_DestroyTexture(kv.second);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
