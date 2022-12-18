#include "Game.h"
#include "raylibCpp.h"
#include "Settings.h"
#include "Board.h"
#include <assert.h>
#include <iostream>
#include "InputManager.h"

Game::Game(int width, int height, int fps, std::string title)
	:
	board(settings::boardPosition, settings::cellSize, settings::padding),
	tetromino(Tetromino(board)),
	counter(0),
	counterDos(0),
	counterTres(0),
	inputManager(InputManager()),
	moved(false)
{
	assert(!IsWindowReady()); // if triggered game is already open.

	std::cout << "Loading Board";
	inputManager.LoadBoard("Default", board);
	std::cout << "Board Loaded";
	SelectRandomPiece(tetromino);
	std::cout << "Tetromino Loaded";
	//SetConfigFlags(FLAG_FULLSCREEN_MODE);
	InitWindow(width, height, title.c_str());
	SetTargetFPS(fps);
}

Game::~Game() noexcept 
{
	assert(GetWindowHandle()); // if triggered game window isn't open 
	CloseWindow();
}

bool Game::GameShouldClose() const
{
	return WindowShouldClose();
}

void Game::Tick()
{
	BeginDrawing();
	Update();
	Draw();
	EndDrawing();
}

void Game::Draw()
{
	DrawRectangleGradientV(0, 0, settings::screenWidth, settings::screenHeight, Color{ 33,1,0,255 }, Color{ 14,42,1,255 });
	board.Draw();
	tetromino.Draw();
	DrawFPS(0, 0);
}

void Game::Update()
{
	if (tetromino.GetFallen())
	{
		SelectRandomPiece(tetromino);
		std::cout << "\nrandom piece selected ";
		tetromino.SetPos({ board.GetWidth() / 2 - tetromino.GetDimension() / 2, 0 });
		tetromino.SetFallen(false);
	}

	if (IsKeyDown(KEY_LEFT))
	{
		counterDos++;
		if (tetromino.IsBottom())
		{
			counter = 0;
		}
		if (counterDos > 10)
		{
			counterTres++;
			if (counterTres > 1)
			{
				tetromino.MoveLeft();
				counterTres = 0;

				if (tetromino.IsBottom())
				{
					counter = 0;
				}
			}
		}
	}
	if (IsKeyReleased(KEY_LEFT))
	{
		counterDos = 0, counterTres = 0;
	}
	if (IsKeyPressed(KEY_LEFT))
	{
		tetromino.MoveLeft();

		if (tetromino.IsBottom())
		{
			counter = 0;
		}
	}

	if (IsKeyDown(KEY_RIGHT))
	{
		counterDos++;
		
		if (tetromino.IsBottom())
		{
			counter = 0;
		}

		if (counterDos > 10)
		{
			counterTres++;
			if (counterTres > 1)
			{
				tetromino.MoveRight();
				counterTres = 0;

				if (tetromino.IsBottom())
				{
					counter = 0;
				}
			}
		}
	}
	if (IsKeyReleased(KEY_RIGHT))
	{
		counterDos = 0, counterTres = 0;
	}
	if (IsKeyPressed(KEY_RIGHT))
	{
		tetromino.MoveRight();

		if (tetromino.IsBottom())
		{
			counter = 0;
		}
	}

	if (IsKeyPressed(KEY_X))
	{
		if (tetromino.IsBottom())
		{
			counter = 0;
		}
		tetromino.RotateClockwise();
		if (tetromino.IsBottom())
		{
			counter = 0;
		}
	}
	if (IsKeyPressed(KEY_Z))
	{
		if (tetromino.IsBottom())
		{
			counter = 0;
		}
		tetromino.RotateCounterClockwise();
		if (tetromino.IsBottom())
		{
			counter = 0;
		}
	}
	if (IsKeyPressed(KEY_C))
	{
		if (tetromino.IsBottom())
		{
			counter = 0;
		}
		tetromino.RotateFull();
		if (tetromino.IsBottom())
		{
			counter = 0;
		}
	}
	if (IsKeyDown(KEY_DOWN))
	{
		if (!tetromino.IsBottom())
		{
			counter += 15 + board.GetSpeed();
		}
		counter += 5 + board.GetSpeed();
	}

	board.ClearLines();
	counter += board.GetSpeed() + 1;
	tetromino.DebugNum();
	
	if (counter >= 60)
	{
		tetromino.Fall();
		counter = 0;
	}
}

void Game::SelectRandomPiece(Tetromino &tetromino)
{
	srand(time(NULL));
	inputManager.LoadTetromino(rand() % 7, tetromino);
}

