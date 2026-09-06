/**
 * @file minesweeper.c
 * @author Patrik Viljehav (patrik.viljehav@gmail.com)
 * @brief Minesweeper game
 * @version 1.0
 * @date 2024-10-23
 *
 * @copyright Copyright (c) 2024
 *
 */



   /* Raylib */

#include "include/raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"
#include "include/rlgl.h"

/* Standard includes */

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/* local includes */
#include "linmath.h"
#include "render.h"
#include "minesweeper.h"

/* Defines / Macros */

#define IS_VALID(r, c, layout) ((r) >= 0 && (r) < (layout).rows && (c) >= 0 && (c) < (layout).cols)

/* Function prototypes */

void resetGameWindow(game_t *pGame);
void newGame(game_t *pGame);
void drawNewGame(game_t *pGame, dropdown_t *pGridSize);
bool IsDigit(char ch);
void validateIntInput(char *pInput, size_t length);
void playMineSweeper(game_t *pGame);
void play3D(game_t *pGame);
void gameOverDialog(game_t *pGame);
void gameWonDialog(game_t *pGame);
unsigned char increaseByOne(unsigned char value);
void coordinateToArrayIndex(game_t *pGame, int *pIdx, int x, int y);
void arrayIndexToCoordinate(game_t *pGame, int idx, int *pX, int *pY);
void floodFill(game_t *pGame, int index);
int getMouseClick(game_t *pGame, MouseButton mouseButton, int *pIndex);
void drawHeader(game_t *pGame);
int drawGameArea(game_t *pGame, bool win);
void middleClick(game_t *pGame, int idx);
void showAllMines(game_t *pGame, bool win);

#include "play3D.c" // NOTE: including this here is temporary!

Image bg;
Texture2D background;

int main(void)
{
    // Init variables
    game_t game = {0};
    dropdown_t gridSize;
    game.settings.volume = 0.5;

    srand(time(NULL)); // TODO: Improve if necessary
    // Start new game
    resetGameWindow(&game);
    game.state = GAME_NEW;
    gridSize.edit = false;
    gridSize.active = 0;
    game.layout.buttonSize = 20;
    game.layout.spacing = 1;
    game.layout.headerHeight = 40;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(game.window.width, game.window.height, game.window.name);

    SetExitKey(0);
    SetTargetFPS(60);
    loadTextures();
    Image topIcon = LoadImage("assets/logo.png");
    bg = LoadImage("assets/background.png");
    background = LoadTextureFromImage(bg);
    Font font;
    if (FileExists("assets/Inter-Regular.ttf"))
    {
        font = LoadFont("assets/Inter-Regular.ttf");
    }
    else if (FileExists("assets/Roboto-Regular.ttf"))
    {
        font = LoadFont("assets/Roboto-Regular.ttf");
    }
    else
    {
        font = LoadFont("assets/RugenExpanded-DOKGE.otf");
    }

    SetWindowIcon(topIcon);
    InitAudioDevice();
    Music music = LoadMusicStream("assets/music.mp3");
    SetMusicVolume(music, game.settings.volume);
    PlayMusicStream(music);

    while (!WindowShouldClose() && game.state != QUIT)
    {
        float mouseWheel;
        UpdateMusicStream(music);
        mouseWheel = GetMouseWheelMove();
        if (mouseWheel > 0)
        {
            game.settings.volume = game.settings.volume >= 1 ? 1.0 : game.settings.volume + 0.1;
        } else if (mouseWheel < 0)
        {
            game.settings.volume = game.settings.volume <= 0 ? 0 : game.settings.volume - 0.1;
        }
        SetMusicVolume(music, game.settings.volume);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        switch (game.state)
        {
            case GAME_NEW:
                GuiSetFont(GetFontDefault());
                drawNewGame(&game, &gridSize);
                break;
            case GAME_ONGOING:
                GuiSetFont(font);
                playMineSweeper(&game);
                break;
            case GAME_3D:
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    resetGameWindow(&game);
                    RestoreWindow();
                    EnableCursor();
                    SetWindowSize(game.window.width, game.window.height);
                    game.state = GAME_NEW;
                }
                play3D(&game);
                break;
            case GAME_OVER_3D:
                /* keep rendering the 3D scene, but show a centered overlay without 2D popup */
                play3D(&game);
                {
                    const char *msg = "GAME OVER";
                    int fontSize = 40;
                    int tw = MeasureText(msg, fontSize);
                    int sw = GetScreenWidth();
                    int sh = GetScreenHeight();
                    int tx = (sw - tw) / 2;
                    int ty = (sh - fontSize) / 2;
                    DrawText(msg, tx, ty, fontSize, RED);
                    /* Instructions under the main message */
                    const char *hint = "Press Esc to quit, R to replay.";
                    int hintSize = 20;
                    int th = MeasureText(hint, hintSize);
                    int hx = (sw - th) / 2;
                    DrawText(hint, hx, ty + fontSize + 10, hintSize, LIGHTGRAY);
                }
                /* Simple controls: R to restart 3D mode, ESC to quit */
                if (IsKeyPressed(KEY_R))
                {
                    newGame(&game);
                    init3D(&game);
                    /* clear any persistent top-left messages from previous run */
                    textToDisplay("", 's');
                    game.state = GAME_3D;
                }
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    resetGameWindow(&game);
                    RestoreWindow();
                    EnableCursor();
                    SetWindowSize(game.window.width, game.window.height);
                    game.state = GAME_NEW;
                }
                break;
            case GAME_WON_3D:
                /* keep rendering the 3D scene, but show a centered overlay without 2D popup */
                play3D(&game);
                {
                    const char *msg = "GAME WON!";
                    int fontSize = 40;
                    int tw = MeasureText(msg, fontSize);
                    int sw = GetScreenWidth();
                    int sh = GetScreenHeight();
                    int tx = (sw - tw) / 2;
                    int ty = (sh - fontSize) / 2;
                    DrawText(msg, tx, ty, fontSize, GREEN);
                    /* Instructions under the main message */
                    const char *hint = "Press Esc to quit, R to replay.";
                    int hintSize = 20;
                    int th = MeasureText(hint, hintSize);
                    int hx = (sw - th) / 2;
                    DrawText(hint, hx, ty + fontSize + 10, hintSize, LIGHTGRAY);
                }
                /* Simple controls: R to restart 3D mode, ESC to quit */
                if (IsKeyPressed(KEY_R))
                {
                    newGame(&game);
                    init3D(&game);
                    /* clear any persistent top-left messages from previous run */
                    textToDisplay("", 's');
                    game.state = GAME_3D;
                }
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    resetGameWindow(&game);
                    RestoreWindow();
                    EnableCursor();
                    SetWindowSize(game.window.width, game.window.height);
                    game.state = GAME_NEW;
                }
                break;
            case GAME_OVER:
                showAllMines(&game, false);
                {
                    const char *msg = "GAME OVER";
                    int fontSize = 40;
                    int tw = MeasureText(msg, fontSize);
                    int sw = GetScreenWidth();
                    int sh = GetScreenHeight();
                    int tx = (sw - tw) / 2;
                    int ty = (sh - fontSize) / 2;
                    DrawText(msg, tx, ty, fontSize, RED);
                }
                gameOverDialog(&game);
                break;
            case GAME_WON:
                showAllMines(&game, true);
                gameWonDialog(&game);
                break;
            default:
                break;
        }
        EndDrawing();
    }

    if (game.layout.minePositions != NULL)
    {
        free(game.layout.minePositions);
        game.layout.minePositions = NULL;
    }
    if (game.layout.boxContent != NULL)
    {
        free(game.layout.boxContent);
        game.layout.boxContent = NULL;
    }
    if (game.layout.grassSeeds != NULL)
    {
        free(game.layout.grassSeeds);
        game.layout.grassSeeds = NULL;
    }
    UnloadMusicStream(music);
    UnloadFont(font);
    CloseWindow();
    return 0;
}

