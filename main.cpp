#include "CS3113/cs3113.h"
#include <array>
#include <string>
#include <math.h>

/**

* Author: Alaya Chowdhury

* Assignment: simple 2d scene

* Date due: 2/14/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

// Constants
constexpr int   SCREEN_WIDTH  = 1600 / 2;
constexpr int   SCREEN_HEIGHT = 1000 / 2;
constexpr int   FPS           = 60;
constexpr float BASE_SIZE     = 100.0f;

// Assets
constexpr char TEXTURE1[] = "assets/pyramid.jpeg";
constexpr char TEXTURE2[] = "assets/program.jpeg";
constexpr char TEXTURE3[] = "assets/sewing.jpeg";

// Game state
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;
float gTime = 0.0f;
bool gIsYellow = true;

// Object structure
struct GameObject {
    Texture2D texture;
    Vector2 position;
    Vector2 size;
    float rotation;
    float scale;
    float timeOffset;

    void Update(float t, GameObject* ref2 = nullptr) {
        // object one moves in a regular sine movement
        if (timeOffset == 0.0f) {
            float height = 70; // height of wave
            float speed = 2; // speed of each cycle
            position.x += 100 * GetFrameTime();
            position.y = 300 + height * sin(t * speed);
            if (position.x > SCREEN_WIDTH) {position.x = 0; }
        }

        // object two rotates, moves diagonally, and is the relative object that object three orbits around
        if (timeOffset == 1.0f) {
            rotation += 60 * GetFrameTime();
            position.x += 100 *GetFrameTime();
            position.y += 100 *GetFrameTime();
            if (position.x > SCREEN_WIDTH || position.y > SCREEN_HEIGHT) {
                position.x = 50; //starts at top left corner 
                position.y = 50;
                gIsYellow = !gIsYellow;
            }
        }

        // object three pulses and orbits and rotates around object twos relative position
        if (timeOffset == 2.0f && ref2 != nullptr) {
            // pulsing
            float pulseSpeed = 2.5f;
            float maxAmplitude = 0.3f;
            scale = 1.0f + maxAmplitude * sin(t * pulseSpeed);
            // orbiting 
            float radius = 100.0f;
            float orbitSpeed = 1.5f;
            position.x = ref2->position.x + radius * cos(t * orbitSpeed);
            position.y = ref2->position.y + radius * sin(t * orbitSpeed);
        }
    }

    void Draw() {

        Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };

        Rectangle dest = {
            position.x,
            position.y,
            size.x * scale,
            size.y * scale
        };

        Vector2 origin = {
            (size.x * scale) / 2.0f,
            (size.y * scale) / 2.0f
        };

        DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
    }

    void Unload() {
        UnloadTexture(texture);
    }
};

// Declare objects
GameObject obj1, obj2, obj3;

void initialise() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib - 3 Object Transformations");
    SetTargetFPS(FPS);

    obj1.texture = LoadTexture(TEXTURE1);
    obj2.texture = LoadTexture(TEXTURE2);
    obj3.texture = LoadTexture(TEXTURE3);

    obj1.position = {0.0f, 300.0f};
    obj2.position = {50.0f, 50.0f};   // top-left
    obj3.position = {400.0f, 300.0f};

    obj1.size = {BASE_SIZE, BASE_SIZE};
    obj2.size = {BASE_SIZE, BASE_SIZE};
    obj3.size = {BASE_SIZE, BASE_SIZE};

    obj1.scale = 1.0f;
    obj2.scale = 1.0f;
    obj3.scale = 1.0f;

    obj1.rotation = 0.0f;
    obj2.rotation = 0.0f;
    obj3.rotation = 0.0f;

    obj1.timeOffset = 0.0f;
    obj2.timeOffset = 1.0f;
    obj3.timeOffset = 2.0f;

    gPreviousTicks = GetTime();
}

void processInput() {

    if (WindowShouldClose()) {
        gAppStatus = TERMINATED;
    }
}

void update() {

    float ticks = GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    gTime += deltaTime;

    obj1.Update(gTime);
    obj2.Update(gTime);
    obj3.Update(gTime, &obj2);
}

void render() {

    BeginDrawing();
    ClearBackground(BLACK);

    if (gIsYellow) {
        ClearBackground(YELLOW);
    } else {
        ClearBackground(BLUE);
    }

    obj1.Draw();
    obj2.Draw();
    obj3.Draw();

    EndDrawing();
}

void shutdown() {

    obj1.Unload();
    obj2.Unload();
    obj3.Unload();
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
