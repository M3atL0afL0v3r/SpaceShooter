#include "game.h"
#include "graphics.h"
#include <cstdlib>

SDL_Color white = { 255, 255, 255, 255 };
SDL_Color highlightColor = { 255, 255, 0, 255 };
SDL_Color red = { 255, 0, 0, 255 };

void renderStartMenu(SDL_Renderer* renderer, int menuSelection)
{
    // Clear background to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render menu title
    int titleX = currentWidth / 2 - 150;
    int titleY = currentHeight / 3;
    drawText(renderer, "SPACE SHOOTER", titleX, titleY, white);

    int playX = currentWidth / 2 - 100;
    int playY = titleY + 50;
    std::string playText = (menuSelection == 0) ? "> Play" : "  Play";
    drawText(renderer, playText.c_str(), playX, playY, white);

    int settingsX = currentWidth / 2 - 100;
    int settingsY = playY + 50;
    std::string settingsText = (menuSelection == 1) ? "> Settings" : "  Settings";
    drawText(renderer, settingsText.c_str(), settingsX, settingsY, white);

    int exitX = currentWidth / 2 - 150;
    int exitY = settingsY + 50;
    std::string exitText = (menuSelection == 2) ? "> Exit" : "  Exit";
    drawText(renderer, exitText.c_str(), exitX, exitY, white);
}

void renderSettingsMenu(SDL_Renderer* renderer, int settingsSelection, bool isFullscreen, Difficulty currentDifficulty) 
{
    // Clear background to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int settingsX = currentWidth / 2 - 150;
    int settingsY = currentHeight / 3;
    drawText(renderer, "SETTINGS", settingsX, settingsY, white);

    int windowModeX = currentWidth / 2 - 100;
    int windowModeY = settingsY + 50;
    std::string fullscreenOption = isFullscreen ? "Windowed Mode" : "Fullscreen Mode";
    drawText(renderer, settingsSelection == 0 ? ("> " + fullscreenOption).c_str() : ("  " + fullscreenOption).c_str(), windowModeX, windowModeY, white);

    std::string difficultyString;
    switch (currentDifficulty)
    {
    case Easy:
        difficultyString = "Difficulty: Easy";
        break;
    case Medium:
        difficultyString = "Difficulty: Medium";
        break;
    case Hard:
        difficultyString = "Difficulty: Hard";
        break;
    }

    int difficultyX = currentWidth / 2 - 100;
    int difficultyY = windowModeY + 50;
    drawText(renderer, settingsSelection == 1 ? ("> " + difficultyString).c_str() : ("  " + difficultyString).c_str(), difficultyX, difficultyY, white);

    int backX = currentWidth / 2 - 150;
    int backY = difficultyY + 50;
    drawText(renderer, settingsSelection == 2 ? "> Back" : "  Back", backX, backY, white);

    SDL_RenderPresent(renderer);
}

void renderGameOverMenu(SDL_Renderer* renderer, int score, int highScore)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int gameOverX = currentWidth / 2 - 150;
    int gameOverY = currentHeight / 3;

    drawText(renderer, "GAME OVER", gameOverX, gameOverY, red);

    std::string scoreText = "Score: " + std::to_string(score);
    int scoreX = currentWidth / 2 - 100;
    int scoreY = gameOverY + 60;

    drawText(renderer, scoreText.c_str(), scoreX, scoreY, red);

    std::string highScoreText = "High Score: " + std::to_string(highScore);
    int highScoreX = currentWidth / 2 - 100;
    int highScoreY = scoreY + 60;

    drawText(renderer, highScoreText.c_str(), highScoreX, highScoreY, red);

    int againX = currentWidth / 2 - 150;
    int againY = highScoreY + 60;

    drawText(renderer, "Play Again? (ENTER)", againX, againY, red);
}

void initStars(std::vector<Star>& stars, int count, int windowW, int windowH) 
{
    for (int i = 0; i < count; ++i) {
        stars.push_back({
            static_cast<float>(rand() % windowW),
            static_cast<float>(rand() % windowH),
            0.5f + static_cast<float>(rand() % 100) / 100.0f
            });
    }
}

void updateStars(std::vector<Star>& stars, int windowH) 
{
    for (auto& star : stars) {
        star.y += star.speed;
        if (star.y >= windowH) {
            star.y = 0;
            star.x = static_cast<float>(rand() % currentWidth);
        }
    }
}

void renderStars(SDL_Renderer* renderer, const std::vector<Star>& stars) 
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (const auto& star : stars) {
        SDL_RenderPoint(renderer, static_cast<int>(star.x), static_cast<int>(star.y));
    }
}

void renderPlayerLives(SDL_Renderer* renderer, SDL_Texture* lifeTexture, int lives)
{
    const int LIFE_WIDTH = 32;
    const int LIFE_HEIGHT = 32;
    const int PADDING = 10;

    for (int i = 0; i < lives; ++i) {
        SDL_FRect dest;
        dest.x = PADDING + i * (LIFE_WIDTH + PADDING);
        dest.y = PADDING;
        dest.w = LIFE_WIDTH;
        dest.h = LIFE_HEIGHT;

        SDL_RenderTexture(renderer, lifeTexture, nullptr, &dest);
    }
}