/**
 * @brief Reset the game
 *
 * @param pGame game struct
 */
void resetGameWindow(game_t *pGame)
{
    pGame->window.height = 350;
    pGame->window.width = 600;
    char name[] = "Minesweeper";
    strncpy(pGame->window.name, name, sizeof(name));
}

/**
 * @brief Reset the game board and start a new game.
 *
 * @param pGame pointer to the game variable
 */
void newGame(game_t *pGame)
{
    // Free the previously allocated memory if it exists
    if (pGame->layout.minePositions != NULL)
    {
        free(pGame->layout.minePositions);
        pGame->layout.minePositions = NULL;
    }
    if (pGame->layout.boxContent != NULL)
    {
        free(pGame->layout.boxContent);
        pGame->layout.boxContent = NULL;
    }
    if (pGame->layout.grassSeeds != NULL)
    {
        free(pGame->layout.grassSeeds);
        pGame->layout.grassSeeds = NULL;
    }

    pGame->layout.cells = pGame->layout.rows * pGame->layout.cols;
    pGame->stats.minesleft = pGame->layout.mines;
    pGame->stats.timer = 0;
    pGame->layout.minePositions = malloc(pGame->layout.cells * sizeof(unsigned char));
    pGame->layout.boxContent = malloc(pGame->layout.cells * sizeof(boxType_e));
    pGame->layout.grassSeeds = malloc(pGame->layout.cells * sizeof(unsigned int));
    // minePositions:
    // MINE (9) = mine
    // 0 = No mine
    // 1-8 = adjecent mines.

    for (int i = 0; i < pGame->layout.cells; i++)
    {
        pGame->layout.boxContent[i] = BUTTON_UNPRESSED;
        pGame->layout.minePositions[i] = 0;
        pGame->layout.grassSeeds[i] = ((unsigned int)rand() << 16) ^ (unsigned int)rand();
    }

    for (int i = 0; i < pGame->layout.mines; i++)
    {
        // pGame->layout.minePositions[i] = -1;
        unsigned char done = 0;
        int tempx;
        while (!done)
        {
            done = 1;
            tempx = rand() % (pGame->layout.cells);
            if (pGame->layout.minePositions[tempx] == MINE)
            {
                done = 0;
            }
        }
        pGame->layout.minePositions[tempx] = MINE;
    }
    for (int i = 0, x = 0, y = 0; i < (pGame->layout.cells); i++, x++)
    {
        if (pGame->layout.minePositions[i] == MINE)
        {
            if (x == 0) // Check if we are in the first column
            {
                pGame->layout.minePositions[i + 1] = increaseByOne(pGame->layout.minePositions[i + 1]);
                if (y == 0) // first row
                {
                    pGame->layout.minePositions[i + pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols]);
                    pGame->layout.minePositions[i + pGame->layout.cols + 1] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols + 1]);
                } else if (y == pGame->layout.rows - 1) // last row
                {
                    pGame->layout.minePositions[i - pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols]);
                    pGame->layout.minePositions[i - pGame->layout.cols + 1] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols + 1]);
                } else
                {
                    pGame->layout.minePositions[i + pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols]);
                    pGame->layout.minePositions[i + pGame->layout.cols + 1] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols + 1]);
                    pGame->layout.minePositions[i - pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols]);
                    pGame->layout.minePositions[i - pGame->layout.cols + 1] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols + 1]);
                }
            } else if (i % pGame->layout.cols == pGame->layout.cols - 1) // check if we are in the last column
            {
                pGame->layout.minePositions[i - 1] = increaseByOne(pGame->layout.minePositions[i - 1]);
                if (y == 0) // first row
                {
                    pGame->layout.minePositions[i + pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols]);
                    pGame->layout.minePositions[i + pGame->layout.cols - 1] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols - 1]);
                } else if (y == pGame->layout.rows - 1) // last row
                {
                    pGame->layout.minePositions[i - pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols]);
                    pGame->layout.minePositions[i - pGame->layout.cols - 1] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols - 1]);
                } else
                {
                    pGame->layout.minePositions[i + pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols]);
                    pGame->layout.minePositions[i + pGame->layout.cols - 1] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols - 1]);
                    pGame->layout.minePositions[i - pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols]);
                    pGame->layout.minePositions[i - pGame->layout.cols - 1] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols - 1]);
                }
            } else if (y == 0) // check if we are in the first row
            {
                pGame->layout.minePositions[i + pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols]);
                if (x == 0) // first columm
                {
                    pGame->layout.minePositions[i + 1] = increaseByOne(pGame->layout.minePositions[i + 1]);
                    pGame->layout.minePositions[i + pGame->layout.cols + 1] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols + 1]);
                } else if (x == pGame->layout.cols - 1) // last column
                {
                    pGame->layout.minePositions[i - 1] = increaseByOne(pGame->layout.minePositions[i - 1]);
                    pGame->layout.minePositions[i + pGame->layout.cols - 1] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols - 1]);
                } else
                {
                    pGame->layout.minePositions[i + 1] = increaseByOne(pGame->layout.minePositions[i + 1]);
                    pGame->layout.minePositions[i + pGame->layout.cols + 1] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols + 1]);
                    pGame->layout.minePositions[i - 1] = increaseByOne(pGame->layout.minePositions[i - 1]);
                    pGame->layout.minePositions[i + pGame->layout.cols - 1] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols - 1]);
                }
            } else if (y == pGame->layout.rows - 1) // check if we are in the last row
            {
                pGame->layout.minePositions[i - pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols]);
                if (x == 0) // first columm
                {
                    pGame->layout.minePositions[i + 1] = increaseByOne(pGame->layout.minePositions[i + 1]);
                    pGame->layout.minePositions[i - pGame->layout.cols + 1] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols + 1]);
                } else if (x == pGame->layout.cols - 1) // last column
                {
                    pGame->layout.minePositions[i - 1] = increaseByOne(pGame->layout.minePositions[i - 1]);
                    pGame->layout.minePositions[i - pGame->layout.cols - 1] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols - 1]);
                } else
                {
                    pGame->layout.minePositions[i + 1] = increaseByOne(pGame->layout.minePositions[i + 1]);
                    pGame->layout.minePositions[i - pGame->layout.cols + 1] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols + 1]);
                    pGame->layout.minePositions[i - 1] = increaseByOne(pGame->layout.minePositions[i - 1]);
                    pGame->layout.minePositions[i - pGame->layout.cols - 1] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols - 1]);
                }
            } else // We are somewhere in the middle
            {
                pGame->layout.minePositions[i - 1] = increaseByOne(pGame->layout.minePositions[i - 1]);
                pGame->layout.minePositions[i + 1] = increaseByOne(pGame->layout.minePositions[i + 1]);
                pGame->layout.minePositions[i - pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols]);
                pGame->layout.minePositions[i - pGame->layout.cols + 1] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols + 1]);
                pGame->layout.minePositions[i - pGame->layout.cols - 1] = increaseByOne(pGame->layout.minePositions[i - pGame->layout.cols - 1]);
                pGame->layout.minePositions[i + pGame->layout.cols] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols]);
                pGame->layout.minePositions[i + pGame->layout.cols + 1] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols + 1]);
                pGame->layout.minePositions[i + pGame->layout.cols - 1] = increaseByOne(pGame->layout.minePositions[i + pGame->layout.cols - 1]);
            }
        }
        if (x == pGame->layout.cols - 1)
        {
            x = -1;
            y += 1;
        }
    }
    pGame->stats.timeDiff = GetTime();
}

