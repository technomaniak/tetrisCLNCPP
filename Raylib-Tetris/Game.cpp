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
	futureMino(Tetromino(board)),
	heldMino(Tetromino(board)),
	drawMino(Tetromino(board)),
	counterFall(1),
	counterMove(0),
	counterKeepMoving(0),
	counterDrop(1),
	holdPiece(-1),
	inputManager(InputManager()),
	moved(false),
	lastAction(0),
	gameShouldEnd(false),
	soundManager(SoundManager()),
	mainMenu(MainMenu(soundManager)),
	boardName("Default"),
	newBest(false)
{
	assert(!IsWindowReady()); // if triggered game is already open.
	std::cout << "\nLoading Board " << sizeof(int);
	inputManager.LoadBoard(boardName, board);
	std::cout << "\nBoard Loaded";
	tetrominoesList.resize(inputManager.GetTetrominoPreviewAmount());
	for (int i = 0; i < inputManager.GetTetrominoPreviewAmount(); i++)
	{
		tetrominoesList[i] = SelectRandomPiece();
	}
	std::cout << "\nTetromino Loaded";
	srand((int)time(NULL));

	//SetConfigFlags(FLAG_FULLSCREEN_MODE);
	InitWindow(width, height, title.c_str());
	SetTargetFPS(fps);
}

Game::~Game() noexcept 
{
	assert(GetWindowHandle()); // if triggered game window isn't open 
	soundManager.CloseSound();
	CloseAudioDevice();
	CloseWindow();
}

bool Game::GameShouldClose() const
{
	return WindowShouldClose();
}

void Game::Tick()
{
	BeginDrawing();
	if (mainMenu.GetGameRunning())
	{
		if (gameShouldEnd)
		{
			GameOver();
		}
		else
		{
			Update();
			Draw();
		}
	}
	else
	{
		mainMenu.LoadMenu();
		mainMenu.Tick();
	}
	EndDrawing();
}

void Game::Draw()
{	//Texture2D texture = LoadTextureFromImage(GenImageGradientV(settings::screenWidth, settings::screenHeight, Color{ 33,1,0,255 }, Color{ 14,42,1,255 }));
	//DrawTexture(texture, 0, 0, WHITE);
	DrawRectangleGradientV(0, 0, (settings::screenWidth / 5) * 3, settings::screenHeight, Color{ 33,1,0,255 }, Color{ 14,42,1,105 });
	DrawRectangleGradientV((settings::screenWidth / 5) * 3, 0, settings::screenWidth, settings::screenHeight, Color{ 33,1,0,255 }, Color{ 14,42,1,255 });
	board.Draw();
	tetromino.Draw();
	DrawDrawMino();
	if (inputManager.GetTetrominoPreviewAmount() != 0)
	{
		DrawFuturePieces();
	}
	DrawFPS(0, 0);
}

