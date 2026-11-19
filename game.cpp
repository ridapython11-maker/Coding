#include "raylib.h"
#include <iostream>
#include <fstream>
using namespace std;

const int WIDTH = 900;
const int HEIGHT = 650;
const int MAX_ENEMIES = 20;
const int MAX_BULLETS = 50;

// ---------------- PLAYER STRUCTURE ----------------

struct Player
{
    Rectangle body;
    float speed;
    int lives;

    Player()
    {
        body = { 420, 530, 60, 65 };
        speed = 400;
        lives = 3;
    }

    void Move(float dt)
    {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            body.x -= speed * dt;

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            body.x += speed * dt;

        if (body.x < 10)
            body.x = 10;

        if (body.x + body.width > WIDTH - 10)
            body.x = WIDTH - body.width - 10;
    }

    void Draw()
    {
        float x = body.x + 30;
        float y = body.y + 30;

        // Neon glow
        DrawCircle((int)x, (int)y, 35, Fade(SKYBLUE, 0.12f));

        // Wings
        DrawTriangle(
            { x - 8, y + 5 },
            { x - 30, y + 27 },
            { x - 25, y - 5 },
            BLUE
        );

        DrawTriangle(
            { x + 8, y + 5 },
            { x + 30, y + 27 },
            { x + 25, y - 5 },
            BLUE
        );

        // Main spaceship body
        DrawTriangle(
            { x, y - 32 },
            { x - 19, y + 25 },
            { x + 19, y + 25 },
            SKYBLUE
        );

        DrawTriangle(
            { x, y - 22 },
            { x - 7, y + 12 },
            { x + 7, y + 12 },
            WHITE
        );

        // Cockpit
        DrawCircle((int)x, (int)(y - 5), 6, DARKBLUE);

        // Engine flame
        DrawTriangle(
            { x - 8, y + 20 },
            { x, y + 43 + (float)GetRandomValue(0, 5) },
            { x + 8, y + 20 },
            ORANGE
        );

        DrawCircle((int)x, (int)y, 3, WHITE);
    }
};

// ---------------- BULLET STRUCTURE ----------------

struct Bullet
{
    Rectangle body;
    bool active;

    Bullet()
    {
        body = { 0, 0, 8, 22 };
        active = false;
    }

    void Update(float dt)
    {
        if (active)
        {
            body.y -= 550 * dt;

            if (body.y < -30)
                active = false;
        }
    }

    void Draw()
    {
        if (active)
        {
            DrawCircle(
                (int)(body.x + 4),
                (int)(body.y + 10),
                10,
                Fade(SKYBLUE, 0.2f)
            );

            DrawRectangleRounded(body, 0.8f, 8, SKYBLUE);

            DrawCircle(
                (int)(body.x + 4),
                (int)(body.y + 5),
                3,
                WHITE
            );
        }
    }
};

// ---------------- ENEMY STRUCTURE ----------------

struct Enemy
{
    Rectangle body;
    bool active;

    Enemy()
    {
        body = { 0, 0, 48, 42 };
        active = false;
    }

    void Update(float dt, float speed)
    {
        if (active)
            body.y += speed * dt;
    }

    void Draw()
    {
        if (active)
        {
            float x = body.x + 24;
            float y = body.y + 20;

            DrawCircle(
                (int)x, (int)y, 29,
                Fade(RED, 0.15f)
            );

            // Enemy wings
            DrawTriangle(
                { x - 8, y },
                { x - 29, y + 18 },
                { x - 23, y - 14 },
                MAROON
            );

            DrawTriangle(
                { x + 8, y },
                { x + 29, y + 18 },
                { x + 23, y - 14 },
                MAROON
            );

            // Main body
            DrawTriangle(
                { x, y + 25 },
                { x - 19, y - 18 },
                { x + 19, y - 18 },
                RED
            );

            // Cockpit
            DrawCircle((int)x, (int)(y - 3), 6, ORANGE);
            DrawCircle((int)x, (int)(y - 3), 3, WHITE);
        }
    }
};

// ---------------- FILE HANDLING ----------------

