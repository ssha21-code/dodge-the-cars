#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <raylib.h>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

constexpr int WINDOW_WIDTH = 1200;
constexpr int WINDOW_HEIGHT = 800;

Texture2D textureRoads;
Texture2D textureCarRed;
Texture2D textureCarOrange;
Texture2D textureCarYellow;
Texture2D textureCarGreen ;
Texture2D textureCarBlue;
Texture2D textureCarIndigo;
Texture2D textureCarViolet;
Texture2D textureCarPink;
Texture2D textureCarRainbow;
Texture2D textureFuel;
Texture2D textureTitleScreen;
Texture2D texturePause;
Texture2D texturePlay;

Font font;

Sound soundCollect;
Sound soundCrash;
Sound soundLevelUp;
Sound soundSelection;
Sound soundFail;

Music musicBackground;

void loadMedia() {
    Image imageRoads = LoadImage("images/roads.png");
    Image imageCarRed = LoadImage("images/car_red.png");
    Image imageCarOrange = LoadImage("images/car_orange.png");
    Image imageCarYellow = LoadImage("images/car_yellow.png");
    Image imageCarGreen = LoadImage("images/car_green.png");
    Image imageCarBlue = LoadImage("images/car_blue.png");
    Image imageCarIndigo = LoadImage("images/car_indigo.png");
    Image imageCarViolet = LoadImage("images/car_violet.png");
    Image imageCarPink = LoadImage("images/car_pink.png");
    Image imageCarRainbow = LoadImage("images/car_rainbow.png");
    Image imageFuel = LoadImage("images/fuel.png");
    Image imageTitleScreen = LoadImage("images/title_screen.png");
    Image imagePause = LoadImage("images/pause.png");
    Image imagePlay = LoadImage("images/play.png");

    textureRoads = LoadTextureFromImage(imageRoads);
    textureCarRed = LoadTextureFromImage(imageCarRed);
    textureCarOrange = LoadTextureFromImage(imageCarOrange);
    textureCarYellow = LoadTextureFromImage(imageCarYellow);
    textureCarGreen = LoadTextureFromImage(imageCarGreen);
    textureCarBlue = LoadTextureFromImage(imageCarBlue);
    textureCarIndigo = LoadTextureFromImage(imageCarIndigo);
    textureCarViolet = LoadTextureFromImage(imageCarViolet);
    textureCarPink = LoadTextureFromImage(imageCarPink);
    textureCarRainbow = LoadTextureFromImage(imageCarRainbow);
    textureFuel = LoadTextureFromImage(imageFuel);
    textureTitleScreen = LoadTextureFromImage(imageTitleScreen);
    texturePause = LoadTextureFromImage(imagePause);
    texturePlay = LoadTextureFromImage(imagePlay);

    font = LoadFontEx("fonts/font.ttf", 64, nullptr, 0);

    soundCollect = LoadSound("sounds/collect.wav");
    soundCrash = LoadSound("sounds/crash.wav");
    soundLevelUp = LoadSound("sounds/levelup.wav");
    soundSelection = LoadSound("sounds/selection.wav");
    soundFail = LoadSound("sounds/fail.wav");

    musicBackground = LoadMusicStream("sounds/background.wav");
    musicBackground.looping = true;
    
    UnloadImage(imageRoads);
    UnloadImage(imageCarRed);
    UnloadImage(imageCarOrange);
    UnloadImage(imageCarYellow);
    UnloadImage(imageCarGreen);
    UnloadImage(imageCarBlue);
    UnloadImage(imageCarIndigo);
    UnloadImage(imageCarViolet);
    UnloadImage(imageCarPink);
    UnloadImage(imageCarRainbow);
    UnloadImage(imageFuel);
    UnloadImage(imageTitleScreen);
    UnloadImage(imagePause);
    UnloadImage(imagePlay);
}

class Player {
public: 
    Vector2 position;
    Vector2 size;
    float velocityX = 8.0f;
    float initialPauseTime = 3.0f;
    double lastUpdateTime = 0;
    float roadGap = 160.0f;
    Texture2D *currentTexture;
    int lives = 3;