void Game::Update()
{
	if (!soundManager.CheckMusicPlaying())
	{
		soundManager.PlayRandomMusic();
	}
	else
	{
		soundManager.UpdateCurrentMusic();
	}

	if (tetromino.GetFallen())
	{
		board.ClearLines(tetromino.GetCurrentPieceId(), lastAction, tetromino.GetAlias(), tetromino.GetPos());
		tetromino.SetCurrentPiece(tetrominoesList[0]);
		inputManager.LoadTetromino(tetrominoesList[0], tetromino);
		tetrominoesList.erase(tetrominoesList.begin());
		tetrominoesList.push_back(SelectRandomPiece());
		std::cout << "\nrandom piece selected ";
		tetromino.SetPos({ board.GetWidth() / 2 - tetromino.GetDimension() / 2, 0 });
		while (!tetromino.IsBottomButTop())
		{
			tetromino.SetPos({ tetromino.GetPos().GetX(), tetromino.GetPos().GetY() - 1 });
		}
		tetromino.SetFallen(false);
		tetromino.SetIsAnythingSetToHeld(false);
		tetromino.SetRotation(Tetromino::Rotation::UP);
	}

	if (IsKeyPressed(KEY_X))
	{
		tetromino.RotateClockwise();
		if (tetromino.IsBottom())
		{
			counterDrop = 40;
			counterFall = 1;
		}
		lastAction = 1;
	}
	if (IsKeyPressed(KEY_Z))
	{
		tetromino.RotateCounterClockwise();
		if (tetromino.IsBottom())
		{
			counterDrop = 40;
			counterFall = 1;
		}
		lastAction = 1;
	}
	if (IsKeyPressed(KEY_W))
	{
		tetromino.RotateFull();
		if (tetromino.IsBottom())
		{
			counterDrop = 40;
			counterFall = 1;
		}
		lastAction = 1;
	}

	if (IsKeyDown(KEY_LEFT))
	{
		counterMove++;
		if (counterMove > 10)
		{
			counterKeepMoving++;
			if (counterKeepMoving > 1)
			{
				tetromino.MoveLeft();
				counterKeepMoving = 0;

				if (tetromino.IsBottom())
				{
					counterDrop = 40;
					counterFall = 1;
				}
			}
		}
		if (tetromino.IsBottom())
		{
			counterDrop = 40;
			counterFall = 1;
		}
		lastAction = 2;
	}
	if (IsKeyReleased(KEY_LEFT))
	{
		counterMove = 0, counterKeepMoving = 0;
	}
	if (IsKeyPressed(KEY_LEFT))
	{
		tetromino.MoveLeft();

		if (tetromino.IsBottom())
		{
			counterDrop = 40;
			counterFall = 1;
		}
		lastAction = 2;
	}

	if (IsKeyDown(KEY_RIGHT))
	{
		counterMove++;

		if (counterMove > 10)
		{
			counterKeepMoving++;
			if (counterKeepMoving > 1)
			{
				tetromino.MoveRight();
				counterKeepMoving = 0;

				if (tetromino.IsBottom())
				{
					counterDrop = 40;
					counterFall = 1;
				}
			}
		}
		if (tetromino.IsBottom())
		{
			counterDrop = 40;
			counterFall = 1;
		}
		lastAction = 2;
	}
	if (IsKeyReleased(KEY_RIGHT))
	{
		counterMove = 0, counterKeepMoving = 0;
	}
	if (IsKeyPressed(KEY_RIGHT))
	{
		tetromino.MoveRight();

		if (tetromino.IsBottom())
		{
			counterDrop = 40;
			counterFall = 1;
		}
		lastAction = 2;
	}

	if (IsKeyDown(KEY_DOWN))
	{
		if (!tetromino.IsBottom())
		{
			counterFall += 15 + board.GetSpeed();
		}
		counterFall += 5 + board.GetSpeed();
	}

	if (IsKeyPressed(KEY_C))
	{
		if (!tetromino.getIsAnythingHeld())
		{
			if (inputManager.GetHeld() != -1)
			{
				int current = tetromino.GetCurrentPieceId();
				inputManager.LoadTetromino(inputManager.GetHeld(), tetromino);
				inputManager.SetHeld(current);
				tetromino.SetPos({ board.GetWidth() / 2 - tetromino.GetDimension() / 2, 0 });
				while (!tetromino.IsBottomButTop())
				{
					tetromino.SetPos({ tetromino.GetPos().GetX(), tetromino.GetPos().GetY() - 1 });
				}
				tetromino.SetRotation(Tetromino::Rotation::UP);
			}
			else
			{
				inputManager.SetHeld(tetromino.GetCurrentPieceId()); tetromino.SetCurrentPiece(tetrominoesList[0]);
				inputManager.LoadTetromino(tetrominoesList[0], tetromino);
				tetrominoesList.erase(tetrominoesList.begin());
				tetrominoesList.push_back(SelectRandomPiece());
				std::cout << "\nrandom piece selected ";
				tetromino.SetPos({ board.GetWidth() / 2 - tetromino.GetDimension() / 2, 0 });
				tetromino.SetRotation(Tetromino::Rotation::UP);
			}
			tetromino.SetIsAnythingSetToHeld(true);
		}
	}

	if (IsKeyPressed(KEY_SPACE))
	{
		tetromino.HardDrop();
		counterDrop = -1;
	}

	counterFall += board.GetSpeed() + 1;
	board.DrawTimerLine(counterDrop);

	if (tetromino.IsBottom())
	{
		counterDrop--;
		if (counterDrop <= 0)
		{
			tetromino.PlaceTetromino(gameShouldEnd);
			if (!gameShouldEnd)
			{
				soundManager.PlaySoundFromName("placeSound");
			}
			counterDrop = 40;
			counterFall = 1;
		}
	}
	else
	{
		counterDrop = 40;
		if (counterFall >= 61)
		{
			tetromino.Fall();
			board.IncreaseScore((int)((1 + board.GetLevel()) * 1.5));
			counterDrop = 40;
			counterFall = 1;
			lastAction = 0;
		}
	}
}