/**
 * @brief Draw the window for creating a new game
 *
 * @param pGame game struct
 * @param pGridSize dropdown content for selecting game rows
 */
void drawNewGame(game_t *pGame, dropdown_t *pGridSize)
{
    static bool rowtextBoxEditMode = false;
    static bool coltextBoxEditMode = false;
    static bool minestextBoxEditMode = false;
    static char rows[6] = "9";
    static char columns[6] = "9";
    static char mines[6] = "10";
    static int showPop = 0;
    static bool maxMode = false;
    int tempVal;

    Rectangle rowTextbox;
    Rectangle colTextbox;
    Rectangle minesTextbox;
    Rectangle settings = {40, 50, 200, 200};
    Rectangle gameProperties = {pGame->window.width - 200 - 40, 50, 200, 240};
    Rectangle temp_r;

    // Styling
    GuiSetStyle(LABEL, TEXT, ColorToInt(RED));
    GuiSetStyle(DEFAULT, TEXT, ColorToInt(RED)); // TODO !IMPORTANT does not work
    ClearBackground(BLACK);
    DrawTextureEx(background, (Vector2){ 50, -50 }, 0, 0.5, WHITE);

    // Draw settings
    // =============
    GuiDrawRectangle(settings, 0, WHITE, Fade(WHITE, 0.75));
    GuiGroupBox(settings, " == SETTINGS == ");
    temp_r.width = GetTextWidth("VOLUME ");
    temp_r.x = settings.x + ((settings.width - temp_r.width) / 2);
    temp_r.y = settings.y + 20;
    temp_r.height = 20;
    GuiLabel(temp_r, "VOLUME");
    temp_r.width = 130;
    temp_r.x = settings.x + ((settings.width - temp_r.width) / 2) - 5;
    temp_r.y += 20;
    temp_r.height = 10;
    GuiSlider(temp_r, "0", "100", &pGame->settings.volume, 0.0, 1.0);
    temp_r.x = settings.x + 15;
    temp_r.y += 50;
    temp_r.width = 10;
    temp_r.height = 10;

    // Draw game properties
    // ====================
    GuiDrawRectangle(gameProperties, 0, WHITE, Fade(WHITE, 0.75));
    // GuiDrawRectangle(gameProperties, 0, WHITE, Fade(SKYBLUE, 0.65));
    GuiGroupBox(gameProperties, " == GAME PROPERTIES == ");
    temp_r.width = GetTextWidth("Select game size ");
    temp_r.height = 25;
    temp_r.y = gameProperties.y + 15;
    temp_r.x = gameProperties.x + 15;

    GuiLabel(temp_r, "Select game size");

    // temp_r.height = 30;
    // temp_r.y += 25;
    temp_r.height = 25;
    temp_r.x += temp_r.width;

    // Dropdown box logic
    if (GuiDropdownBox(temp_r, "Beginner;Intermediate;Expert;Custom", &pGridSize->active, pGridSize->edit))
    {
        pGridSize->edit = !pGridSize->edit;

        // Update edit mode if "Custom" is selected
        // textBoxEditMode = (gridSize->active == 3);
    }

    // Set game parameters based on dropdown selection
    switch (pGridSize->active)
    {
        case 0:
            pGame->layout.rows = 9;
            pGame->layout.cols = 9;
            pGame->layout.mines = 10;
            strncpy(rows, "9", sizeof(rows));
            strncpy(columns, "9", sizeof(columns));
            strncpy(mines, "10", sizeof(mines));
            break;

        case 1:
            pGame->layout.rows = 16;
            pGame->layout.cols = 16;
            pGame->layout.mines = 40;
            strncpy(rows, "16", sizeof(rows));
            strncpy(columns, "16", sizeof(columns));
            strncpy(mines, "40", sizeof(mines));
            break;

        case 2:
            pGame->layout.rows = 16;
            pGame->layout.cols = 30;
            pGame->layout.mines = 99;
            strncpy(rows, "16", sizeof(rows));
            strncpy(columns, "30", sizeof(columns));
            strncpy(mines, "99", sizeof(mines));
            break;

        default:
            // Custom selection, rows/cols/mines can be edited
            break;
    }
    temp_r.x = gameProperties.x + 15;
    temp_r.width = 50;
    temp_r.height = 25;
    temp_r.y += 40;
    tempVal = temp_r.y;
    GuiLabel(temp_r, "Rows");
    temp_r.y += temp_r.height;
    GuiLabel(temp_r, "Columns");
    temp_r.y += temp_r.height;
    GuiLabel(temp_r, "Mines");

    temp_r.y = tempVal;
    temp_r.x += temp_r.width;
    temp_r.width = 40;
    memcpy(&rowTextbox, &temp_r, sizeof(temp_r));

    temp_r.y += temp_r.height;
    memcpy(&colTextbox, &temp_r, sizeof(temp_r));

    temp_r.y += temp_r.height;
    memcpy(&minesTextbox, &temp_r, sizeof(temp_r));

    // temp_b.top = minesLabel.y + 2 * minesLabel.height + 10;
    // temp_b.left = minesLabel.x;

    // Check if the textbox is clicked
    if (pGridSize->active == 3)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, rowTextbox))
            {
                rowtextBoxEditMode = true;
                coltextBoxEditMode = false;
                minestextBoxEditMode = false;
            }
            if (CheckCollisionPointRec(mousePos, colTextbox))
            {
                rowtextBoxEditMode = false;
                coltextBoxEditMode = true;
                minestextBoxEditMode = false;
            }
            if (CheckCollisionPointRec(mousePos, minesTextbox))
            {
                rowtextBoxEditMode = false;
                coltextBoxEditMode = false;
                minestextBoxEditMode = true;
            }
        }
    } else
    {
        rowtextBoxEditMode = false;
        coltextBoxEditMode = false;
        minestextBoxEditMode = false;
    }

    // New game button
    // ===============
    temp_r.x = gameProperties.x + ((gameProperties.width - 4 * minesTextbox.width) / 3);
    temp_r.y = minesTextbox.y + 60;
    temp_r.width = 2 * minesTextbox.width;
    temp_r.height = minesTextbox.height;

    if (maxMode || GuiButton(temp_r, "New game"))
    {
        if (pGame->layout.cols <= 0 || pGame->layout.rows <= 0 || pGame->layout.mines <= 0 ||
            pGame->layout.mines > pGame->layout.rows * pGame->layout.cols)
        {
            showPop = 1;
        } else
        {
            if (maxMode)
            {
                int windowWidth = GetScreenWidth();
                int windowHeight = GetScreenHeight();
                int maxX = (int)(windowWidth / (pGame->layout.buttonSize + pGame->layout.spacing));
                int maxY = (int)((windowHeight - pGame->layout.headerHeight) / (pGame->layout.buttonSize + pGame->layout.spacing));
                int maxMines = (int)(maxX * maxY * 0.2);
                pGame->layout.rows = maxY;
                pGame->layout.cols = maxX;
                pGame->layout.mines = maxMines;
                sprintf(rows, "%d", maxY);
                sprintf(columns, "%d", maxX);
                sprintf(mines, "%d", maxMines);
            }

            newGame(pGame);
            pGame->state = GAME_ONGOING;
            return;
        }
    }
    if (showPop)
    {
        int result = GuiMessageBox((Rectangle){ pGame->window.width / 2 - 100, pGame->window.height / 2 - 50, 200, 100 }, "Oh no!", "The math does not quite add up.\n", "Ok");
        if (result == 1)
        {
            showPop = 0;
        }
    }

    GuiTextBox(rowTextbox, rows, sizeof(rows), rowtextBoxEditMode);
    GuiTextBox(colTextbox, columns, sizeof(columns), coltextBoxEditMode);
    GuiTextBox(minesTextbox, mines, sizeof(mines), minestextBoxEditMode);

    temp_r.x = gameProperties.x + ((gameProperties.width - 4 * minesTextbox.width) / 3);
    temp_r.y += 1.5 * temp_r.height;
    temp_r.width = 2 * minesTextbox.width;
    temp_r.height = minesTextbox.height;

    if (GuiButton(temp_r, "MAX!"))
    {
        MaximizeWindow();
        maxMode = true;
    }
    temp_r.x += temp_r.width + ((gameProperties.width - 2 * temp_r.width) / 3);
    if (GuiButton(temp_r, "3D!"))
    {
        MaximizeWindow();
        newGame(pGame);
        init3D(pGame);
        pGame->state = GAME_3D;
    }

    validateIntInput(rows, strlen(rows));
    validateIntInput(columns, strlen(columns));
    validateIntInput(mines, strlen(mines));
    int newRows = atoi(rows);
    if (newRows > 0) // Ensure the input is a valid number
    {
        pGame->layout.rows = newRows; // TODO, if the input box are empty, last valid digit will be used which might be misleading
    }
    int newColumns = atoi(columns);
    if (newColumns > 0) // Ensure the input is a valid number
    {
        pGame->layout.cols = newColumns; // TODO, if the input box are empty, last valid digit will be used which might be misleading
    }
    int newMines = atoi(mines);
    if (newMines > 0) // Ensure the input is a valid number
    {
        pGame->layout.mines = newMines; // TODO, if the input box are empty, last valid digit will be used which might be misleading
    }
}

