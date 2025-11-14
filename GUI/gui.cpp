#include "gui.hpp"
#include "../Core/MoveGen.hpp"
#include "../Bot/Bot.hpp"

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
GuiApp::GuiApp() {
	GUI::initApp(this->app, "Chess Engine", GUI::WIDTH, GUI::HEIGHT);
	this->clickedPiece;
	this->init();
}
void GuiApp::mainLoop(Game& game) {
    Bot bot;
    GUI::GUIBoard board;
	std::array<Move, 218> moves;
    bool quit = false;
	bool changed = true;
    SDL_Event e;
    uint8_t moveCount = 0;
    while (!quit) {
        if (game.isBotTurn()) {
			Move move = bot.getBestMove(game, moves.data(), moveCount);
			game.makeMove(move);
            changed = true;
        }
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
				changed = true;
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
				this->handlePress(game, moveCount, moves.data(), board, mouseX, mouseY);
                
                if (game.gameState != GameState::ONGOING) {
                    std::cout << "Game Over! State: " << static_cast<int>(game.gameState) << '\n';
                    quit = true;
                }
            }
        }

        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
        SDL_RenderClear(app.renderer);

        if (changed) {
            moveCount = MoveGen::genAllLegalMoves(game, moves.data());

            changed = false;
            drawChessBoard();
            GUI::convertBoard(game.board, board);
            drawPieces(board);
            drawSquareHighlight();
			this->showMoves(moveCount, moves.data(), board);
            SDL_RenderPresent(app.renderer);

        }
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
}
void GuiApp::drawPieces(const GUI::GUIBoard& board) {
    char piece;

    for (int y = 0; y < Chess::FILE_SIZE; y++) {
        for (int x = 0; x < Chess::RANK_SIZE; x++) {
            piece = board[y][x];

            if (piece != '.') {
                if (this->pieces.contains(piece)) {
                    SDL_Rect targetSquare = { x * GUI::TILE_SIZE, y * GUI::TILE_SIZE, GUI::TILE_SIZE, GUI::TILE_SIZE };
                    SDL_RenderCopy(app.renderer, this->pieces.at(piece), nullptr, &targetSquare);
                }
            }
        }
    }
}
void GuiApp::drawSquareHighlight() {
    if (this->clickedPiece.isClicked()) {
        SDL_SetRenderDrawColor(app.renderer, 255, 0, 0, 100);
        SDL_RenderFillRect(app.renderer, this->clickedPiece.clickedSquare.get());
		SDL_RenderPresent(app.renderer);
        SDL_RenderCopy(app.renderer, this->pieces.at(this->clickedPiece.piece), nullptr, this->clickedPiece.clickedSquare.get());
    }
}
void GuiApp::handlePress(Game& game, uint8_t moveCount, Move* moves, const GUI::GUIBoard& board, int mouseX, int mouseY) {
    Index file = mouseX / GUI::TILE_SIZE;
    Index rank = mouseY / GUI::TILE_SIZE;
    char piece = board[rank][file];
    
    if (std::isalpha(static_cast<unsigned char>(piece)) && std::isupper(piece) == game.whiteToMove) {
        SDL_Rect rect = {
            file * GUI::TILE_SIZE,
            rank * GUI::TILE_SIZE,
            GUI::TILE_SIZE,
            GUI::TILE_SIZE
        };
        this->clickedPiece.clickedSquare = std::make_unique<SDL_Rect>(rect);
        this->clickedPiece.piece = piece;
    }
    else {
        if (this->clickedPiece.isClicked()) {
            Index startFile = this->clickedPiece.clickedSquare->x / GUI::TILE_SIZE;
            Index startRank = this->clickedPiece.clickedSquare->y / GUI::TILE_SIZE;
            Index startSquare = (Chess::RANK_SIZE - 1 - startRank) * Chess::RANK_SIZE + startFile;
            Index targetSquare = (Chess::RANK_SIZE - 1 - rank) * Chess::RANK_SIZE + file;

            for (uint8_t i = 0; i < moveCount; i++) {
                if (moves[i].from == startSquare && moves[i].to == targetSquare) {
					game.makeMove(moves[i]);
                    break;
                }
            }
        }
        this->clickedPiece.clickedSquare = nullptr;
        this->clickedPiece.piece = '.';
    }
}

void GuiApp::showMoves(uint8_t moveCount, Move* moves, const GUI::GUIBoard& board) {
    if (this->clickedPiece.isClicked()) {
        Index square = (Chess::RANK_SIZE - 1 - (this->clickedPiece.clickedSquare->y / GUI::TILE_SIZE)) * Chess::RANK_SIZE + (this->clickedPiece.clickedSquare->x / GUI::TILE_SIZE);

        for (uint8_t i = 0; i < moveCount; i++) {
            if (moves[i].from == square) {
                Index to = moves[i].to;
                Index toFile = to % Chess::RANK_SIZE;
                Index toRank = Chess::RANK_SIZE - 1 - (to / Chess::RANK_SIZE);

                SDL_Rect highlightRect = {
                    toFile * GUI::TILE_SIZE,
                    toRank * GUI::TILE_SIZE,
                    GUI::TILE_SIZE,
                    GUI::TILE_SIZE
                };

                SDL_SetRenderDrawColor(app.renderer, 0, 255, 0, 100);
                SDL_RenderFillRect(app.renderer, &highlightRect);
                SDL_RenderPresent(app.renderer);

                char piece = board[toRank][toFile];
                if (piece != '.') {
                    SDL_RenderCopy(app.renderer, this->pieces.at(piece), nullptr, &highlightRect);
                }
            }
        }
	}
}
