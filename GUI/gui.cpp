#include "gui.hpp"
#include "../Core/MoveGen.hpp"

GuiApp::GuiApp() {
    GUI::initApp(this->app, "Chess Engine", GUI::WIDTH, GUI::HEIGHT);
    this->init();
}

void GuiApp::init() {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	this->pieces['P'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "white-pawn.png");
    this->pieces['R'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "white-rook.png");
    this->pieces['N'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "white-knight.png");
    this->pieces['B'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "white-bishop.png");
    this->pieces['Q'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "white-queen.png");
    this->pieces['K'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "white-king.png");
    this->pieces['p'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "black-pawn.png");
    this->pieces['r'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "black-rook.png");
    this->pieces['n'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "black-knight.png");
    this->pieces['b'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "black-bishop.png");
    this->pieces['q'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "black-queen.png");
    this->pieces['k'] = GUI::loadTexture(this->app, GUI::ASSETS_PATH + "black-king.png");
}
void GuiApp::drawChessBoard() {
    SDL_Color light = { 240, 217, 181, 255 }; // light beige
    SDL_Color dark = { 181, 136, 99, 255 };  // brown

    for (int row = 0; row < Chess::BOARD_SIZE; ++row) {
        for (int col = 0; col < Chess::BOARD_SIZE; ++col) {
            bool isLight = (row + col) % 2 == 0;
            SDL_Color color = isLight ? light : dark;

            SDL_SetRenderDrawColor(this->app.renderer, color.r, color.g, color.b, color.a);

            SDL_Rect square = { col * GUI::TILE_SIZE, row * GUI::TILE_SIZE, GUI::TILE_SIZE, GUI::TILE_SIZE };
            SDL_RenderFillRect(this->app.renderer, &square);
        }
    }
}

void GuiApp::mainLoop(Game& game) {
    this->conv = GUIConverter(game);

    bool quit = false;
	bool changed = true;
    GUI::Coord press;
    SDL_Event e;
    uint8_t moveCount = 0;
    while (!quit) {
        changed = changed || conv.handleBot();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
				changed = true;
                SDL_GetMouseState(&press.x, &press.y);
                conv.handlePress(press);
            }
        }

        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
        SDL_RenderClear(app.renderer);

        if (changed) {
            changed = false;
            drawChessBoard();
            drawPieces();
            drawSquareHighlight();
			this->showMoves();
            SDL_RenderPresent(app.renderer);

        }

        quit = conv.gameOver();
        SDL_Delay(16);
    }
    std::cout << "Game Over!";
    SDL_Delay(2000);

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
}
void GuiApp::drawPieces() {
    char piece;

    for (int rank = 0; rank < Chess::RANK_SIZE; rank++) {
        for (int file = 0; file < Chess::FILE_SIZE; file++) {
            piece = this->conv.getPiece(Chess::toIndex(rank, file));

            if (piece != '.') {
                if (this->pieces.contains(piece)) {
                    SDL_Rect targetSquare = { file * GUI::TILE_SIZE, rank * GUI::TILE_SIZE, GUI::TILE_SIZE, GUI::TILE_SIZE };
                    SDL_RenderCopy(app.renderer, this->pieces.at(piece), nullptr, &targetSquare);
                }
            }
        }
    }
}
void GuiApp::drawSquareHighlight() {
    Index squareIndex = this->conv.getPressedSquare();

    if (squareIndex != -1) {
        GUI::Coord coord = this->conv.getCoord(squareIndex);
        SDL_Rect targetSquare = { coord.x, coord.y, GUI::TILE_SIZE, GUI::TILE_SIZE };
        char piece = this->conv.getPiece(squareIndex);
        int rank = Chess::rankOf(squareIndex);
        int file = Chess::fileOf(squareIndex);

        if ((rank + file) % 2 == 0) {
            SDL_SetRenderDrawColor(app.renderer, 240, 160, 90, 255);
        }
        else {
            SDL_SetRenderDrawColor(app.renderer, 205, 110, 50, 255);
        }
        SDL_RenderFillRect(app.renderer, &targetSquare);
		SDL_RenderPresent(app.renderer);
        if (this->pieces.contains(piece))
            SDL_RenderCopy(app.renderer, this->pieces.at(piece), nullptr, &targetSquare);
    }
}

void GuiApp::showMoves() {
    for (int i = 0; i < this->conv.getPieceMoveCount(); i++) {
        Index targetSquare = this->conv.getPieceMove(i);
        GUI::Coord coord = this->conv.getCoord(targetSquare);
        SDL_Rect highlightRect = {
                    coord.x,
                    coord.y,
                    GUI::TILE_SIZE,
                    GUI::TILE_SIZE
        };

        if ((Chess::rankOf(targetSquare) + Chess::fileOf(targetSquare)) % 2 == 0) {
            SDL_SetRenderDrawColor(app.renderer, 247, 226, 122, 255);
        }
        else {
            SDL_SetRenderDrawColor(app.renderer, 217, 178, 76, 255);
        }
        SDL_RenderFillRect(app.renderer, &highlightRect);
        SDL_RenderPresent(app.renderer);

        char piece = this->conv.getPiece(targetSquare);
        if (this->pieces.contains(piece)) {
            SDL_RenderCopy(app.renderer, this->pieces.at(piece), nullptr, &highlightRect);
        }
    }
}