/**
 * @brief Validate that a char is an int
 *
 * @param ch the char
 * @return true
 * @return false
 */
bool IsDigit(char ch)
{
    return (ch >= '0' && ch <= '9');
}

/**
 * @brief Validate that a string only contains digits, remove any other chars.
 *
 * @param pInput string to fiddle with
 * @param length length of string
 */
void validateIntInput(char *pInput, size_t length)
{
    char new_input[12] = {0};
    int j = 0;
    for (int i = 0; i < length; i++)
    {
        if (IsDigit(pInput[i]))
        {
            new_input[j] = pInput[i];
            j++;
        }
    }
    while (j < length)
    {
        new_input[j] = 0;
        j++;
    }
    strncpy(pInput, new_input, length);
}

/**
 * @brief Main game loop
 *
 * @param pGame pointer to the game variable
 */
void playMineSweeper(game_t *pGame)
{
    int header = pGame->layout.headerHeight;
    Rectangle button;
    button.height = pGame->layout.buttonSize;
    button.width = pGame->layout.buttonSize;
    pGame->window.width = pGame->layout.cols * button.width + (pGame->layout.cols * pGame->layout.spacing) - pGame->layout.spacing;
    pGame->window.height = pGame->layout.rows * button.height + header + (pGame->layout.rows * pGame->layout.spacing) - pGame->layout.spacing;
    int buttonClickedOn;
    int leftToOpen = 0;
    static int showPopUp = 0;

    SetWindowSize(pGame->window.width, pGame->window.height);
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    if (1 == getMouseClick(pGame, MOUSE_BUTTON_LEFT, &buttonClickedOn))
    {
        if (pGame->layout.boxContent[buttonClickedOn] != BUTTON_FLAGGED)
        {
            if (MINE == pGame->layout.minePositions[buttonClickedOn])
            { // mine
                pGame->layout.boxContent[buttonClickedOn] = OPEN_MINE;
            } else
            { // no mine
                if (pGame->layout.minePositions[buttonClickedOn] == 0)
                {
                    floodFill(pGame, buttonClickedOn);
                } else
                {
                    pGame->layout.boxContent[buttonClickedOn] = OPEN_WITH_HINT;
                }
            }
        }
    } else if (1 == getMouseClick(pGame, MOUSE_BUTTON_MIDDLE, &buttonClickedOn))
    {
        middleClick(pGame, buttonClickedOn);
    } else if (1 == getMouseClick(pGame, MOUSE_BUTTON_RIGHT, &buttonClickedOn))
    {
        if (pGame->layout.boxContent[buttonClickedOn] == BUTTON_FLAGGED)
        {
            pGame->layout.boxContent[buttonClickedOn] = BUTTON_UNPRESSED;
            pGame->stats.minesleft++;
        } else if (pGame->layout.boxContent[buttonClickedOn] == BUTTON_UNPRESSED)
        {
            pGame->layout.boxContent[buttonClickedOn] = BUTTON_FLAGGED;
            pGame->stats.minesleft--;
        }
    } else
    {
        buttonClickedOn = -1;
    }

    // Draw everything
    leftToOpen = drawGameArea(pGame, false);

    if (0 == leftToOpen - pGame->layout.mines)
    {
        pGame->state = GAME_WON;
    }
    drawHeader(pGame);
}