int Game::SelectRandomPiece() const
{
	return rand() % inputManager.GetTetrominoAmount();
}

void Game::DrawFuturePieces()
{
	for (int z = 0; z < inputManager.GetTetrominoPreviewAmount(); z++)
	{
		inputManager.LoadTetromino(tetrominoesList[z], futureMino);
		std::vector<bool> shape(futureMino.GetDimension() * futureMino.GetDimension());
		Vec2<int> pos = { settings::boardWidthHeight.GetX() + 3, (z * (inputManager.GetMaxDimension())) };
		for (int y = 0; y < futureMino.GetDimension(); y++)
		{
			for (int x = 0; x < futureMino.GetDimension(); x++)
			{
				bool cell = false;

				shape = futureMino.GetShape();
				cell = shape[y * futureMino.GetDimension() + x];

				if (cell)
				{
					if (futureMino.GetDimension() < inputManager.GetMaxDimension())
					{
						board.DrawFutureCell(pos + Vec2<int>{x - ((futureMino.GetDimension() - inputManager.GetMaxDimension()) / 2),
							y - ((futureMino.GetDimension() - inputManager.GetMaxDimension()) / 2)}, futureMino.GetColor());
					}
					else
					{
						board.DrawFutureCell(pos + Vec2<int>{x, y}, futureMino.GetColor());
					}
				}
			}
		}

	}

	if (inputManager.GetHeld() != -1)
	{
		inputManager.LoadTetromino(inputManager.GetHeld(), heldMino);
		for (int y = 0; y < heldMino.GetDimension(); y++)
		{
			std::vector<bool> shape(heldMino.GetDimension() * heldMino.GetDimension());
			Vec2<int> pos = { -4 , 0};
			for (int x = 0; x < heldMino.GetDimension(); x++)
			{
				bool cell = false;

				shape = heldMino.GetShape();
				cell = shape[y * heldMino.GetDimension() + x];

				if (cell)
				{
					if (tetromino.getIsAnythingHeld())
					{
						board.DrawHeldCell(pos + Vec2<int>{x, y}, LIGHTGRAY);
					}
					else 
					{
						board.DrawHeldCell(pos + Vec2<int>{x, y}, heldMino.GetColor());
					}
				}
			}
		}
	}

	board.DrawFutureBoard({ ((settings::boardWidthHeight.GetX() + 8) * board.GetCellSize()), board.GetScreenPos().GetY() - 26 },
		{ (5 * board.GetCellSize()) + board.GetCellSize() - 6, (inputManager.GetTetrominoPreviewAmount() * board.GetCellSize() * inputManager.GetMaxDimension()) - 5 + (board.GetCellSize()) * 2 });
	board.DrawFutureBorder({ ((settings::boardWidthHeight.GetX() + 8) * board.GetCellSize()), board.GetScreenPos().GetY() - 26 },
		{ (5 * board.GetCellSize()) + board.GetCellSize() - 6, (inputManager.GetTetrominoPreviewAmount() * board.GetCellSize() * inputManager.GetMaxDimension()) - 5 + (board.GetCellSize() * 2) });
	board.DrawFutureBoardGrid({ ((settings::boardWidthHeight.GetX() + 8) * board.GetCellSize()), board.GetScreenPos().GetY() - 20 }, inputManager.GetTetrominoPreviewAmount(), inputManager.GetMaxDimension());
}

void Game::DrawDrawMino()
{
	drawMino.AlignPos(tetromino);
	drawMino.Draw(1);
}

void Game::GameOver()
{
	board.Draw();
	tetromino.Draw();
	board.SaveScore(boardName, newBest);
	raycpp::DrawRectangle({ 0, 0 }, { settings::screenWidth, settings::screenHeight }, Color{ 0, 0, 0, 30 });

	if (newBest)
	{
		DrawText("GAME\nOVER", settings::levelCounterPosition.GetX() - 400, 300, settings::scoreCounterSize, Color{ 255, 0, 0, 100 });
		DrawText("NEW BEST", settings::levelCounterPosition.GetX() - 400, 600, settings::scoreCounterSize - 20, Color{ 255, 69, 0, 100 });
	}
	else
	{
		DrawText("GAME\nOVER", settings::levelCounterPosition.GetX() - 400, 300, settings::scoreCounterSize, Color{ 255, 25, 13, 100 });
	}
}