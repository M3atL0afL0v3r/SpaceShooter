#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <vector>
#include <string>
#include <functional>

extern int currentWidth, currentHeight;
const int FRAME_DELAY = 16;

enum GameState {
    START_MENU,
    SETTINGS_MENU,
    PLAYING,
    GAME_OVER
};

enum Difficulty
{
    Easy,
    Medium,
    Hard
};

struct Star {
    float x, y;
    float speed;
};

struct Player {
    float x, y;
    float width, height;
    int speed;
    SDL_Texture* texture;
};

struct Enemy {
    SDL_FRect rect;
    float speed;
    Uint64 lastShotTime;
    Uint64 shootCooldown;
};

struct Bullet {
    float x, y;
    float speed;
    bool fromEnemy = false;
    SDL_Texture* texture = nullptr;
    int frameCounter = 0;
};

void renderStartMenu(SDL_Renderer* renderer, int menuSelection);
void renderSettingsMenu(SDL_Renderer* renderer, int settingsSelection, bool isFullscreen, Difficulty currentDifficulty);
void renderGameOverMenu(SDL_Renderer* renderer, int score, int highScore);

void initStars(std::vector<Star>& stars, int count, int windowW, int windowH);
void updateStars(std::vector<Star>& stars, int windowH);
void renderStars(SDL_Renderer* renderer, const std::vector<Star>& stars);

void renderPlayerLives(SDL_Renderer* renderer, SDL_Texture* lifeTexture, int lives);

void initPlayer(Player& player, SDL_Texture* texture);
void handlePlayerMovement(Player& player, const bool* keystates, int windowWidth);
void renderPlayer(SDL_Renderer* renderer, const Player& player);

void spawnEnemy(std::vector<Enemy>& enemies, int screenWidth, Difficulty currentDifficulty);
void updateEnemies(std::vector<Enemy>& enemies, std::vector<Bullet>& bullets, float deltaTime);
void renderEnemies(SDL_Renderer* renderer, const std::vector<Enemy>& enemies, SDL_Texture* enemyTexture);


void handleShooting(std::vector<Bullet>& bullets, const Player& player, Uint64& lastShotTime, const bool* keystates);
void updateBullets(std::vector<Bullet>& bullets);
void renderBullets(SDL_Renderer* renderer, const std::vector<Bullet>& bullets, SDL_Texture* startTex, SDL_Texture* fireTex, SDL_Texture* loopTex);

bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);


#endif