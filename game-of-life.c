#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef enum Window { START, MAIN } Window;

typedef struct Cell {
    bool active;
    Rectangle bounds;
} Cell;

bool *evaluate(int ROWS, int COLS, Cell grid[ROWS][COLS], int row, int col) {
    bool *result = malloc(3 * sizeof(bool));
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0)
                continue;
            if (row + i >= 0 && row + i < ROWS && col + j >= 0 &&
                col + j < COLS) {
                if (grid[row + i][col + j].active) {
                    count++;
                }
            }
        }
    }
    result[0] = count < 2;
    result[1] = count > 3;
    result[2] = count == 3;
    return result;
}

int main() {
    SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_MSAA_4X_HINT);
    InitWindow(0, 0, "grid");
    SetTargetFPS(60);

    Window window = START;

    const float screenWidth = GetScreenWidth();
    const float screenHeight = GetScreenHeight();
    const int maxGridSize = 140;
    const float cellSize = fminf(screenWidth, screenHeight) / maxGridSize;
    const int ROWS = (int)(screenHeight / cellSize);
    const int COLS = (int)(screenWidth / cellSize);

    Cell grid[ROWS][COLS];
    Cell gridNext[ROWS][COLS];
    Cell gridSaved[ROWS][COLS];

    bool pause = false;
    bool *result;

    int generation = 5;
    int fpsCounter = 0;

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            grid[row][col] = (Cell){.active = false,
                .bounds = {
                .width = cellSize,
                .height = cellSize,
                    .x = col * cellSize,
                    .y = row * cellSize,
                }};
            if (row == col || row + col == ROWS) {
                grid[row][col].active = true;
            }
        }
    }

    while (!WindowShouldClose()) {
        ClearBackground(RAYWHITE);
        BeginDrawing();
        fpsCounter++;
        if (IsKeyPressed('=') && generation < 60) {
            generation += 5;
        }
        if (IsKeyPressed('-') && generation > 0) {
            generation -= 5;
        }
        if (IsKeyPressed('R')) {
            memcpy(grid, gridSaved, sizeof(grid));
            pause = false;
            window = START;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int posX = GetMouseX() / cellSize;
            int posY = GetMouseY() / cellSize;
            if (posX >= 0 && posX < COLS && posY >= 0 && posY < ROWS) {
                grid[posY][posX].active = !grid[posY][posX].active;
            }
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            int posX = GetMouseX() / cellSize;
            int posY = GetMouseY() / cellSize;
            if (posX >= 0 && posX < COLS && posY >= 0 && posY < ROWS) {
                grid[posY][posX].active = !grid[posY][posX].active;
            }
        }
        if (IsKeyPressed(KEY_SPACE)) {
            pause = !pause;
        }

        switch (window) {
            case START:
                if (IsKeyPressed(KEY_ENTER)){
                    memcpy(gridSaved, grid, sizeof(grid));
                    window = MAIN;
                }
                break;
            case MAIN:
                if (fpsCounter >= generation && !pause) {
                    memcpy(gridNext, grid, sizeof(grid));
                    for (int row = 0; row < ROWS; row++) {
                        for (int col = 0; col < COLS; col++) {
                            result = evaluate(ROWS, COLS, grid, row, col);
                            if (grid[row][col].active) {
                                if (result[0] || result[1])
                                    gridNext[row][col].active = false;
                            } else {
                                if (result[2])
                                    gridNext[row][col].active = true;
                            }
                            free(result);
                        }
                    }
                    memcpy(grid, gridNext, sizeof(grid));
                    fpsCounter = 0;
                }
                break;
        }
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                Color cellColor = grid[row][col].active ? BLACK : RAYWHITE;
                DrawRectangleRec(grid[row][col].bounds, cellColor);
                DrawRectangleLinesEx(grid[row][col].bounds, 0.3f, LIGHTGRAY);
            }
        }
        DrawText(TextFormat("Generation Delay: %d", generation), 30, 30, 34, GRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