/**
 * @brief Game over, see what the player want to do about it.
 *
 * @param pGame pointer to game thing..
 */
void gameOverDialog(game_t *pGame)
{
    int result = GuiMessageBox((Rectangle){pGame->window.width / 2 - 100, pGame->window.height / 2 - 50, 200, 100}, "Oh no!", "GAME OVER", "Quit;Restart");
    if (result == 2)
    {
        pGame->state = GAME_ONGOING;
        newGame(pGame);
    } else if (result == 1)
    {
        pGame->state = QUIT;
    }
}

/**
 * @brief Game won, see what the player want to do about it.
 *
 * @param pGame pointer to game thing..
 */
void gameWonDialog(game_t *pGame)
{
    int result = GuiMessageBox((Rectangle){ pGame->window.width / 2 - 75, pGame->window.height / 2 - 50, 150, 100 }, "CONGRATULATIONS!", "You have won!", "Quit;Restart");
    if (result == 2)
    {
        pGame->state = GAME_ONGOING;
        newGame(pGame);
    } else if (result == 1)
    {
        pGame->state = QUIT;
    }
}

/**
 * @brief Increase a number by one, unless it is a 9, in that case it is a mine.
 *
 * @param value the value to increase
 * @return unsigned char increased value
 */
unsigned char increaseByOne(unsigned char value)
{
    if (value == 9)
    {
        return value;
    }
    return value + 1;
}

