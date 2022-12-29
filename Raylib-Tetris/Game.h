#pragma once
#include <string>
#include "Board.h"
#include "Tetrominoes.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "MainMenu.h"

class Game
{
public:
	Game(int width, int height, int fps, std::string title);
	Game(const Game& other)= delete;
	Game& operator=(const Game& other) = delete;
	~Game() noexcept;

	bool GameShouldClose() const;
	void Tick();

private:
	Board board;
	Tetromino tetromino;
	Tetromino futureMino;
	Tetromino drawMino;
	Tetromino heldMino;
	InputManager inputManager;
	SoundManager soundManager;
	MainMenu mainMenu;

	Texture2D darkOverlay;

	void Draw();
	void Update();
	void UpdateMusic();
	int SelectRandomPiece() const;
	void DrawFuturePieces();
	void DrawDrawMino();
	void GameOver();
	void PauseMenu();
	void MainMenuButton(bool isNewBest);
	void DrawMainMenuButton(bool isNewBest);
	void RestartButton(bool isNewBest);
	void DrawRestartButton(bool isNewBest);

	int counterFall;
	int counterMove;
	int counterKeepMoving;
	int counterDrop;
	int lastAction;
	std::vector<int> tetrominoesList;
	int holdPiece;
	bool moved;
	bool gameShouldEnd;
	bool newBest;
	bool gamePaused;
	bool mouseOverRestartButton;
	bool mouseOverMainMenuButton;
	bool overlayLoaded;
	int restartButtonCounter;
	int mainMenuButtonCounter;
	std::string boardName;

};