int LoadHighScore()
{
    int highScore = 0;

    ifstream file("highscore.txt");

    if (file.is_open())
    {
        file >> highScore;
        file.close();
    }

    return highScore;
}

void SaveHighScore(int score)
{
    ofstream file("highscore.txt");

    if (file.is_open())
    {
        file << score;
        file.close();
    }
}

// ---------------- START LEVEL FUNCTION ----------------

void StartLevel(
    Enemy enemies[],
    int level,
    int& spawned,
    int& total
)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
        enemies[i].active = false;

    spawned = 0;
    total = 5 + level * 2;
}

// ---------------- MAIN FUNCTION ----------------

int main()
{
    InitWindow(WIDTH, HEIGHT, "NEON SPACE SHOOTER");
    SetTargetFPS(60);

    Player player;
    Player* ptr = &player;

    Bullet bullets[MAX_BULLETS];
    Enemy enemies[MAX_ENEMIES];

    int score = 0;
    int highScore = LoadHighScore();
    int level = 1;

    int spawned = 0;
    int total = 0;

    float shootTimer = 0;
    float spawnTimer = 0;
    float pickupTimer = 0;

    Rectangle pickup = { 0, -50, 25, 25 };
    bool pickupActive = false;

    int gameState = 0;
    // 0 = playing, 1 = game over, 2 = won

    // Star positions
    int starX[100];
    int starY[100];

    for (int i = 0; i < 100; i++)
    {
        starX[i] = GetRandomValue(0, WIDTH);
        starY[i] = GetRandomValue(0, HEIGHT);
    }

    StartLevel(enemies, level, spawned, total);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // ---------------- GAMEPLAY ----------------

        if (gameState == 0)
        {
            ptr->Move(dt);

            // Automatic shooting
            shootTimer += dt;

            if (shootTimer >= 0.28f)
            {
                for (int i = 0; i < MAX_BULLETS; i++)
                {
                    if (!bullets[i].active)
                    {
                        bullets[i].body.x =
                            player.body.x + 26;

                        bullets[i].body.y =
                            player.body.y - 10;

                        bullets[i].active = true;
                        shootTimer = 0;
                        break;
                    }
                }
            }

            // Enemy spawning
            spawnTimer += dt;

            float delay = 1.3f - level * 0.05f;

            if (spawned < total && spawnTimer >= delay)
            {
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    if (!enemies[i].active)
                    {
                        enemies[i].body.x =
                            GetRandomValue(20, WIDTH - 70);

                        enemies[i].body.y = -50;
                        enemies[i].active = true;

                        spawned++;
                        spawnTimer = 0;
                        break;
                    }
                }
            }

            // Update bullets
            for (int i = 0; i < MAX_BULLETS; i++)
                bullets[i].Update(dt);

            // Update enemies
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active)
                {
                    enemies[i].Update(dt, 75 + level * 12);

                    if (CheckCollisionRecs(
                        enemies[i].body, player.body)
                        || enemies[i].body.y > HEIGHT)
                    {
                        enemies[i].active = false;
                        player.lives--;

                        if (player.lives <= 0)
                        {
                            gameState = 1;
                            break;
                        }
                    }
                }
            }

            // Bullet and enemy collision
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bullets[i].active)
                {
                    for (int j = 0; j < MAX_ENEMIES; j++)
                    {
                        if (enemies[j].active &&
                            CheckCollisionRecs(
                                bullets[i].body,
                                enemies[j].body))
                        {
                            bullets[i].active = false;
                            enemies[j].active = false;

                            score += 10;
                            break;
                        }
                    }
                }
            }

            // Extra life pickup
            pickupTimer += dt;

            if (!pickupActive && pickupTimer >= 12)
            {
                pickup.x = GetRandomValue(30, WIDTH - 60);
                pickup.y = -30;
                pickupActive = true;
                pickupTimer = 0;
            }

            if (pickupActive)
            {
                pickup.y += 140 * dt;

                if (CheckCollisionRecs(player.body, pickup))
                {
                    if (player.lives < 3)
                        player.lives++;

                    pickupActive = false;
                }

                if (pickup.y > HEIGHT)
                    pickupActive = false;
            }

            // Check level completion
            int activeEnemies = 0;

            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active)
                    activeEnemies++;
            }

            if (spawned == total &&
                activeEnemies == 0 &&
                gameState == 0)
            {
                level++;

                if (level > 10)
                    gameState = 2;
                else
                    StartLevel(enemies, level, spawned, total);
            }
        }

        // Save high score
        if (gameState != 0 && score > highScore)
        {
            highScore = score;
            SaveHighScore(highScore);
        }

        // Restart game
        if (gameState != 0 && IsKeyPressed(KEY_R))
        {
            score = 0;
            level = 1;
            player.lives = 3;
            player.body.x = 420;

            pickupActive = false;
            pickupTimer = 0;
            shootTimer = 0;
            spawnTimer = 0;

            for (int i = 0; i < MAX_BULLETS; i++)
                bullets[i].active = false;

            StartLevel(enemies, level, spawned, total);
            gameState = 0;
        }

        // ---------------- DRAWING ----------------

        BeginDrawing();

        ClearBackground(Color{ 3, 5, 25, 255 });

        // Moving starfield
        for (int i = 0; i < 100; i++)
        {
            DrawCircle(starX[i], starY[i], 1.5f, SKYBLUE);

            starY[i] += 1 + level / 3;

            if (starY[i] > HEIGHT)
            {
                starY[i] = 0;
                starX[i] = GetRandomValue(0, WIDTH);
            }
        }

        // Background neon decoration
        DrawCircleLines(100, 300, 120, Fade(BLUE, 0.15f));
        DrawCircleLines(800, 400, 160, Fade(PURPLE, 0.15f));

        // HUD
        DrawRectangleRounded(
            { 15, 12, 180, 43 }, 0.3f, 8,
            Fade(DARKBLUE, 0.8f)
        );

        DrawRectangleRounded(
            { 220, 12, 210, 43 }, 0.3f, 8,
            Fade(DARKBLUE, 0.8f)
        );

        DrawRectangleRounded(
            { 690, 12, 195, 43 }, 0.3f, 8,
            Fade(DARKBLUE, 0.8f)
        );

        DrawText(TextFormat("LIVES: %i", player.lives),
            28, 22, 22, SKYBLUE);

        DrawText(TextFormat("SCORE: %i", score),
            235, 22, 22, WHITE);

        DrawText(TextFormat("LEVEL: %i / 10", level),
            705, 22, 20, SKYBLUE);

        DrawText(TextFormat("HIGH SCORE: %i", highScore),
            20, 65, 18, GOLD);

        // Draw game objects
        if (gameState == 0)
            ptr->Draw();

        for (int i = 0; i < MAX_BULLETS; i++)
            bullets[i].Draw();

        for (int i = 0; i < MAX_ENEMIES; i++)
            enemies[i].Draw();

        // Draw extra life
        if (pickupActive)
        {
            DrawCircle(
                (int)(pickup.x + 12),
                (int)(pickup.y + 12),
                19, Fade(GREEN, 0.25f)
            );

            DrawCircle(
                (int)(pickup.x + 12),
                (int)(pickup.y + 12),
                12, GREEN
            );

            DrawText("+",
                (int)pickup.x + 5,
                (int)pickup.y - 1,
                25, WHITE
            );
        }

        DrawText("MOVE: A / D OR ARROW KEYS",
            20, HEIGHT - 30, 16, RAYWHITE);

        DrawText("AUTO SHOOTING ENABLED",
            620, HEIGHT - 30, 16, SKYBLUE);

        // Game over / winning screen
        if (gameState != 0)
        {
            DrawRectangle(
                0, 0, WIDTH, HEIGHT,
                Fade(BLACK, 0.8f)
            );

            if (gameState == 1)
            {
                DrawText("GAME OVER",
                    280, 220, 55, RED);
            }
            else
            {
                DrawText("YOU WIN!",
                    320, 220, 55, GREEN);
            }

            DrawText(TextFormat("FINAL SCORE: %i", score),
                325, 310, 28, WHITE);

            DrawText(TextFormat("HIGH SCORE: %i", highScore),
                345, 355, 22, GOLD);

            DrawText("PRESS R TO RESTART",
                300, 425, 23, SKYBLUE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