/**
 * @brief convert x, y coordinates to an array index
 *
 * @param pGame game_t game data
 * @param pIdx pointer to an index variable
 * @param x x coordinate
 * @param y y coordinate
 */
void coordinateToArrayIndex(game_t *pGame, int *pIdx, int x, int y)
{
    *pIdx = y * pGame->layout.cols + x;
}

/**
 * @brief Convert array index to x, y coordinates
 *
 * @param pGame game_t game data.
 * @param idx Index to convert.
 * @param pX pointer to x value.
 * @param pY pointer to y value.
 */
void arrayIndexToCoordinate(game_t *pGame, int idx, int *pX, int *pY)
{
    *pX = idx % pGame->layout.cols;
    *pY = (int)((idx / pGame->layout.cols) + 0.5);
}

void floodFill(game_t *pGame, int index)
{
    int x, y;
    layout_t *layout = &pGame->layout;
    arrayIndexToCoordinate(pGame, index, &x, &y);

    // If the starting cell is out of bounds, return
    if (!IS_VALID(y, x, *layout))
    {
        return;
    }

    // Only check unopened spots
    if (layout->boxContent[index] != BUTTON_UNPRESSED)
    {
        return;
    }

    if (layout->minePositions[index] == 0)
    {
        // Mark this cell as opened
        layout->boxContent[index] = OPEN_NO_MINE;
    } else if (layout->minePositions[index] == MINE)
    {
        layout->boxContent[index] = OPEN_MINE;
    } else
    {
        layout->boxContent[index] = OPEN_WITH_HINT;
        // Base case: If cell has a mine hint, stop further recursion
        return;
    }

    // Directions for adjacent cells
    int directions[8][2] = {
        {-1, -1},
        {0, -1},
        {1, -1}, // top row
        {-1, 0},
        {1, 0}, // sides
        {-1, 1},
        {0, 1},
        {1, 1} // bottom row
    };

    // Recur for all adjacent cells
    for (int i = 0; i < 8; ++i)
    {
        int newRow = y + directions[i][1];
        int newCol = x + directions[i][0];

        if (IS_VALID(newRow, newCol, *layout))
        {
            int newIndex = newRow * layout->cols + newCol;
            if (layout->boxContent[newIndex] == BUTTON_UNPRESSED)
            {
                int idx;
                coordinateToArrayIndex(pGame, &idx, newCol, newRow);
                floodFill(pGame, idx);
            }
        }
    }
}

