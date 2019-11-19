#include "Level2.h"

#define LEVEL2_WIDTH 38
#define LEVEL2_HEIGHT 16

unsigned int level2_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 3, 3, 3, 3, 3, 0, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 0, 0, 1, 1, 1, 0, 0, 5, 0, 3, 3, 3, 3, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 1, 0, 7, 7, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 7, 0, 0, 0, 7, 3,
    3, 0, 0, 0, 0, 7, 0, 7, 7, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 7, 7, 0, 0, 0, 0, 7, 1, 1, 0, 0, 1, 2, 0, 0, 3, 3, 3, 3, 3, 3,
    3, 0, 0, 0, 7, 1, 0, 0, 1, 1, 2, 0, 0, 2, 7, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 0, 0, 2, 0, 0, 0, 3, 3, 3, 3, 3, 3,
    3, 1, 1, 1, 1, 2, 1, 1, 2, 2, 2, 0, 0, 2, 0, 0, 0, 5, 1, 1, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 1, 1, 1, 1, 2, 2, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
};


void Level2::Initialize() {
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);
    
    state.player.entityType = PLAYER;
    state.player.isStatic = false;
    state.player.isActive = true;
    state.player.position = glm::vec3(3, -4, 0);
    state.player.acceleration = glm::vec3(0, -22.0f, 0);
    state.player.textureID = Util::LoadTexture("george.png");
    state.player.cols = 4;
    state.player.rows = 4;
    state.player.animFrames = 4;
    state.player.animLeftIndices = new int[4] {1, 5, 9, 13};
    state.player.animRightIndices = new int[4] {3, 7, 11, 15};
    state.nextLevel = -1;
    
    GLuint coinTextureID = Util::LoadTexture("coin.png");
    GLuint enemyTextureID = Util::LoadTexture("evil.png");
    int levelCoins = 0;
    int levelEnemies = 0;
    
    for (int y = 0; y < LEVEL2_HEIGHT; y++) {
        for (int x = 0; x < LEVEL2_WIDTH; x++) {
            if (level2_data[y * LEVEL2_WIDTH + x] == 7) {
                state.coins[levelCoins].entityType = COIN;
                state.coins[levelCoins].isStatic = false;
                state.coins[levelCoins].isActive = true;
                state.coins[levelCoins].position = glm::vec3(x, -y, 0);
                state.coins[levelCoins].width = 0.5;
                state.coins[levelCoins].height = 0.5;
                state.coins[levelCoins].textureID = coinTextureID;
                state.coins[levelCoins].cols = 6;
                state.coins[levelCoins].rows = 1;
                state.coins[levelCoins].animFrames = 6;
                state.coins[levelCoins].animRightIndices = new int[6] {0, 1, 2, 3, 4, 5};
                state.coins[levelCoins].direction = 1;
                levelCoins++;
            }
            else if (level2_data[y * LEVEL2_WIDTH + x] == 4 || level2_data[y * LEVEL2_WIDTH + x] == 5) {
                state.enemies[levelEnemies].entityType = ENEMY;
                state.enemies[levelEnemies].isStatic = false;
                state.enemies[levelEnemies].isActive = true;
                state.enemies[levelEnemies].position = glm::vec3(x, -y, 0);
                state.enemies[levelEnemies].textureID = enemyTextureID;
                state.enemies[levelEnemies].direction = 1;
                state.enemies[levelEnemies].acceleration = glm::vec3(0, -22.0f, 0);
                if (level2_data[y * LEVEL2_WIDTH + x] == 5) {
                    state.enemies[levelEnemies].canJump = false;
                }
                levelEnemies++;
            }
        }
    }
}

void Level2::Update(float deltaTime) {
    if (GameOver == 0) {
        state.player.Update(deltaTime, state.enemies, MAX_ENEMIES, state.coins, MAX_COIN_COUNT, state.map);

        for (int i = 0; i < MAX_COIN_COUNT; i++) {
            if (state.coins[i].isActive) state.coins[i].Update(deltaTime, NULL, 0, NULL, 0, state.map);
        }
        
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (state.enemies[i].isActive) {
                state.enemies[i].Update(deltaTime, NULL, 0, NULL, 0, state.map);
                state.enemies[i].CheckDistanceToPlayer(state.player);
            }
        }
    }
    if (state.player.position.y < -15) {
        state.lives -= 1;
        this->Initialize();
    }
    else if (state.player.loseLife) {
        state.player.loseLife = false;
        state.lives -= 1;
        this->Initialize();
    }
    if (state.lives == 0) GameOver = -1;
    
    bool coinsLeft = false;
    for (int i = 0; i < MAX_COIN_COUNT; i++) {
        if(state.coins[i].isActive) {
            coinsLeft = true;
            break;
        }
    }
    if (!coinsLeft) state.nextLevel = 3;
}

void Level2::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player.Render(program);
    
    for (int i = 0; i < MAX_COIN_COUNT; i++) {
        if (state.coins[i].isActive) state.coins[i].Render(program);
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (state.enemies[i].isActive) state.enemies[i].Render(program);
    }
    
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, to_string(state.lives) + " Lives", 0.5f, -0.25f, glm::vec3(state.player.position.x -4.5, state.player.position.y + 3.5, 0));
    
    if (GameOver == -1) {
        Util::DrawText(program, fontTextureID, "YOU LOSE", 1.0f, -0.5f, glm::vec3(state.player.position.x -2, state.player.position.y + 2, 0));
    }
}
