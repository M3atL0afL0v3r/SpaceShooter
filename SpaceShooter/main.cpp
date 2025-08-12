#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include "game.h"
#include "graphics.h"

int highScore = 0;
int score = 0;

Difficulty currentDifficulty = Easy;

int currentWidth = 1920;
int currentHeight = 1080;

bool fullscreen = true;

void gameLoop(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* playerTexture, SDL_Texture* enemyTexture, SDL_Texture* bulletStartTex, SDL_Texture* bulletFireTex, SDL_Texture* bulletLoopTex, SDL_Texture* lifeTexture, bool& highScoreUpdated);

int main(int argc, char* argv[])
{

    bool highScoreUpdated = false;

    //Access file where high score is stored to load previous high score.
    std::ifstream inFile("highScore.txt");
    if (!inFile.is_open())
    {
        std::cout << "Error: could not open high score text file!\n";
        return 1;
    }
    else std::cout << "High score text file opened!";

    inFile >> highScore;

    inFile.close();

    //Innitialize Simple DirectMedia Layer 3.0 for imput and video output.
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    //Initialize SDL TTF for font and menu design.
    if (!TTF_Init()) {
        std::cout << "TTF_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SpaceShooter", currentWidth, currentHeight, SDL_WINDOW_FULLSCREEN);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GetWindowSize(window, &currentWidth, &currentHeight);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderLogicalPresentation(renderer, 1920, 1080, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    //Create SDL textures from the asset .BMPs.
    SDL_Surface* playerSurface = SDL_LoadBMP("Assets/player_ship.bmp");
    SDL_Texture* playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_DestroySurface(playerSurface);

    SDL_Surface* enemySurface = SDL_LoadBMP("Assets/enemy.bmp");
    SDL_Texture* enemyTexture = SDL_CreateTextureFromSurface(renderer, enemySurface);
    SDL_DestroySurface(enemySurface);

    SDL_Surface* lifeSurface = SDL_LoadBMP("Assets/player_ship.bmp");
    SDL_Texture* lifeTexture = SDL_CreateTextureFromSurface(renderer, lifeSurface);
    SDL_DestroySurface(lifeSurface);

    SDL_Surface* surfaceBulletStart = SDL_LoadBMP("Assets/bullet_start.bmp");
    SDL_Surface* surfaceBulletFire = SDL_LoadBMP("Assets/bullet_fire.bmp");
    SDL_Surface* surfaceBulletLoop = SDL_LoadBMP("Assets/bullet_loop.bmp");

    SDL_Texture* bulletStartTex = SDL_CreateTextureFromSurface(renderer, surfaceBulletStart);
    SDL_Texture* bulletFireTex = SDL_CreateTextureFromSurface(renderer, surfaceBulletFire);
    SDL_Texture* bulletLoopTex = SDL_CreateTextureFromSurface(renderer, surfaceBulletLoop);

    SDL_DestroySurface(surfaceBulletStart);
    SDL_DestroySurface(surfaceBulletFire);
    SDL_DestroySurface(surfaceBulletLoop);

    gameLoop(window, renderer, playerTexture, enemyTexture, bulletStartTex, bulletFireTex, bulletLoopTex, lifeTexture, highScoreUpdated);

    if (highScoreUpdated)
    {
        std::ofstream outFile("highScore.txt");
        if (outFile.is_open())
        {
            outFile << highScore;
            outFile.close();
            std::cout << "High score saved successfully!\n";
        }
        else std::cout << "Failed to open highScore.txt for saving.\n";
    }

    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(lifeTexture);
    if (enemyTexture) SDL_DestroyTexture(enemyTexture);
    if (bulletStartTex) SDL_DestroyTexture(bulletStartTex);
    if (bulletFireTex) SDL_DestroyTexture(bulletFireTex);
    if (bulletLoopTex) SDL_DestroyTexture(bulletLoopTex);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

void gameLoop(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* playerTexture, SDL_Texture* enemyTexture, SDL_Texture* bulletStartTex, SDL_Texture* bulletFireTex, SDL_Texture* bulletLoopTex, SDL_Texture* lifeTexture, bool& highScoreUpdated)
{
    Player player_ship;
    initPlayer(player_ship, playerTexture);

    int playerLives = 3;

    //Enemy vector and spawn delay to track spawn timing.
    std::vector<Enemy> enemies;
    Uint32 lastEnemySpawn = 0;
    const Uint32 enemySpawnDelay = 1500;

    //Star vector and random star generation.
    std::vector<Star> stars;
    srand(static_cast<unsigned int>(time(nullptr)));
    initStars(stars, 100, currentWidth, currentHeight);

    //Bullet vector to track enemy shot timing.
    std::vector<Bullet> bullets;
    Uint64 lastShotTime = 0;

    //Variables to track menu navigation.
    bool prevUp = false;
    bool prevDown = false;
    bool prevSpace = false;

    bool running = true;
    SDL_Event event;

    GameState gameState = START_MENU;

    int startMenuSelection = 0;
    int settingsMenuSelection = 0;


    while (running)
    {
        Uint64 frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        const bool* keystates = SDL_GetKeyboardState(nullptr);


        switch (gameState)
        {
        case START_MENU:
        {
            bool currentUp = keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W];
            bool currentDown = keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S];
            bool currentSpace = keystates[SDL_SCANCODE_RETURN] || keystates[SDL_SCANCODE_SPACE];

            const int MENU_COUNT = 3; //Play, Settings, Exit

            renderStartMenu(renderer, startMenuSelection);

            if (currentUp && !prevUp) startMenuSelection = (startMenuSelection - 1 + MENU_COUNT) % MENU_COUNT;

            if (currentDown && !prevDown) startMenuSelection = (startMenuSelection + 1) % MENU_COUNT;

            if (currentSpace && !prevSpace)
            {
                if (startMenuSelection == 0) gameState = PLAYING;

                else if (startMenuSelection == 1) gameState = SETTINGS_MENU;

                else if (startMenuSelection == 2) running = false;

            }

            prevUp = currentUp;
            prevDown = currentDown;
            prevSpace = currentSpace;

            break;
        }

        case SETTINGS_MENU:
        {
            bool currentUp = keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W];
            bool currentDown = keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S];
            bool currentSpace = keystates[SDL_SCANCODE_RETURN] || keystates[SDL_SCANCODE_SPACE];
            bool currentEscape = keystates[SDL_SCANCODE_ESCAPE];

            const int SETTINGS_MENU_COUNT = 3; //Fullscreen, Difficulty, Back

            renderSettingsMenu(renderer, settingsMenuSelection, fullscreen, currentDifficulty);

            if (currentUp && !prevUp) settingsMenuSelection = (settingsMenuSelection - 1 + SETTINGS_MENU_COUNT) % SETTINGS_MENU_COUNT;

            if (currentDown && !prevDown) settingsMenuSelection = (settingsMenuSelection + 1) % SETTINGS_MENU_COUNT;

            if (currentSpace && !prevSpace)
            {
                if (settingsMenuSelection == 0)
                {
                    // Toggle fullscreen
                    fullscreen = !fullscreen;
                    if (fullscreen) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                    else
                    {
                        SDL_SetWindowFullscreen(window, 0);
                        SDL_SetWindowSize(window, 800, 600);
                    }
                }
                else if (settingsMenuSelection == 1) currentDifficulty = static_cast<Difficulty>((currentDifficulty + 1) % 3);

                else if (settingsMenuSelection == 2) gameState = START_MENU;
            }

            if (currentEscape) gameState = START_MENU;

            prevUp = currentUp;
            prevDown = currentDown;
            prevSpace = currentSpace;

            break;
        }

        case PLAYING:
        {

            if (keystates[SDL_SCANCODE_ESCAPE]) running = false;

            handlePlayerMovement(player_ship, keystates, currentWidth);
            handleShooting(bullets, player_ship, lastShotTime, keystates);

            updateStars(stars, currentHeight);
            updateBullets(bullets);

            for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); )
            {
                if (!bulletIt->fromEnemy)
                {
                    bool bulletRemoved = false;

                    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end(); )
                    {
                        // collision check
                        if (checkCollision(enemyIt->rect.x, enemyIt->rect.y, enemyIt->rect.w, enemyIt->rect.h,
                            bulletIt->x, bulletIt->y, 8.0f, 20.0f))
                        {
                            enemyIt = enemies.erase(enemyIt);
                            bulletIt = bullets.erase(bulletIt);
                            bulletRemoved = true;
                            score++;
                            if (score > highScore)
                            {
                                highScore = score;
                                highScoreUpdated = true;
                            }
                            break; // stop checking enemies for this bullet
                        }
                        else
                        {
                            ++enemyIt;
                        }
                    }

                    if (!bulletRemoved)
                    {
                        ++bulletIt;  // only increment if bullet was not erased
                    }
                }
                else
                {
                    if (checkCollision(player_ship.x, player_ship.y, player_ship.width, player_ship.height,
                        bulletIt->x, bulletIt->y, 8.0f, 20.0f))
                    {
                        bulletIt = bullets.erase(bulletIt);
                        playerLives--;
                        if (playerLives <= 0) gameState = GAME_OVER;
                        continue;
                    }
                    else
                    {
                        ++bulletIt;
                    }
                }
            }


            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            int currentWidth, currentHeight;
            SDL_GetWindowSize(window, &currentWidth, &currentHeight);

            renderStars(renderer, stars);
            renderBullets(renderer, bullets, bulletStartTex, bulletFireTex, bulletLoopTex);
            renderPlayerLives(renderer, lifeTexture, playerLives);
            renderPlayer(renderer, player_ship);

            Uint32 now = SDL_GetTicks();
            if (now - lastEnemySpawn >= enemySpawnDelay) {
                spawnEnemy(enemies, currentWidth, currentDifficulty);
                lastEnemySpawn = now;
            }

            float deltaTime = FRAME_DELAY / 1000.0f;
            updateEnemies(enemies, bullets, deltaTime);

            renderEnemies(renderer, enemies, enemyTexture);
            break;
        }


        case GAME_OVER:
        {
            renderGameOverMenu(renderer, score, highScore);
            if (keystates[SDL_SCANCODE_ESCAPE])
                running = false;
            else if (keystates[SDL_SCANCODE_RETURN])
            {
                // Reset game state
                playerLives = 3;
                bullets.clear();
                enemies.clear();
                initPlayer(player_ship, playerTexture);
                score = 0;
                gameState = PLAYING;
            }
            break;
        }
        }

        SDL_RenderPresent(renderer);

        Uint64 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY)
            SDL_Delay(static_cast<Uint32>(FRAME_DELAY - frameTime));
    }
}