/**
 * @brief Get the Mouse Click object
 *
 * @param pGame game_t game object
 * @param mouseButton mousebutton to check for
 * @param pIndex the spot in the grid that was clicked
 * @return int 0 no click, 1 click, -1 ERROR
 */
int getMouseClick(game_t *pGame, MouseButton mouseButton, int *pIndex)
{
    // static int check = -1;

    if (IsMouseButtonReleased(mouseButton))
    {
        Vector2 mousePosition = GetMousePosition();
        for (int spotToFind = 0; spotToFind < pGame->layout.cells; spotToFind++)
        {
            int tX, tY;
            arrayIndexToCoordinate(pGame, spotToFind, &tX, &tY);
            int tempX = tX * pGame->layout.buttonSize + pGame->layout.spacing * tX;
            int tempY = tY * pGame->layout.buttonSize + pGame->layout.headerHeight + pGame->layout.spacing * tY;

            if (mousePosition.x > tempX && mousePosition.x < tempX + pGame->layout.buttonSize)
            {
                if (mousePosition.y > tempY && mousePosition.y < tempY + pGame->layout.buttonSize)
                {
                    *pIndex = spotToFind;
                    return 1;
                }
            }
        }
        return -1;
        // }
        // else
        // {
        //     printf("check: %d\n", check);
        //     return -2;
        // }
    }
    return 0;
}

/**
 * @brief Draw the header!
 *
 * @param pGame game thing
 */
void drawHeader(game_t *pGame)
{

    Rectangle headerBox = {0, 0, pGame->window.width, pGame->layout.headerHeight};
    Rectangle minesLeftBox = {pGame->window.width - 80, 7.5, 70, pGame->layout.headerHeight - 15};
    char minesLeft[6] = {'\0', '\0', '\0', '\0', '\0', '\0'};
    char timer[6] = {'\0', '\0', '\0', '\0', '\0', '\0'};
    int m, s;

    sprintf(minesLeft, "%05d", pGame->stats.minesleft);
    pGame->stats.timer = GetTime() - pGame->stats.timeDiff;

    m = (int)(pGame->stats.timer / 60) % 100;
    s = (int)((int)pGame->stats.timer % 60);
    m = m < 0 ? 99 : m; // cap at 99
    s = s < 0 ? 99 : s; // cap at 99
    // m can be max 99, s can be max 59. sprintf is safe.
    snprintf(timer, sizeof(timer), "%02d:%02d", m, s);

    GuiDrawRectangle(headerBox, 0, SKYBLUE, BLACK);
    GuiDrawRectangle(minesLeftBox, 2, LIME, BLACK);
    GetTextWidth(minesLeft);
    DrawText(minesLeft, minesLeftBox.x + ((minesLeftBox.width - GetTextWidth(minesLeft)) / 4), minesLeftBox.y + 4, 20, LIME);
    DrawText(timer, 10, minesLeftBox.y + 4, 20, LIME);
}

