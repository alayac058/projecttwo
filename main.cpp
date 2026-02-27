#include "CS3113/cs3113.h"
#include <math.h>
#include <string.h>

/**
* Author: Alaya Chowdhury
* Assignment: Pong Clone
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on Academic Misconduct.
**/

//Global variables
constexpr int SCREEN_WIDTH  = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int FPS           = 60;
AppStatus gAppStatus       = RUNNING;
float     gPreviousTicks   = 0.0f;

//Textures
Texture2D paddleTexture;
Texture2D ballTexture;

//Voltorbs 
constexpr float VOL_SIZE  = 75.0f;
constexpr float VOL_SPEED = 300.0f;
Vector2 volPos[3];
Vector2 volVel[3];
bool   volActive[3] = { true, false, false };
int    activeVolCount = 1;

//Paddles
Vector2 paddle1Pos;
Vector2 paddle2Pos;
constexpr float PADDLE_WIDTH  = 120.0f;
constexpr float PADDLE_HEIGHT = 120.0f;
constexpr float PADDLE_SPEED  = 400.0f;
bool movePaddle1Up    = false;
bool movePaddle1Down  = false;
bool movePaddle2Up    = false;
bool movePaddle2Down  = false;
bool singlePlayerMode = false;

//Functions 
void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Clone");
    SetTargetFPS(FPS);

    // Load textures
    //
    paddleTexture = LoadTexture("assets/paddle.png");

    //https://pokemondb.net/artwork/voltorb
    ballTexture   = LoadTexture("assets/voltorb.png");

    // Initial paddle positions
    paddle1Pos = { 50.0f, SCREEN_HEIGHT / 2.0f - PADDLE_HEIGHT / 2.0f };
    paddle2Pos = { SCREEN_WIDTH - PADDLE_WIDTH, SCREEN_HEIGHT / 2.0f - PADDLE_HEIGHT / 2.0f };

    // Initial voltorb positions and velocities
    for (int i = 0; i < 3; i++) {
        volPos[i] = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        volVel[i] = { VOL_SPEED, VOL_SPEED };
    }

    gPreviousTicks = GetTime();
}

void processInput() {
    //if t is pressed we switch to single player mode
    if (IsKeyPressed(KEY_T)) {singlePlayerMode = !singlePlayerMode;}

    // Player 1 input (w and s key)
    movePaddle1Up   = IsKeyDown(KEY_W);
    movePaddle1Down = IsKeyDown(KEY_S);

    // Player 2 input depending on player switch 
    if (!singlePlayerMode) {
        movePaddle2Up   = IsKeyDown(KEY_UP);
        movePaddle2Down = IsKeyDown(KEY_DOWN);
    }

    //always generate three voltorbs but take activity status
    //because voltorbs are only rendered/updated when active
    if (IsKeyPressed(KEY_ONE)) {
        activeVolCount = 1;
        volActive[0] = true; 
        volActive[1] = false;
        volActive[2] = false;
    }
    if (IsKeyPressed(KEY_TWO)) {
        activeVolCount = 2;
        volActive[0] = true; 
        volActive[1] = true; 
        volActive[2] = false;
    }
    if (IsKeyPressed(KEY_THREE)) {
        activeVolCount = 3;
        volActive[0] = true; 
        volActive[1] = true; 
        volActive[2] = true;
    }

    if (WindowShouldClose()) {gAppStatus = TERMINATED;}
}