void initPlayer(Player& player, SDL_Texture* texture)
{
    float w = 64, h = 64;

    player.x = currentWidth / 2.0f - w / 2.0f;
    player.y = currentHeight - h - currentHeight/10;
    player.width = w;
    player.height = h;
    player.speed = 5;
    player.texture = texture;
}

void handlePlayerMovement(Player& player, const bool* keystates, int windowWidth)
{
    if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A])
    {
        player.x -= player.speed;
        if (player.x < 0) player.x = 0;
    }
    if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D])
    {
        player.x += player.speed;
        if (player.x > windowWidth - player.width) player.x = windowWidth - player.width;
    }
}

void renderPlayer(SDL_Renderer* renderer, const Player& player)
{
    SDL_FRect dest = { player.x, player.y, player.width, player.height };
    SDL_RenderTexture(renderer, player.texture, nullptr, &dest);
}

void spawnEnemy(std::vector<Enemy>& enemies, int screenWidth, Difficulty currentDifficulty) {
    Enemy e;
    e.rect.w = 40;
    e.rect.h = 40;
    e.rect.x = rand() % (screenWidth - (int)e.rect.w);
    e.rect.y = -e.rect.h;

    switch (currentDifficulty)
    {
    case Easy:
        e.speed = 25.0f; // pixels per second
        e.lastShotTime = SDL_GetTicks();
        e.shootCooldown = 4000 + rand() % 4000; // shoot every 4–8s
        break;
    case Medium:
        e.speed = 50.0f; // pixels per second
        e.lastShotTime = SDL_GetTicks();
        e.shootCooldown = 2000 + rand() % 2000; // shoot every 2–4s
        break;
    case Hard:
        e.speed = 75.0f; // pixels per second
        e.lastShotTime = SDL_GetTicks();
        e.shootCooldown = 1000 + rand() % 1000; // shoot every 1–2s
        break;
    }
    enemies.push_back(e);
}

void updateEnemies(std::vector<Enemy>& enemies, std::vector<Bullet>& bullets, float deltaTime) {
    Uint32 now = SDL_GetTicks();

    for (auto& e : enemies) {
        e.rect.y += e.speed * deltaTime;

        if (now - e.lastShotTime >= e.shootCooldown) {
            Bullet b;
            b.x = e.rect.x + e.rect.w / 2 - 8.0f / 2;
            b.y = e.rect.y + e.rect.h;
            b.speed = 4.0f;
            b.fromEnemy = true;
            bullets.push_back(b);

            e.lastShotTime = now;
        }
    }
}

void renderEnemies(SDL_Renderer* renderer, const std::vector<Enemy>& enemies, SDL_Texture* enemyTexture) {
    for (const auto& e : enemies) {
        SDL_RenderTexture(renderer, enemyTexture, nullptr, &e.rect);
    }
}


void handleShooting(std::vector<Bullet>& bullets, const Player& player, Uint64& lastShotTime, const bool* keystates)
{
    Uint64 currentTime = SDL_GetTicks();
    const int shootDelay = 250;

    if (keystates[SDL_SCANCODE_SPACE] && (currentTime - lastShotTime > shootDelay)) {
        Bullet newBullet;
        newBullet.x = player.x + player.width / 2.0f - 2;
        newBullet.y = player.y;
        newBullet.speed = 7.0f;
        newBullet.fromEnemy = false;
        newBullet.frameCounter = 0;
        bullets.push_back(newBullet);
        lastShotTime = currentTime;
    }
}

void updateBullets(std::vector<Bullet>& bullets)
{
    for (auto& bullet : bullets) {
        bullet.y += (bullet.fromEnemy ? bullet.speed : -bullet.speed);
        bullet.frameCounter++;
    }

    // Remove bullets off-screen
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return b.y < -20 || b.y > currentHeight + 20; }), bullets.end());

}

void renderBullets(SDL_Renderer* renderer, const std::vector<Bullet>& bullets, SDL_Texture* startTex, SDL_Texture* fireTex, SDL_Texture* loopTex)
{
    {
        for (const auto& bullet : bullets) {
            SDL_Texture* currentTex = nullptr;
            if (bullet.frameCounter == 0 && startTex)
                currentTex = startTex;
            else if (bullet.frameCounter > 0 && bullet.frameCounter < 5 && fireTex)
                currentTex = fireTex;
            else
                currentTex = loopTex;

            SDL_FRect dest = { bullet.x, bullet.y, 8.0f, 20.0f };
            if (currentTex)
            {
                float angle = bullet.fromEnemy ? 180.0f : 0.0f;
                SDL_RenderTextureRotated(renderer, currentTex, nullptr, &dest, angle, nullptr, SDL_FLIP_NONE);
            }
            else 
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &dest);
            }
        }
    }
}

bool checkCollision(float x1, float y1, float w1, float h1,
    float x2, float y2, float w2, float h2) {
    return !(x1 + w1 < x2 || x2 + w2 < x1 ||
        y1 + h1 < y2 || y2 + h2 < y1);
}