void middleClick(game_t *pGame, int idx)
{
    if (pGame->layout.boxContent[idx] != OPEN_WITH_HINT)
    {
        return;
    }
    int x, y;
    int flagged = 0;
    arrayIndexToCoordinate(pGame, idx, &x, &y);
    // Directions for adjacent cells
    int directions[8][2] = {
        {-1, -1},
        {0, -1},
        {1, -1}, // top row
        {-1, 0},
        {1, 0}, // sides
        {-1, 1},
        {0, 1},
        {1, 1} // bottom row
    };
    // make sure the correct amount of neighbours are flagged
    for (int i = 0; i < 8; ++i)
    {
        int newRow = y + directions[i][1];
        int newCol = x + directions[i][0];
        const layout_t *layout = &pGame->layout;

        if (IS_VALID(newRow, newCol, *layout))
        {
            int newIndex;
            coordinateToArrayIndex(pGame, &newIndex, newCol, newRow);
            if (layout->boxContent[newIndex] == BUTTON_FLAGGED)
            {
                flagged++;
            }
        }
    }
    if (pGame->layout.minePositions[idx] != flagged)
    {
        return;
    }

    // Recur for all adjacent cells
    for (int i = 0; i < 8; ++i)
    {
        int newRow = y + directions[i][1];
        int newCol = x + directions[i][0];
        const layout_t *layout = &pGame->layout;

        if (IS_VALID(newRow, newCol, *layout))
        {
            int newIndex;
            coordinateToArrayIndex(pGame, &newIndex, newCol, newRow);
            if (layout->boxContent[newIndex] == BUTTON_UNPRESSED)
            {
                int spot;
                coordinateToArrayIndex(pGame, &spot, newCol, newRow);
                floodFill(pGame, spot);
            }
        }
    }
    return;
}

/**
 * @brief Draw the game area except for the header
 *
 * @param pGame the game thingy
 * @param win win or not, only really applicable if true
 * @return int number of cells not opened
 */
int drawGameArea(game_t *pGame, bool win)
{
    int header = pGame->layout.headerHeight;
    int leftToOpen = 0;
    Rectangle button;
    button.height = pGame->layout.buttonSize;
    button.width = pGame->layout.buttonSize;

    for (int i = 0, x = 0, y = 0; i < (pGame->layout.cells); i++, x++)
    {
        button.y = header + (button.height * y) + (pGame->layout.spacing * y);
        button.x = button.width * x + pGame->layout.spacing * x;

        switch (pGame->layout.boxContent[i])
        {
            case BUTTON_UNPRESSED:
                GuiButton(button, NULL);
                break;
            case BUTTON_FLAGGED:
                GuiButton(button, NULL);
                GuiDrawIcon(ICON_GEAR_EX, button.x + 1, button.y + 2, 1, RED);
                break;
            case OPEN_NO_MINE:
                // fall-through
            case OPEN_WITH_HINT:

                if (pGame->layout.minePositions[i] > 0)
                {
                    Color color;
                    char text[2] = {'\0', '\0'};
                    text[0] = pGame->layout.minePositions[i] + '0';
                    text[1] = '\0';
                    Rectangle rect = {button.x, button.y, button.width, button.height};
                    int fontSize = 12; // Adjust to desired font size

                    // Calculate text width and height
                    int textWidth = MeasureText(text, fontSize);
                    int textHeight = fontSize; // Approximate height based on font size
                    // Calculate the centered position within the rectangle
                    int centerX = rect.x + (rect.width / 2) - (textWidth / 2);
                    int centerY = rect.y + (rect.height / 2) - (textHeight / 2);
                    // Draw the centered text
                    switch (pGame->layout.minePositions[i])
                    {
                        case 1:
                            color = BLUE;
                            break;
                        case 2:
                            color = DARKGREEN;
                            break;
                        case 3:
                            color = RED;
                            break;
                        case 4:
                            color = PURPLE;
                            break;
                        case 5:
                            color = MAROON;
                            break;
                        case 6:
                            color = SKYBLUE;
                            break;
                        case 7:
                            color = BLACK;
                            break;
                        case 8:
                            color = GRAY;
                            break;
                        default:
                            color = WHITE;
                    }
                    DrawText(text, centerX, centerY, fontSize, color);
                    GuiDrawText(text, rect, TEXT_ALIGN_CENTER, color);
                }

                GuiLabel(button, NULL);
                break;

            case OPEN_MINE:
                Color color = win ? GREEN : RED;
                GuiLabel(button, NULL);

                GuiDrawIcon(ICON_DEMON, button.x + 1, button.y + 2, 1, color);
                if (!win)
                {
                    pGame->state = GAME_OVER;
                }
                break;
        }

        if (x == pGame->layout.cols - 1)
        {
            x = -1;
            y += 1;
        }
        if (pGame->layout.boxContent[i] == BUTTON_FLAGGED || pGame->layout.boxContent[i] == BUTTON_UNPRESSED)
        {
            leftToOpen++;
        }
    }
    return leftToOpen;
}

/**
 * @brief Helper function for showing all mines when the game is over.
 *
 * @param pGame game struct
 * @param win bool for if we won or not
 */
void showAllMines(game_t *pGame, bool win)
{
    for (int i = 0; i < pGame->layout.cells; i++)
    {
        if (MINE == pGame->layout.minePositions[i])
        {
            pGame->layout.boxContent[i] = OPEN_MINE;
        }
    }
    drawGameArea(pGame, win);
}