    bool startedMoving = false;
    bool isInSuperState = false;
    bool isDead = false;

    Player(Texture2D *currentTexture) {
        this->position = {50.0f, 22.0f};
        this->currentTexture = currentTexture;
        this->size = {(float)currentTexture->width, (float)currentTexture->height};
    }

    void draw(float cameraOffsetX) {
        DrawTexture(isInSuperState ? textureCarRainbow : *currentTexture, position.x + cameraOffsetX, position.y, WHITE);
    }

    void update() {
        double currentTime = GetTime();
        if (currentTime - lastUpdateTime >= initialPauseTime) {
            startedMoving = true;
            position.x += velocityX;
            if (IsKeyPressed(KEY_S) && position.y < 662.0f) {
                position.y += roadGap;
            }
            if (IsKeyPressed(KEY_W) && position.y > 22.0f) {
                position.y -= roadGap;
            }
        }
        size = {(float)currentTexture->width, (float)currentTexture->height};
    }
};

class CollisionCar {
public:
    Vector2 position;
    Vector2 size;
    Texture2D *currentTexture;
    float velocityX = -8.0f;

    bool shouldBeDestroyed = false;

    CollisionCar(Vector2 position, Texture2D *currentTexture) {
        this->position = position;
        this->currentTexture = currentTexture;
        this->size = {(float)currentTexture->width, (float)currentTexture->height};
    }

    void draw(float cameraOffsetX) {
        this->size = {(float)currentTexture->width, (float)currentTexture->height};
        Rectangle src = {size.x, 0, -size.x, size.y};
        Rectangle dest = {position.x + cameraOffsetX, position.y, size.x, size.y};
        DrawTexturePro(*currentTexture, src, dest, {0, 0}, 0, WHITE);
    }
    void update() {
        position.x += velocityX;
    }
};

class FuelTank {
public: 
    Vector2 position;
    Vector2 size;

    bool shouldBeDestroyed = false;
    
    FuelTank(Vector2 position) {
        this->position = position;
    }

    void draw(float cameraOffsetX) {
        DrawTexture(textureFuel, position.x + cameraOffsetX, position.y, WHITE);
    }
    void update() {
        size = {(float)textureFuel.width, (float)textureFuel.height};
    }
};

class Game {
public: 
    float cameraOffsetX = 0.0f;
    const float cameraFollowX = 25.0f;
    std::vector<CollisionCar> collisionCars = {};
    std::vector<FuelTank> fuelTanks = {};
    double lastUpdateTime = 0;
    double lastFuelUpdateTime = 0;
    double lastSuperStateUpdateTime = 0;
    int numCarsToSpawn = 1;
    int numFuelTanksToSpawn = 1;
    int playerSkinIndex = 0;
    int playerSuperLevel = 0;
    float spawnDelay = 2.0f;
    float fuelSpawnDelay = 7.0f;
    float superStateDuration = 15.0f;
    Texture2D playerSkin;
    std::vector<Texture2D> collisionCarSkins;
    std::vector<Texture2D> carSkins;
    std::vector<int> scores;
    Player player = Player(&playerSkin);
    int score = 0;
    int highScore = 0;

    Rectangle startButtonHitbox = {497, 711, 223, 71};
    Rectangle pausePlayButtonHitbox = {WINDOW_WIDTH - 150, WINDOW_HEIGHT - 150, 100, 100};

    bool shouldSpawnCar = false;
    bool shouldSpawnFuel = false;
    bool isPaused = false;
    bool isInTitleScreen = true;

    Game() {
        InitAudioDevice();
        loadMedia();
        PlayMusicStream(musicBackground);
        SetRandomSeed(time(NULL));
        setSkins();
    }

    void setSkins() {
        carSkins.clear();
        collisionCarSkins.clear();

        carSkins.push_back(textureCarRed);
        carSkins.push_back(textureCarOrange);
        carSkins.push_back(textureCarYellow);
        carSkins.push_back(textureCarGreen);
        carSkins.push_back(textureCarBlue);
        carSkins.push_back(textureCarIndigo);
        carSkins.push_back(textureCarViolet);
        carSkins.push_back(textureCarPink);
        carSkins.push_back(textureCarRainbow);

        playerSkin = carSkins.at(playerSkinIndex);

        collisionCarSkins = carSkins;
        collisionCarSkins.erase(collisionCarSkins.begin() + playerSkinIndex);
    }