void update() {
    float ticks = GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    //update paddle movement
    //paddle one always displays results of keyboard input
    if (movePaddle1Up)   {
        paddle1Pos.y -= PADDLE_SPEED * deltaTime;}
    else if (movePaddle1Down) {
        paddle1Pos.y += PADDLE_SPEED * deltaTime;}

    //paddle two either displays results of keyboard input or simple up-down movement
    if (!singlePlayerMode) {
        if (movePaddle2Up)   {paddle2Pos.y -= PADDLE_SPEED * deltaTime;}
        if (movePaddle2Down) {paddle2Pos.y += PADDLE_SPEED * deltaTime;}
    } else {
        for (int i = 0; i < 3; i++) {
            //only update active balls
            if (!volActive[i]) {continue;}
            if (volPos[i].y < paddle2Pos.y + PADDLE_HEIGHT / 2.0f) {
                paddle2Pos.y -= PADDLE_SPEED * deltaTime;}
            else if (volPos[i].y > paddle2Pos.y + PADDLE_HEIGHT / 2.0f) {
                paddle2Pos.y += PADDLE_SPEED * deltaTime;}
            break;
        }
    }

    // ensure that paddles stay within the window screen
    if (paddle1Pos.y < 0) {
        paddle1Pos.y = 0; }
    else if (paddle1Pos.y > SCREEN_HEIGHT - PADDLE_HEIGHT) {
        paddle1Pos.y = SCREEN_HEIGHT - PADDLE_HEIGHT;}
    if (paddle2Pos.y < 0) {
        paddle2Pos.y = 0;}
    else if (paddle2Pos.y > SCREEN_HEIGHT - PADDLE_HEIGHT) {
        paddle2Pos.y = SCREEN_HEIGHT - PADDLE_HEIGHT;}

    //update voltorb movement
    for (int i = 0; i < 3; i++) {
        if (!volActive[i]) {continue;}

        volPos[i].x += volVel[i].x * deltaTime;
        volPos[i].y += volVel[i].y * deltaTime;

        //voltorbs can't leave through top/bottom, only bounce off of them
        if (volPos[i].y < 0) {volPos[i].y = 0, volVel[i].y *= -1;}
        if (volPos[i].y > SCREEN_HEIGHT - VOL_SIZE) {volPos[i].y = SCREEN_HEIGHT - VOL_SIZE, volVel[i].y *= -1;}

        //voltorb-paddle box collision
        Rectangle paddle1Rect = { paddle1Pos.x, paddle1Pos.y, PADDLE_WIDTH, PADDLE_HEIGHT };
        Rectangle paddle2Rect = { paddle2Pos.x, paddle2Pos.y, PADDLE_WIDTH, PADDLE_HEIGHT };
        Rectangle volRect     = { volPos[i].x, volPos[i].y, VOL_SIZE, VOL_SIZE };

        if (CheckCollisionRecs(volRect, paddle1Rect) && volVel[i].x < 0) volVel[i].x *= -1;
        if (CheckCollisionRecs(volRect, paddle2Rect) && volVel[i].x > 0) volVel[i].x *= -1;

        // End game if voltorb leaves screen
        if (volPos[i].x + VOL_SIZE < 0 || volPos[i].x > SCREEN_WIDTH) {gAppStatus = TERMINATED;}
    }
}


void render() {
    BeginDrawing();
    ClearBackground(ColorFromHex("#155b5aff"));

    // render paddle texture
    Rectangle textureArea = { 0.0f, 0.0f, (float)paddleTexture.width, (float)paddleTexture.height };
    Rectangle destinationArea;
    Vector2 origin;

    // Paddle 1
    destinationArea = { paddle1Pos.x, paddle1Pos.y, PADDLE_WIDTH, PADDLE_HEIGHT };
    origin = { destinationArea.width / 2.0f, destinationArea.height / 2.0f };
    DrawTexturePro(paddleTexture, textureArea, destinationArea, origin, 0.0f, WHITE);

    // Paddle 2
    destinationArea = { paddle2Pos.x, paddle2Pos.y, PADDLE_WIDTH, PADDLE_HEIGHT };
    origin = { destinationArea.width / 2.0f, destinationArea.height / 2.0f };
    DrawTexturePro(paddleTexture, textureArea, destinationArea, origin, 0.0f, WHITE);

    // render voltorb texture
    textureArea = { 0.0f, 0.0f, (float)ballTexture.width, (float)ballTexture.height };

    //only render a generated voltorb if it is active
    for (int i = 0; i < 3; i++) {
        if (!volActive[i]) continue;
        destinationArea = { volPos[i].x, volPos[i].y, VOL_SIZE, VOL_SIZE };
        origin = { destinationArea.width / 2.0f, destinationArea.height / 2.0f };
        DrawTexturePro(ballTexture, textureArea, destinationArea, origin, 0.0f, WHITE);
    }

    EndDrawing();
}

void shutdown() {
    CloseWindow();
}

int main(void) {
    initialise();
    while (gAppStatus == RUNNING) {
        processInput();
        update();
        render();
    }
    shutdown();
    return 0;
}