    void draw() {
        DrawText("Install textures and sounds and fonts.", 50, 50, 50, WHITE);
        if (isInTitleScreen) { 
            Color boxColor;
            Rectangle comboboxBounds = {WINDOW_WIDTH - 350, WINDOW_HEIGHT - 80, 200, 60};
            DrawTexture(textureTitleScreen, 0, 0, WHITE);
            GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
            GuiSetStyle(COMBOBOX, COMBO_BUTTON_WIDTH, 70);
            GuiSetStyle(COMBOBOX, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
            GuiSetStyle(COMBOBOX, TEXT_COLOR_FOCUSED, ColorToInt(BLACK));
            GuiSetStyle(COMBOBOX, TEXT_COLOR_PRESSED, ColorToInt(BLACK));
            GuiSetFont(font);
            switch (playerSkinIndex) {
                case 0: boxColor = {255, 81, 81, 255}; break;
                case 1: boxColor = {255, 150, 81, 255}; break;
                case 2: boxColor = {255, 216, 81, 255}; break;
                case 3: boxColor = {180, 255, 81, 255}; break;
                case 4: boxColor = {81, 227, 255, 255}; break;
                case 5: boxColor = {81, 90, 255, 255}; break;
                case 6: boxColor = {172, 81, 255, 255}; break;
                case 7: boxColor = {255, 81, 239, 255}; break;
            }
            GuiSetStyle(COMBOBOX, BASE_COLOR_NORMAL, ColorToInt(boxColor));
            GuiSetStyle(COMBOBOX, BASE_COLOR_PRESSED, ColorToInt(boxColor));
            GuiSetStyle(COMBOBOX, BASE_COLOR_FOCUSED, ColorToInt(boxColor));
            static int prevPlayerSkinIndex = 0;
            GuiComboBox(comboboxBounds, "Red;Orange;Yellow;Green;Blue;Indigo;Violet;Pink", &playerSkinIndex);
            playerSkin = carSkins.at(playerSkinIndex);
            player.currentTexture = &playerSkin;
            collisionCarSkins = carSkins;
            collisionCarSkins.erase(collisionCarSkins.begin() + playerSkinIndex);
            if (scores.size() >= 1) {
                DrawTextEx(font, TextFormat("High Score: %i", highScore), {100, WINDOW_HEIGHT - 100}, 40, 1.5, WHITE);
            }
            if (playerSkinIndex != prevPlayerSkinIndex) {
                PlaySound(soundSelection);
                prevPlayerSkinIndex = playerSkinIndex;
            }
        } else {
            float grassTileWidth = textureRoads.width;
            float snappedOffsetX = floorf(-cameraOffsetX);
            float startX = -fmod(snappedOffsetX, grassTileWidth) - grassTileWidth;
            float endX = WINDOW_WIDTH + grassTileWidth;
    
            for (float x = startX; x < endX; x += grassTileWidth) {
                DrawTexture(textureRoads, x, 0, WHITE);
            }
            player.draw(cameraOffsetX);
            for (CollisionCar &collisionCar: collisionCars) {
                collisionCar.draw(cameraOffsetX);
            }
            for (FuelTank &fuelTank: fuelTanks) {
                fuelTank.draw(cameraOffsetX);
            }
            float meterStartingX = WINDOW_WIDTH - 600;
            float meterUnitLen = 50.0f;
            float meterGap = 5.0f;
            DrawRectangleLinesEx({meterStartingX, 40, (meterUnitLen + meterGap) * 5 + meterGap, 75.0f}, 3, WHITE);
            for (int i = 0; i < playerSuperLevel; i++) {
                if (playerSuperLevel >= 5) {
                    playerSuperLevel = 5;
                    DrawRectangleGradientH(meterStartingX + meterGap + (meterGap + meterUnitLen) * i, 45, meterUnitLen, 65, WHITE, YELLOW);
                    continue;
                }
                DrawRectangle(meterStartingX + meterGap + (meterGap + meterUnitLen) * i, 45, meterUnitLen, 65, WHITE);
            }
            if (player.isInSuperState) {
                int secondsLeft = superStateDuration - (GetTime() - lastSuperStateUpdateTime);
                DrawTextEx(font, TextFormat("%i", secondsLeft), {400, 40}, 40, 1.5, WHITE);
            }
            DrawTextEx(font, TextFormat("Score: %i", score), {WINDOW_WIDTH - 230, 30}, 40, 1.5, WHITE);
            DrawTextEx(font, TextFormat("Lives: %i", player.lives), {WINDOW_WIDTH - 230, 75}, 40, 1.5, WHITE);
            DrawTexture(isPaused ? texturePlay : texturePause, pausePlayButtonHitbox.x, pausePlayButtonHitbox.y, WHITE);
        }
    }

    void update() {
        Vector2 mousePos = GetMousePosition();
        if (isInTitleScreen) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, startButtonHitbox)) {
                PlaySound(soundSelection);
                isInTitleScreen = false;
                player.lastUpdateTime = GetTime();
            }
        } else {
            if (!isPaused) {
                static bool hasPlayedLevelUpSound = false;
                UpdateMusicStream(musicBackground);
                player.update();
                for (CollisionCar &collisionCar: collisionCars) {
                    collisionCar.update();
                }
                for (FuelTank &fuelTank: fuelTanks) {
                    fuelTank.update();
                }
                float targetCameraOffsetX = 0.0f;
                
                if (player.position.x > cameraFollowX) {
                    targetCameraOffsetX = cameraFollowX - player.position.x;
                }
                cameraOffsetX += (targetCameraOffsetX - cameraOffsetX) * 0.1f;
                if (player.startedMoving) {
                    double currentTime = GetTime();
                    if (currentTime - lastUpdateTime >= spawnDelay) {
                        lastUpdateTime = currentTime;
                        shouldSpawnCar = true;
                    }
                    if (currentTime - lastFuelUpdateTime >= fuelSpawnDelay) {
                        lastFuelUpdateTime = currentTime;
                        shouldSpawnFuel = true;
                    }
                    if (score > 20) {
                        numCarsToSpawn = 3;
                        numFuelTanksToSpawn = 2;
                        player.velocityX = 10.0f;
                    } else if (score > 10) {
                        numCarsToSpawn = 2;
                        numFuelTanksToSpawn = 1;
                        player.velocityX = 9.0f;
                    } else {
                        numCarsToSpawn = 1;
                        numFuelTanksToSpawn = 1;
                        player.velocityX = 8.0f;
                    }
                    if (player.isDead) {
                        PlaySound(soundFail);
                        reset();
                    }
                }
                if (player.startedMoving && shouldSpawnCar) {
                    std::vector<int> spawnedPositions;
                    for (int i = 0; i < numCarsToSpawn; i++) {
                        int carRow;
                        do {
                            carRow = GetRandomValue(0, 4);
                        } while (std::find(spawnedPositions.begin(), spawnedPositions.end(), carRow) != spawnedPositions.end());
                        int carColorIndex = GetRandomValue(0, 6);
                        Vector2 carPosition = {player.position.x + 1000, 22.0f + (160.0f * carRow)};
                        collisionCars.push_back(CollisionCar(carPosition, &collisionCarSkins.at(carColorIndex)));
                        spawnedPositions.push_back(carRow);
                    }
                    shouldSpawnCar = false;
                }
                if (player.startedMoving && shouldSpawnFuel) {
                    std::vector<int> spawnedPositions;
                    for (int i = 0; i < numFuelTanksToSpawn; i++) {
                        int fuelRow;
                        do {
                            fuelRow = GetRandomValue(0, 4);
                        } while (std::find(spawnedPositions.begin(), spawnedPositions.end(), fuelRow) != spawnedPositions.end());
                        Vector2 fuelPosition = {player.position.x + 1000, 22.0f + (160.0f * fuelRow)};
                        fuelTanks.push_back(FuelTank(fuelPosition));
                        spawnedPositions.push_back(fuelRow);
                    }
                    shouldSpawnFuel = false;
                }
                if (player.isInSuperState) {
                    double currentTime = GetTime();
                    static bool hasResetUpdateTime = false;
                    if (!hasResetUpdateTime) {
                        lastSuperStateUpdateTime = currentTime;
                        hasResetUpdateTime = true;
                    }
                    if (currentTime - lastSuperStateUpdateTime > superStateDuration) {
                        playerSuperLevel = 0;
                        hasPlayedLevelUpSound = false;
                        player.isInSuperState = false;
                        hasResetUpdateTime = false;
                    }
                    player.lives = 3;
                }
                if (playerSuperLevel >= 5) {
                    playerSuperLevel = 5;
                    player.isInSuperState = true;
                }
                if (player.isInSuperState) {
                    if (!hasPlayedLevelUpSound) {
                        PlaySound(soundLevelUp);
                        hasPlayedLevelUpSound = true;
                    }
                }
        
                checkForRemoval();
                garbageCollect();
                checkForCollisions();
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, pausePlayButtonHitbox)) {
                PlaySound(soundSelection);
                isPaused = !isPaused;
            }
        }
    }
    void checkForRemoval() {
        for (CollisionCar &collisionCar: collisionCars) {
            if (collisionCar.position.x < player.position.x - 400.0f) {
                collisionCar.shouldBeDestroyed = true;
            }
        }
        for (FuelTank &fuelTank: fuelTanks) {
            if (fuelTank.position.x < player.position.x - 400.0f) {
                fuelTank.shouldBeDestroyed = true;
            }
        }
    }
    void garbageCollect() {
        for (int i = 0; i < collisionCars.size(); i++) {
            if (collisionCars.at(i).shouldBeDestroyed) {
                collisionCars.erase(collisionCars.begin() + i);
                i--;
            }
        }
        for (int i = 0; i < fuelTanks.size(); i++) {
            if (fuelTanks.at(i).shouldBeDestroyed) {
                fuelTanks.erase(fuelTanks.begin() + i);
                i--;
            }
        }
    }
    void checkForCollisions() {
        for (CollisionCar &collisionCar: collisionCars) {
            if (CheckCollisionRecs({player.position.x, player.position.y, player.size.x, player.size.y}, {collisionCar.position.x, collisionCar.position.y, collisionCar.size.x, collisionCar.size.y})) {
                PlaySound(soundCrash);
                if (player.isInSuperState) {
                    score++;
                    collisionCar.shouldBeDestroyed = true;
                    continue;
                }
                if (player.lives > 0) {
                    player.lives--;
                }
                if (player.lives <= 0) {
                    player.isDead = true;
                }
                collisionCar.shouldBeDestroyed = true;
            }
        }
        for (FuelTank &fuelTank: fuelTanks) {
            if (CheckCollisionRecs({player.position.x, player.position.y, player.size.x, player.size.y}, {fuelTank.position.x, fuelTank.position.y, fuelTank.size.x, fuelTank.size.y})) {
                PlaySound(soundCollect);
                score++;
                playerSuperLevel++;
                fuelTank.shouldBeDestroyed = true;
            }
        }
    }
    void reset() {
        player.isDead = false;
        isInTitleScreen = true;
        isPaused = false;
        scores.push_back(score);
        setHighScore();
        score = 0;
        player.lives = 3;
        playerSuperLevel = 0;
        player.isInSuperState = false;
        player.startedMoving = false;
        collisionCars.clear();
        fuelTanks.clear();
        player.position = {50.0f, 22.0f};
        cameraOffsetX = 0;
        numFuelTanksToSpawn = 1;
    }
    void setHighScore() {
        int highScore_l = 0;
        for (int current = 0; current < scores.size(); current++) {
            if (scores.at(current) >= highScore_l) {
                highScore_l = scores.at(current);
            }
        }
        highScore = highScore_l;
    }
};

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dodge The Cars");
    SetTargetFPS(60);

    Game game = Game();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        game.update();
        game.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}